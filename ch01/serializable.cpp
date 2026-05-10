#include "serialize.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

using namespace std;

bool A::Serialize(int fd) const
{
    int r = write(fd, &i, sizeof(int));
    if (r != sizeof(int)) return false;
    r = write(fd, &c, sizeof(char));
    if (r != sizeof(char)) return false;
    return true;
}
bool A::Deserialize(int fd)
{
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

    if (write(fd, &type::A, sizeof(int)) != sizeof(int)) 
        return false;

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

    int type;
    if (read(fd, &type, sizeof(int)) != sizeof(int)) 
        return false;
    if (type != type::A) return false;

    Deserialize(fd);
    close(fd);
    return true;
}
int A::GetIndex()
{
    return type::A;
}
void A::PutInfo()
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

    if (write(fd, &type::B, sizeof(int)) != sizeof(int)) 
        return false;

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

    int type;
    if (read(fd, &type, sizeof(int)) != sizeof(int)) 
        return false;
    if (type != type::B) return false;

    Deserialize(fd);
    close(fd);
    return true;
}
int B::GetIndex()
{
    return type::B;
}
void B::PutInfo()
{
    fprintf(stdout, "%f\n", i);
}
    
float B::GetI()
{
    return i;
}

bool C::Serialize(int fd) const 
{
    int len = s.length();
    if (write(fd, &len, sizeof(int)) != sizeof(int)) return false;
    if (write(fd, s.c_str(), len) != len) return false;
    return true;
}
bool C::Deserialize(int fd) 
{
    int len;
    if (read(fd, &len, sizeof(int)) != sizeof(int)) return false;
    if (len < 0) return false;
    s.resize(len);
    if (read(fd, &s[0], len) != len) return false;
    return true;
}
int C::GetIndex()
{
    return type::C;
}
void C::PutInfo()
{
    fprintf(stdout, "%s\n", s.c_str());
}