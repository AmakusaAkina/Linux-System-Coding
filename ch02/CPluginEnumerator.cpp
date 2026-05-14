#include "CPluginEnumerator.h"
#include <dirent.h>
#include <iostream>
using namespace std;

CPluginEnumerator::CPluginEnumerator()
{
}
CPluginEnumerator::~CPluginEnumerator()
{
}

bool CPluginEnumerator::GetPluginNames(std::vector<std::string>& names)
{
    string plugin_dir = "../plugin";

    DIR* dir = opendir(plugin_dir.c_str());
    if (!dir) {
        cerr << "Cannot open directory: " << plugin_dir << endl;
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;

        if (filename == "." || filename == "..") continue;

            if (filename.size() >= 3 && filename.substr(filename.size() - 3) == ".so") {
            string path = plugin_dir + "/" + filename;
            names.push_back(path);
        }
    }
    closedir(dir);
    return true;
}