#include "serialize.hpp"
using namespace std;

int main()
{
    {
        A a1(1, 'c'), a2(2, 'd');
        B b1(2.0), b2(2.1);
        C c1("hello"), c2("world");
        
        vector<unique_ptr<Serializable>> v;
        
        v.push_back(make_unique<C>(c1));
        v.push_back(make_unique<C>(c2));
        v.push_back(make_unique<A>(a1));
        v.push_back(make_unique<A>(a2));
        v.push_back(make_unique<B>(b1));
        v.push_back(make_unique<B>(b2));
        
        Serializer s;
        s.SerializeRegister(a1.GetIndex(), "dataA");
        s.SerializeRegister(b1.GetIndex(), "dataB");
        s.SerializeRegister(c1.GetIndex(), "dataC");
        s.Serialize(v);
    }
    // {
    //     Serializer s;
    //     vector<unique_ptr<Serializable>> v;
    //     s.Deserialize("data", v);

    //     for(auto &item : v)
    //     {
    //         item->PutInfo();
    //     }
    // }
    return 0;
}