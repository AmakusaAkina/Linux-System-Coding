#pragma once
#include "IPlugin.h"
#include <vector>
#include <unordered_map>
class PluginManager 
{
public:
    PluginManager();
    PluginManager(std::string dirpath);
    virtual ~PluginManager();
    bool GetPluginPaths(std::string plugin_dir);
    void LoadPlugins();
    void List();
    void Run(std::string cmd);
private:
    std::vector<std::string> paths;
    std::vector<PluginInfo> plugins;
    std::unordered_map<std::string, std::function<void()>> registry;
};