#include "PluginManager.h"
#include "IPlugin.h"
#include <iostream>
#include <dirent.h>
#include <fcntl.h>


using namespace std;

int main(int argc, char** argv) {

    if (argc < 2) {
        cout << "./a.out help 以显示所有" << endl;
        return -1;
    }

    PluginManager pluginManager("../plugin");

    string cmd = argv[1];

    pluginManager.Run(cmd);

    return 0;
}