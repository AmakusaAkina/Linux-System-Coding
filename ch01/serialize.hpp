#include <iostream>
#include <vector>
#pragma once

using namespace std;

struct Serialized
{
    int nType;  //0 for A;1 for B
    void *pObj;
    Serialized(){}
    Serialized(int n, void* p):nType(n), pObj(p){}
};

class A 
{
    int i;
    char c;
public:
    A(){i = 0; c = 'a';}
    A(int i):i(i){}
    A(int i, int c):i(i), c(c){}

    bool Serialize(const char* pFilePath);
    bool Deserialize(const char* pFilePath);
    bool Serialize(int fd) const;
    bool Deserialize(int fd);

    void f();
    int GetI();
    void PutI();
    void PutC();
};

class B
{
    float i;
public:
    B(){i = 0;}
    B(float i):i(i){}

    bool Serialize(const char* pFilePath);
    bool Deserialize(const char* pFilePath);
    bool Serialize(int fd) const;
    bool Deserialize(int fd);

    void f();
    float GetI();
};

class Serializer
{
public:
    bool Serialize(const char* pFilePath, const vector<A>& v);
    bool Deserialize(const char* pFilePath, vector<A>& v);

    bool Serialize(const char* pFilePath, vector<Serialized>& v);
    bool Deserialize(const char* pFilePath, vector<Serialized>& v);
};