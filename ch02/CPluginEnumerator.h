#pragma once
#include <vector>
#include <string>
class CPluginEnumerator 
{
public:
    CPluginEnumerator();
    virtual ~CPluginEnumerator();
    bool GetPluginNames(std::vector<std::string>& names);
};