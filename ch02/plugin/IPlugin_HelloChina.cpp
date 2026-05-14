#include <iostream>

using namespace std;

extern "C" const char* GetPluginID()
{
    return "HelloChina";
}

extern "C" const char* GetPluginDescription()
{
    return "输出Hello China";
}

extern "C" void Execute()
{
    cout << "Hello China!" << endl;
}