#include "IPlugin.h"
#include <iostream>

using namespace std;

class PrintHelloWorld: public IPlugin
{
public:
    const char* GetID() const override
    {
    return "helloworld";
    }
    const char* Description() const override
    {
        return "输出Hello World";
    }
    void Execute() const override
    {
        cout << "Hello World!" << endl;
    }
};

extern "C" void CreateObj(IPlugin ** ppPlugin)
{
    static PrintHelloWorld entry;
    *ppPlugin = &entry;
}
// extern "C" const char* GetPluginID()
// {
//     return "HelloWorld";
// }

// extern "C" const char* GetPluginDescription()
// {
//     return "输出Hello World";
// }

// extern "C" void Execute()
// {
//     cout << "Hello World!" << endl;
// }