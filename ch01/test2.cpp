#include "serialize.hpp"
using namespace std;

int main()
{
    {
        A a1(1, 'a'), a2(2, 'c'), a3(4, 'b');

        vector<A> v;
        v.push_back(a1);
        v.push_back(a2);
        v.push_back(a3);

        Serializer s;
        s.Serialize("data" ,v);
    }

    {
        Serializer s;
        vector<A> v;
        s.Deserialize("data", v);
        
        for (auto &item : v)
        {
            item.f();
        }
    }
    return 0;
}