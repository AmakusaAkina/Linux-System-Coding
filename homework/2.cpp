#include <dlfcn.h>
#include <iostream>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

using namespace std;

// 递归遍历目录，打印层次结构
void list(const string &path, const string &ignoreFolder, int level = 0) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        cerr << "Cannot open directory: " << path << endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;

        if (filename == "." || filename == "..") continue;

        string fullPath = path + "/" + filename;

        // 缩进层次
        string indent(level * 2, ' ');

        // 判断路径是否是目录
        if (entry->d_type != DT_DIR) {
            // 非文件夹打印文件名
            cout << indent << filename << endl;
            continue;
        }

        // 判断路径是否是要求不展开的文件夹
        if (filename == ignoreFolder) {
            // 精准匹配忽略的文件夹，只打印文件名
            cout << indent << filename << endl;
            continue;
        }

        // 打印目录名
        cout << indent << filename << "/" << endl;

        // 递归调用，层级 +1
        list(fullPath, ignoreFolder, level + 1);
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <directory_path> [ignore_folder]" << endl;
        return 1;
    }

    string path = argv[1];
    string ignoreFolder = "";
    if (argc >= 3) {
        ignoreFolder = argv[2];  // 第二个参数为要忽略的文件夹名字
    }

    list(path, ignoreFolder);
    return 0;
}