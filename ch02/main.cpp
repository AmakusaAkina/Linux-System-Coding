#include <dlfcn.h>
#include <iostream>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

using namespace std;

typedef void (*FUNC_PRINT)();

int main() {
    string plugin_dir = "../plugin";

    DIR* dir = opendir(plugin_dir.c_str());
    if (!dir) {
        cerr << "Cannot open directory: " << plugin_dir << endl;
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;

        if (filename == "." || filename == "..") continue;

        if (filename.size() >= 3 && filename.substr(filename.size() - 3) == ".so") {
            string path = plugin_dir + "/" + filename;

            // 检查文件可读
            int fd = open(path.c_str(), O_RDONLY);
            if (fd == -1) {
                cerr << "Cannot open file: " << path << endl;
                continue;
            }
            close(fd);

            // cout << "Loading: " << path << endl;

            void* handle = dlopen(path.c_str(), RTLD_LAZY);
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
    }

    closedir(dir);
    return 0;
}