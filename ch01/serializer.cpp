#include "serialize.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

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

bool Serializer::Serialize(const vector<unique_ptr<Serializable>>& v)
{
    //  TODO: 遍历serializeMap构建<filename, fd> dict 构建<typeIndex, fd>dict
    //  TODO: 遍历v, 依次写入文件
    //  TODO: 遍历dict, 关闭所有打开的文件
    unordered_map<int, int>dict;
    bool openfile = true;
    for(auto &entry : serializeMap)
    {
        auto it = dict.find(entry.first);
        if (it == dict.end())
        {
            int fd = open(entry.second, O_WRONLY | O_CREAT | O_TRUNC, FILE_PERM_ALL);
            if (fd == -1)
            {
                perror("Serializer open error");
                openfile = false;
                break;
            }
            dict.insert({entry.first, fd});
        }
    }

    bool success = true;
    for (auto &sample : v)
    {
        int type = sample->GetIndex();
        auto it = dict.find(type);
        if (it == dict.end())
        {
            fprintf(stderr, "Skip: Unregistered Type\n");
            continue;
        }
        int fd = it->second;
        if (write(fd, &type, sizeof(int)) != sizeof(int))
        {
            fprintf(stderr, "Write Type Error\n");
            success = false;
            break;
        }
            
        if (!sample->Serialize(fd))
        {
            fprintf(stderr, "Write Error\n");
            success = false;
            break;
        }
    }

    bool closefile = true;
    for(auto & entry : dict)
    {
        if (-1 == close(entry.second))
        {
            closefile = false;
            fprintf(stderr, "Close File Failed\n");
        }
    }
        
    return openfile && success && closefile;
}

bool Serializer::SerializeRegister(Serializable* obj, const char* pFilePath)
{
    return SerializeRegister(obj->GetIndex(), pFilePath);
}
bool Serializer::DeserializeRegister(Serializable* obj)
{
    return DeserializeRegister(obj->GetIndex());
}
bool Serializer::SerializeRegister(int typeIndex, const char* pFilePath)
{
    serializeMap.emplace(typeIndex, pFilePath);
    return true;
}
bool Serializer::DeserializeRegister(int typeIndex)
{
    deserializeSet.insert(typeIndex);
    return true;
}