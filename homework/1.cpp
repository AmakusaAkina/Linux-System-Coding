#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <string>

using namespace std;
#define FILE_PERM_ALL (S_IRWXU | S_IRWXG | S_IRWXO)
int main()
{
    //  O_APPEND打开文件测试
    int fd = open("1.txt", O_RDWR | O_APPEND | O_CREAT | O_TRUNC, FILE_PERM_ALL);
    if (-1 == fd) {
        fprintf(stderr, "Open File Error\n");
        return -1;
    }
    //  查看当前f_pos
    int f_pos = lseek(fd, 0, SEEK_CUR);
    fprintf(stdout, "f_pos: %d\n", f_pos);

    //  第一次写入
    const char* t = "11111";
    write(fd, t, 5);

    //  查看第一次写入后的f_pos
    f_pos = lseek(fd, 0, SEEK_CUR);
    fprintf(stdout, "f_pos: %d\n", f_pos);

    //  将f_pos设置到文件开始
    f_pos = lseek(fd, 1, SEEK_SET);
    fprintf(stdout, "f_pos: %d\n", f_pos);

    //  第二次写入
    t = "22222";
    write(fd, t, 5);

    //  查看写入后的f_pos
    f_pos = lseek(fd, 0, SEEK_CUR);
    fprintf(stdout, "f_pos: %d\n", f_pos);

    close(fd);
    return 0;
}