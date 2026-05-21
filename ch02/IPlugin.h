#pragma once
#include <functional>
#include <string>

class IPlugin
{
public:
    virtual ~IPlugin() = default;
    virtual void Execute() const = 0;
    virtual const char* Description() const = 0;
    virtual const char* GetID() const = 0;
};
using FUNC_CREATE_OBJ = void (*)(IPlugin**);

struct PluginInfo {
    void* handle;
    IPlugin* plugin;
    PluginInfo(void* handle, IPlugin* plugin):handle(handle), plugin(plugin){}
};