#include "CPluginEnumerator.h"
#include <dlfcn.h>
#include <iostream>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

typedef void (*FUNC_PRINT)();

int main() {
    CPluginEnumerator enumerator;
    vector<string> pluginnames;
    if (!enumerator.GetPluginNames(pluginnames)) {
        cerr << "Cannot Get Plugin Names" << endl;
    }
    
    for (auto name : pluginnames) {
                // 检查文件可读
        int fd = open(name.c_str(), O_RDONLY);
        if (fd == -1) {
            cerr << "Cannot open file: " << name << endl;
            continue;
        }
        close(fd);

        void* handle = dlopen(name.c_str(), RTLD_LAZY);
        if (!handle) {
            cerr << "dlopen error: " << dlerror() << endl;
            continue;
        }

        FUNC_PRINT dl_print = (FUNC_PRINT)dlsym(handle, "Print");
        if (!dl_print) {
            cerr << "dlsym error: " << dlerror() << endl;
            dlclose(handle);
            continue;
        }

        dl_print();
        dlclose(handle);
    }

    return 0;
}