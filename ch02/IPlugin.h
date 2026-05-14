#pragma once
#include <functional>
#include <string>

using FUNC_GET_ID = const char* (*)();

using FUNC_GET_DESC = const char* (*)();

using FUNC_EXECUTE = void (*)();

struct PluginInfo {
    std::string id;
    std::string desc;
    void* handle;
    std::function<void()> execute;
    PluginInfo(std::string i, std::string d, void* h, std::function<void()> e)
    :id(i), desc(d), handle(h), execute(e){} 
};