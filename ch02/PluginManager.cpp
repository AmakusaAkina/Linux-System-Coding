#include "PluginManager.h"
#include <dirent.h>
#include <iostream>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
using namespace std;

PluginManager::PluginManager()
{
    auto func_help = [this](){this->List();};
    plugins.emplace_back(
        "help",
        "显示所有插件",
        nullptr,
        func_help
    );
    registry["help"] = func_help;
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
        }
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

        FUNC_GET_ID get_id = (FUNC_GET_ID)dlsym(handle, "GetPluginID");
        FUNC_GET_DESC get_desc = (FUNC_GET_DESC)dlsym(handle, "GetPluginDescription");
        FUNC_EXECUTE execute = (FUNC_EXECUTE)dlsym(handle, "Execute");

        if (!get_id || !get_desc || !execute) {
            dlclose(handle);
            continue;
        }

        plugins.emplace_back(get_id(), get_desc(), handle, execute);
        registry[get_id()] = execute;
    }
}

void PluginManager::List()
{
    for (const auto& p : plugins) {
        cout << p.id << ": " << p.desc << endl;
    }
}

void PluginManager::Run(const string& cmd)
{
    auto it = registry.find(cmd);
    if (it == registry.end()) {
        cerr << "Unknown cmd" << endl << "help 查看有效cmd" << endl;
        return;
    }

    it->second();
}