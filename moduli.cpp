#include <stdio.h>
#include <iostream>
#include <bitset>
#include <vector>

#define bs32 std::bitset<32>

unsigned int FastPow(unsigned int a, unsigned int n, unsigned long long moduli)
{
    unsigned long long temp = a, res = 1;
    while(n != 0)
    {
        if (n & 1)
            res = (res * temp) % moduli;
        n >>= 1;
        temp = (temp * temp) % moduli;
    }
    return res;
}
int main()
{
    int A = 0x0F00F00F;
    int B = 0xFF000000;
    int C = A + B;

    std::vector<int> v;
    v.size();
    std::cout << bs32(FastPow(3, 1 << 16, 1ll << 32)) << std::endl;
    std::cout << bs32(A) << std::endl;
    std::cout << bs32(B) << std::endl;
    std::cout << bs32(C) << std::endl;


    unsigned int moduli = 0xFF01;
    std::cout << bs32(A % moduli) << std::endl;
    std::cout << bs32(B % moduli) << std::endl;
    std::cout << std::endl;
    std::cout << bs32(C % moduli) << std::endl;
    std::cout << bs32(((A % moduli) + (B % moduli)) % moduli) << std::endl;
}