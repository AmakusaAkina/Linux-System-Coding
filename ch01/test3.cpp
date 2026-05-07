#include "serialize.hpp"
using namespace std;

int main()
{
    {
        A a1(1, 'c'), a2(2, 'd'), a3(4, 'e');
        B b1(2.0), b2(2.1), b3(2.2);
        vector<Serialized> v;
        v.emplace_back(0,&a1);
        v.emplace_back(0,&a2);
        v.emplace_back(0,&a3);
        v.emplace_back(1,&b1);
        v.emplace_back(1,&b2);
        v.emplace_back(1,&b3);

        Serializer s;
        s.Serialize("data", v);
    }

    {
        Serializer s;
        vector<Serialized> v;
        s.Deserialize("data", v);

        for(auto item : v)
        {
            if (item.nType == 0)  {
                A* a = (A*)item.pObj;
                a->PutInfo();
            } else if (item.nType == 1) {
                B* b = (B*)item.pObj;
                b->PutInfo();
            }
        }
    }

    return 0;
}