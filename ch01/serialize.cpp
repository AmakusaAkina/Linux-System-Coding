#include "serialize.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

#define FILE_PERM_ALL (S_IRWXU | S_IRWXG | S_IRWXO)

bool A::Serialize(int fd) const
{
    if (fd == -1) return false;
    int r = write(fd, &i, sizeof(int));
    if (r != sizeof(int)) return false;
    r = write(fd, &c, sizeof(char));
    if (r != sizeof(char)) return false;
    return true;
}
bool A::Deserialize(int fd)
{
    if (fd == -1) return false;
    int r = read(fd, &i, sizeof(int));
    if (r != sizeof(int)) return false;
    r = read(fd, &c, sizeof(char));
    if (r != sizeof(char)) return false;
    return true;
}
bool A::Serialize(const char *pFilePath)
{
    int fd = open(pFilePath, O_WRONLY | O_CREAT | O_APPEND, FILE_PERM_ALL);
    if (fd == -1)
    {
        perror("open error");
        return false;
    }
    Serialize(fd);
    close(fd);
    return true;
}
bool A::Deserialize(const char *pFilePath)
{
    int fd = open(pFilePath, O_RDONLY);
    if (fd == -1)
    {
        perror("open error");
        return false;
    }
    Deserialize(fd);
    close(fd);
    return true;
}
void A::f()
{
    PutI();
    PutC();
    fprintf(stdout, "\n");
}  
int A::GetI()
{
    return i;
}
void A::PutI()
{
    fprintf(stdout, "%d ", i);
}
void A::PutC()
{
    fprintf(stdout, "%c ", c);
}

bool B::Serialize(int fd) const
{
    return write(fd, &i, sizeof(float)) == sizeof(float);
}
bool B::Deserialize(int fd)
{
    return read(fd, &i, sizeof(float)) == sizeof(float);
}
bool B::Serialize(const char *pFilePath)
{
    int fd = open(pFilePath, O_WRONLY | O_CREAT | O_APPEND, FILE_PERM_ALL);
    if (fd == -1)
    {
        perror("open error");
        return false;
    }
    Serialize(fd);
    close(fd);
    return true;
}
bool B::Deserialize(const char *pFilePath)
{
    int fd = open(pFilePath, O_RDONLY);
    if (fd == -1)
    {
        perror("open error");
        return false;
    }
    Deserialize(fd);
    close(fd);
    return true;
}
void B::f()
{
    fprintf(stdout, "%f\n", i);
}
    
float B::GetI()
{
    return i;
}

bool Serializer::Serialize(const char *pFilePath, const vector<A> &v)
{
    int fd = open(pFilePath, O_WRONLY | O_CREAT | O_TRUNC, FILE_PERM_ALL);
    if (fd == -1)
    {
        perror("Serializer open error");
        return false;
    }

    for (auto &item : v)
    {
        item.Serialize(fd);
    }

    close(fd);
    return true;
}

bool Serializer::Deserialize(const char *pFilePath, vector<A> &v)
{
    int fd = open(pFilePath, O_RDONLY);
    if (fd == -1)
    {
        perror("Deserialize open error");
        return false;
    }

    while (true)
    {
        A tmp;
        if (!tmp.Deserialize(fd))
            break;

        v.push_back(tmp);
    }

    close(fd);
    return true;
}
bool Serializer::Serialize(const char* pFilePath, vector<Serialized>& v)
{
    int fd = open(pFilePath, O_WRONLY | O_CREAT | O_TRUNC, FILE_PERM_ALL);
    if (fd == -1)
    {
        perror("Serializer open error");
        return false;
    }

    for(auto &item : v)
    {
        write(fd, &item.nType, sizeof(int));
        if(item.nType == 0)
            ((A*)item.pObj)->Serialize(fd);
        else if(item.nType == 1)
            ((B*)item.pObj)->Serialize(fd);
    }

    close(fd);

    return true;
}
bool Serializer::Deserialize(const char* pFilePath, vector<Serialized>& v)
{
    int fd = open(pFilePath, O_RDONLY);
    if (fd == -1)
    {
        perror("Deserialize open error");
        return false;
    }

    while(true)
    {
        Serialized item;
        int r = read(fd, &item.nType, sizeof(int));

        if(r != sizeof(int)) break;

        if(item.nType == 0) {
            item.pObj = new A();
            ((A*)item.pObj)->Deserialize(fd);
        } else if (item.nType == 1) {
            item.pObj = new B();
            ((B*)item.pObj)->Deserialize(fd);
        }
        v.push_back(item);
    }

    close(fd);

    return true;
}