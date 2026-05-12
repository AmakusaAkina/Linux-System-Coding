#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include <dirent.h>
#include <iomanip>
#include <pwd.h>
#include <grp.h>
#include <sstream>
using namespace std;

// 格式化输出stat信息
string FormatStat(const struct stat& s, const char* filename)
{
    stringstream ss;
    // 文件类型
    if (S_ISDIR(s.st_mode))     //  文件夹
        ss << "d";
    else if (S_ISLNK(s.st_mode))//  链接
        ss << "l";
    else                        //  普通文件
        ss << "-";

    // 用户权限
    ss << ((s.st_mode & S_IRUSR) ? "r" : "-");
    ss << ((s.st_mode & S_IWUSR) ? "w" : "-");
    ss << ((s.st_mode & S_IXUSR) ? "x" : "-");

    // 组权限
    ss << ((s.st_mode & S_IRGRP) ? "r" : "-");
    ss << ((s.st_mode & S_IWGRP) ? "w" : "-");
    ss << ((s.st_mode & S_IXGRP) ? "x" : "-");

    // 其他用户权限
    ss << ((s.st_mode & S_IROTH) ? "r" : "-");
    ss << ((s.st_mode & S_IWOTH) ? "w" : "-");
    ss << ((s.st_mode & S_IXOTH) ? "x" : "-");

    // 硬链接数
    ss << " " << setw(2) << s.st_nlink;

    // 用户名
    passwd* pw = getpwuid(s.st_uid);
    if (pw)
        ss << " " << setw(6) << pw->pw_name;

    // 组名
    group* gr = getgrgid(s.st_gid);
    if (gr)
        ss << " " << setw(6) << gr->gr_name;

    // 文件大小
    ss << " " << setw(8) << s.st_size;

    // 修改时间
    char timebuf[64];
    tm* tm_info = localtime(&s.st_mtime);

    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);

    ss << " " << timebuf;

    ss << " " << filename;

    ss << endl;

    return ss.str();
}

//  ls -l
int main(int argc, char** argv)
{
    //  TODO:查找本目录下所有的文件
    vector<string> filenames;
    DIR* dir = opendir(".");
    if (!dir) {
        cerr << "Cannot open directory" << endl;
        return -1;
    }
    struct dirent* entry;
    while((entry = readdir(dir)) != nullptr) {
        //  ignore . & ..
        string filename = entry->d_name;
        if (filename == "." || filename == "..") continue;
        filenames.emplace_back(entry->d_name);
    }
    closedir(dir);

    //  TODO:遍历filenames查询文件信息并格式化
    struct stat st;         //  stat buffer
    vector<string> output;  //  output buffer
    int blocks = 0;         //  blocks count
    for(auto& entry : filenames){
        if (-1 == stat(entry.c_str(), &st)) {
            cerr << "Cannot read entry" << endl;
            continue;
        }
        //  统计blocks
        blocks += st.st_blocks;
        //  格式化保存stat信息
        output.push_back(FormatStat(st, entry.c_str()));
    }

    //  TODO:打印格式化输出
    cout << "total " << blocks/2  << endl;  //  1mb->512kb
    for(auto& entry : output)
        cout << entry;

    return 0;
}