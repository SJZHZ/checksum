#include <cstdio>
#include <cmath>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <string.h>
#include <iostream>
#include <bitset>
// #include <mpi.h>
#include <omp.h>
#include <cstdlib>
#include <ctime>

#define bs32 std::bitset<32>
int main()
{
    unsigned long long ans = 0;
    unsigned int Stream_Moduli = 0b11110000110000111010010101101001;
    int A[10] = {0};
    A[8] = 0b10001;
    for (int i = 0; i < 10; i++)
    {
        ans = ((ans << 32) + A[i]) % Stream_Moduli;
    }
    unsigned int out = ans;
    std::cout << bs32(out) << std::endl;
    return 0;
}