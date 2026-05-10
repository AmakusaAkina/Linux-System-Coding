#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#define FILE_PERM_ALL (S_IRWXU | S_IRWXG | S_IRWXO)

using namespace std;

struct Serialized
{
    int nType;  //0 for A;1 for B
    void *pObj;
    Serialized(){}
    Serialized(int n, void* p):nType(n), pObj(p){}
};

class Serializable
{
public:
    virtual ~Serializable() = default;

    virtual bool Serialize(int fd) const = 0;
    virtual bool Deserialize(int fd) = 0;
    virtual int GetIndex() = 0;
    virtual void PutInfo() = 0;

    static bool TypeValid(int i);
    static unique_ptr<Serializable> Create(int i);

    enum class Type : int {
        A = 0,
        B = 1,
        C = 2
    };
private:
    using Creator = function<std::unique_ptr<Serializable>()>;
    static const unordered_map<int, Creator> factory;
};

class A : public Serializable
{
    int i;
    char c;
public:
    A(){i = 0; c = 'a';}
    A(int i):i(i){}
    A(int i, int c):i(i), c(c){}

    bool Serialize(const char* pFilePath);
    bool Deserialize(const char* pFilePath);
    bool Serialize(int fd) const override;
    bool Deserialize(int fd) override;

    int GetIndex() override;
    void PutInfo() override;
    int GetI();
    void PutI();
    void PutC();
};

class B : public Serializable
{
    float i;
public:
    B(){i = 0;}
    B(float i):i(i){}

    bool Serialize(const char* pFilePath);
    bool Deserialize(const char* pFilePath);
    bool Serialize(int fd) const override;
    bool Deserialize(int fd) override;

    int GetIndex() override;
    void PutInfo() override;
    float GetI();
};

class C : public Serializable
{
    string s;
public:
    C(){}
    C(string string):s(string){}

    bool Serialize(int fd) const override;
    bool Deserialize(int fd) override;

    int GetIndex() override;
    void PutInfo() override;
};

class Serializer
{
public:
    bool Serialize(const char* pFilePath, const vector<A>& v);
    bool Deserialize(const char* pFilePath, vector<A>& v);

    bool Serialize(const char* pFilePath, const vector<Serialized>& v);
    bool Deserialize(const char* pFilePath, vector<Serialized>& v);

    //  序列化所有对象到指定文件
    bool Serialize(const char* pFilePath, const vector<unique_ptr<Serializable>>& v);
    bool Deserialize(const char* pFilePath, vector<unique_ptr<Serializable>>& v);   //  

    enum class Filter{
        Blacklist = -1,
        Normal = 0,
        Whitelist = 1
    };
    //  根据注册表信息序列化对象到文件
    bool Serialize(const vector<unique_ptr<Serializable>>& v);
    //  过滤型反序列化
    bool Deserialize(const char* pFilePath, vector<unique_ptr<Serializable>>& v, Filter option);

    bool RegisterSerialize(Serializable* obj, const char* pFilePath);
    bool RegisterDeserialize(Serializable* obj);
    bool RegisterSerialize(int typeIndex, const char* pFilePath);
    bool RegisterDeserialize(int typeIndex);
    void ClearRegSerialize();
    void ClearRegDeserialize();
private:
    unordered_map<int, const char*> serializeMap;
    unordered_set<int> deserializeSet;
    bool CreateTypeFdDict(unordered_map<int, int>& dict);
    bool CloseFilesByDict(unordered_map<int, int>& dict);
    bool SerializeByDict(const vector<unique_ptr<Serializable>>& v,const unordered_map<int, int>& dict);
    inline bool ShouldAppend(Filter option, bool isRegistered) const;
};

inline bool Serializer::ShouldAppend(Filter option, bool isRegistered) const {
    switch (option) {
        case Filter::Normal:    return true;
        case Filter::Blacklist: return !isRegistered;
        case Filter::Whitelist: return isRegistered;
        default : return false;
    }
}