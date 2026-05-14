#include <iostream>

using namespace std;

extern "C" const char* GetPluginID()
{
    return "HelloWorld";
}

extern "C" const char* GetPluginDescription()
{
    return "输出Hello World";
}

extern "C" void Execute()
{
    cout << "Hello World!" << endl;
}