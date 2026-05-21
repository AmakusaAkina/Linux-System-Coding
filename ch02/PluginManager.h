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
    void List() const;
    void Run(const std::string& cmd) const;
private:
    class PluginHelp : public IPlugin
    {
        PluginManager* manager;
    public:
        PluginHelp(PluginManager* manager):manager(manager){}
        const char* GetID() const override
        {
            return "help";
        }
        const char* Description() const override
        {
            return "显示所有插件";
        }
        void Execute() const override
        {
            manager->List();
        }
    };
    std::vector<PluginInfo> plugins;
    std::unordered_map<std::string, IPlugin*> registry;

    std::vector<std::string> GetPluginPaths(const std::string& plugin_dir);
};