#include "IPlugin.h"
#include <iostream>

using namespace std;

class PrintHelloChina: public IPlugin
{
public:
    const char* GetID() const override
    {
        return "hellochina";
    }
    const char* Description() const override
    {
        return "输出Hello China";
    }
    void Execute() const override
    {
        cout << "Hello China!" << endl;
    }
};
extern "C" void CreateObj(IPlugin ** ppPlugin)
{
    static PrintHelloChina entry;
    *ppPlugin = &entry;
}
// extern "C" const char* GetPluginID()
// {
//     return "HelloChina";
// }

// extern "C" const char* GetPluginDescription()
// {
//     return "输出Hello China";
// }

// extern "C" void Execute()
// {
//     cout << "Hello China!" << endl;
// }