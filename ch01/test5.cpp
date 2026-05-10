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
        Serializer s1, s2, s3;
        vector<unique_ptr<Serializable>> v1, v2, v3;
        A a;
        B b;
        C c;
        s1.RegisterDeserialize(static_cast<int>(Serializable::Type::A));
        s1.Deserialize("dataAB", v1, Serializer::Filter::Normal);
        fprintf(stdout, "S1Test:\n");
        for(auto &item : v1)
        {
            item->PutInfo();
        }

        s2.RegisterDeserialize(a.GetIndex());
        s2.Deserialize("dataAB", v2, Serializer::Filter::Whitelist);
        fprintf(stdout, "S2Test:\n");
        for(auto &item : v2)
        {
            item->PutInfo();
        }

        s3.RegisterDeserialize(c.GetIndex());
        s3.Deserialize("dataC", v3, Serializer::Filter::Blacklist);
        fprintf(stdout, "S3Test:\n");
        for(auto &item : v3)
        {
            item->PutInfo();
        }
    }
    {

        
        


    }
    return 0;
}