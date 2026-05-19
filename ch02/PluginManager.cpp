#include "PluginManager.h"
#include <dirent.h>
#include <iostream>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
using namespace std;

PluginManager::PluginManager()
{
    IPlugin* help = new PluginHelp(this);
    plugins.emplace_back(nullptr, help);
    registry[help->GetID()] = help;
}
PluginManager::PluginManager(const string& dirpath):PluginManager()
{
    LoadPlugins(dirpath);
}
PluginManager::~PluginManager()
{
    for (auto& p : plugins) {
        if(p.handle) {
            dlclose(p.handle);
            continue;
        }
        delete(p.plugin);   //  处理builtin help
    }
}
vector<string> PluginManager::GetPluginPaths(const string& plugin_dir)
{
    vector<string> paths;
    DIR* dir = opendir(plugin_dir.c_str());
    if (!dir) {
        cerr << "Cannot open directory: " << plugin_dir << endl;
        paths.clear();
        return paths;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;

        if (filename == "." || filename == "..") continue;

            if (filename.size() >= 3 && filename.substr(filename.size() - 3) == ".so") {
            string path = plugin_dir + "/" + filename;
            paths.push_back(path);
        }
    }
    closedir(dir);
    return paths;
}

void PluginManager::LoadPlugins(const string& dir)
{
    std::vector<std::string> paths;
    paths = GetPluginPaths(dir);
    for (const auto& path : paths) {
        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) {
            cerr << "dlopen error: " << dlerror() << endl;
            continue;
        }

        FUNC_CREATE_OBJ create_obj = (FUNC_CREATE_OBJ)dlsym(handle, "CreateObj");
        if (!create_obj) {
            dlclose(handle);
            continue;
        }

        IPlugin* plugin = nullptr;
        create_obj(&plugin);
        if (!plugin) {
            cerr << "CreateObj Failed" << endl;
            dlclose(handle);
            continue;
        }

        plugins.emplace_back(handle, plugin);
        registry[plugin->GetID()] = plugin;
    }
}

void PluginManager::List() const
{
    for (const auto& p : plugins) {
        cout << p.plugin->GetID() << ": " << p.plugin->Description() << endl;
    }
}

void PluginManager::Run(const string& cmd) const
{
    auto it = registry.find(cmd);
    if (it == registry.end()) {
        cerr << "Unknown cmd" << endl << "help 查看有效cmd" << endl;
        return;
    }

    it->second->Execute();
}