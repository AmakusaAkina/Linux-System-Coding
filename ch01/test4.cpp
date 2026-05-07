#include "serialize.hpp"
using namespace std;

int main()
{
    {
        A a1(1, 'c'), a2(2, 'd'), a3(4, 'e');
        B b1(2.0), b2(2.1), b3(2.2);
        
        vector<unique_ptr<Serializable>> v;
        
        // 拷贝到堆上
        v.push_back(std::make_unique<A>(a1));
        v.push_back(std::make_unique<A>(a2));
        v.push_back(std::make_unique<A>(a3));
        v.push_back(std::make_unique<B>(b1));
        v.push_back(std::make_unique<B>(b2));
        v.push_back(std::make_unique<B>(b3));
        
        Serializer s;
        s.Serialize("data", v);
    }
        {
        Serializer s;
        vector<unique_ptr<Serializable>> v;
        s.Deserialize("data", v);

        for(auto &item : v)
        {
            item->PutInfo();
        }
    }
    return 0;
}