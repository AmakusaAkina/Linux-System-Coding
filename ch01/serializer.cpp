#include "serialize.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

const unordered_map<int, Serializable::Creator> Serializable::factory = {
    {static_cast<int>(Serializable::Type::A), []() { return std::make_unique<A>(); }},
    {static_cast<int>(Serializable::Type::B), []() { return std::make_unique<B>(); }},
    {static_cast<int>(Serializable::Type::C), []() { return std::make_unique<C>(); }},
};
bool Serializable::TypeValid(int i)
{
    return factory.find(i) != factory.end();
}
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
    if (fd == -1) {
        perror("Serializer open error");
        return false;
    }

    for (auto &item : v) {
        item.Serialize(fd);
    }

    close(fd);
    return true;
}

bool Serializer::Deserialize(const char *pFilePath, vector<A> &v)
{
    int fd = open(pFilePath, O_RDONLY);
    if (fd == -1) {
        perror("Deserialize open error");
        return false;
    }

    while (true) {
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
    if (fd == -1) {
        perror("Serializer open error");
        return false;
    }

    for(auto &item : v) {
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
    if (fd == -1) {
        perror("Deserialize open error");
        return false;
    }

    while(true) {
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
    if (fd == -1) {
        perror("Serializer open error");
        close(fd);
        return false;
    }
    for (auto &sample : v) {
        int type = sample->GetIndex();
        if (write(fd, &type, sizeof(int)) != sizeof(int)) {
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
    if (fd == -1) {
        perror("Deserialize open error");
        return false;
    }

    while(true) {
        int type;
        int r = read(fd, &type, sizeof(int));
        if (r == 0) break;
        if(r != sizeof(int)) {
            close(fd);
            return false;
        }
        auto obj = Serializable::Create(type);
        if (!obj) {
            fprintf(stderr, "Unknown type ID: %d\n", type);
            close(fd);
            return false;
        }
        if (!obj->Deserialize(fd)) {
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
    unordered_map<int, int>dict;//<type, fd>
    if (!CreateTypeFdDict(dict)) {
        CloseFilesByDict(dict);
        return false;
    } 
    if (!SerializeByDict(v, dict)) {
        CloseFilesByDict(dict);
        return false;
    }
    return CloseFilesByDict(dict);
}
bool Serializer::Deserialize(const char* pFilePath, vector<unique_ptr<Serializable>>& v, Filter option)
{
    int fd = open(pFilePath, O_RDONLY);
    if (fd == -1) {
        perror("Deserialize open error");
        return false;
    }

    while(true) {
        int type;
        int r = read(fd, &type, sizeof(int));
        if (r == 0) break;
        if(r != sizeof(int)) {
            close(fd);
            return false;
        }
        auto obj = Serializable::Create(type);
        if (!obj) {
            fprintf(stderr, "Unknown type ID: %d\n", type);
            close(fd);
            return false;
        }   
            if (!obj->Deserialize(fd)) {
                close(fd);
                return false;
            }
            bool isRegistered = deserializeSet.find(type) != deserializeSet.end();
            if (ShouldAppend(option, isRegistered)) {
                v.push_back(move(obj));
            }
    }
    close(fd);
    return true;
}
bool Serializer::RegisterSerialize(Serializable* obj, const char* pFilePath)
{
    if (obj == nullptr) return false;
    return RegisterSerialize(obj->GetIndex(), pFilePath);
}
bool Serializer::RegisterDeserialize(Serializable* obj)
{
    if (obj == nullptr) return false;
    return RegisterDeserialize(obj->GetIndex());
}
bool Serializer::RegisterSerialize(int typeIndex, const char* pFilePath)
{
    if (!Serializable::TypeValid(typeIndex)) return false;
    serializeMap[typeIndex] = pFilePath;
    return true;
}
bool Serializer::RegisterDeserialize(int typeIndex)
{
    if (!Serializable::TypeValid(typeIndex)) return false;
    deserializeSet.insert(typeIndex);
    return true;
}

void Serializer::ClearRegSerialize()
{
    serializeMap.clear();
}
void Serializer::ClearRegDeserialize()
{
    deserializeSet.clear();
}

bool Serializer::CreateTypeFdDict(unordered_map<int, int>& dict)
{
    unordered_map<string, int>pathFdDict;
    bool openfile = true;
        for(auto& [type, path] : serializeMap) {
            string filepath = path;
            auto it = pathFdDict.find(filepath);
            if (it == pathFdDict.end()) {
                int fd = open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, FILE_PERM_ALL);
                if (fd == -1) {
                    perror("Serializer open error");
                    openfile = false;
                    break;
                }
                dict.insert({type, fd});
                pathFdDict[filepath] = fd;
            } else {
                dict[type] = it->second;
            }
        }
    return openfile;
}

bool Serializer::CloseFilesByDict(unordered_map<int, int>& dict)
{
    unordered_set<int> closedFd;
    bool closefile = true;
    
    for (auto& [type, fd] : dict) {
        if (closedFd.find(fd) == closedFd.end()) {
            if (close(fd) == -1) {
                closefile = false;
                fprintf(stderr, "Close File Failed: fd=%d\n", fd);
            }
            closedFd.insert(fd);
        }
    }
    dict.clear();
    return closefile;
}

bool Serializer::SerializeByDict(const vector<unique_ptr<Serializable>>& v,const unordered_map<int, int>& dict)
{
    bool success = true;
    for (auto &sample : v) {
        int type = sample->GetIndex();
        auto it = dict.find(type);
        if (it == dict.end()) {
            fprintf(stderr, "Skip: Unregistered Type\n");
            continue;
        }
        int fd = it->second;
        if (write(fd, &type, sizeof(int)) != sizeof(int)) {
            fprintf(stderr, "Write Type Error\n");
            success = false;
            break;
        }
            
        if (!sample->Serialize(fd)) {
            fprintf(stderr, "Write Error\n");
            success = false;
            break;
        }
    }
    return success;
}