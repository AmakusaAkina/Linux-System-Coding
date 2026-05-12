#include "serialize.hpp"
using namespace std;

int main()
{
    {
        A a(13, 'x');
        a.Serialize("data");
    }

    {
        A a;
        a.Deserialize("data");
        a.PutInfo();
    }
    return 0;
}