#pragma once
#include "IPlugin.h"
#include <vector>
#include <unordered_map>
class PluginManager 
{
public:
    PluginManager();
    PluginManager(const std::string& dirpath);
    virtual ~PluginManager();
    void LoadPlugins(const std::string& dir);
    void List();
    void Run(const std::string& cmd);
private:
    std::vector<PluginInfo> plugins;
    std::unordered_map<std::string, std::function<void()>> registry;

    std::vector<std::string> GetPluginPaths(const std::string& plugin_dir);
};