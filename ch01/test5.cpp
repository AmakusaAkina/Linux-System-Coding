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
        s.RegisterSerialize(a1.GetIndex(), "dataAB");
        s.RegisterSerialize(b1.GetIndex(), "dataAB");
        s.RegisterSerialize(c1.GetIndex(), "dataC");
        s.Serialize(v);
    }
    {
        Serializer s;
        vector<unique_ptr<Serializable>> v;
        s.Deserialize("dataA", v, Serializer::Filter::Normal);

        for(auto &item : v)
        {
            item->PutInfo();
        }
    }
    return 0;
}