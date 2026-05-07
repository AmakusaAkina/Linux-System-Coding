#include "serialize.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

#define FILE_PERM_ALL (S_IRWXU | S_IRWXG | S_IRWXO)

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

const std::unordered_map<int, Serializable::Creator> Serializable::factory = {
    {type::A, []() { return std::make_unique<A>(); }},
    {type::B, []() { return std::make_unique<B>(); }},
    {type::C, []() { return std::make_unique<C>(); }},
};
unique_ptr<Serializable> Serializable::Create(int i)
{
    auto it = factory.find(i);
    if (it == factory.end())
        return nullptr;
    return it->second();
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
bool Serializer::Serialize(const char* pFilePath,const vector<Serialized>& v)
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

        if (r == 0) break;
        if(r != sizeof(int)) return false;

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

bool Serializer::Serialize(const char* pFilePath, const vector<unique_ptr<Serializable>>& v)
{
    int fd = open(pFilePath, O_WRONLY | O_CREAT | O_TRUNC, FILE_PERM_ALL);
    if (fd == -1)
    {
        perror("Serializer open error");
        close(fd);
        return false;
    }
    for (auto &sample : v)
    {
        int type = sample->GetIndex();
        if (write(fd, &type, sizeof(int)) != sizeof(int))
        {
            close(fd);
            return false;
        }
            
        sample->Serialize(fd);
    }
    close(fd);
    return true;
}
bool Serializer::Deserialize(const char* pFilePath, vector<unique_ptr<Serializable>>& v)
{
    int fd = open(pFilePath, O_RDONLY);
    if (fd == -1)
    {
        perror("Deserialize open error");
        return false;
    }

    while(true)
    {
        int type;
        int r = read(fd, &type, sizeof(int));
        if (r == 0) break;
        if(r != sizeof(int)) 
        {
            close(fd);
            return false;
        }
        auto obj = Serializable::Create(type);
        if (!obj) 
        {
            fprintf(stderr, "Unknown type ID: %d\n", type);
            close(fd);
            return false;
        }
        if (!obj->Deserialize(fd))
        {
            close(fd);
            return false;
        }
        v.push_back(move(obj));
    }
    close(fd);
    return true;
}