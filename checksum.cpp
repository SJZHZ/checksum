#include <cstdio>
#include <cmath>
#include <string>
#include <string.h>
#include <iostream>
#include <bitset>
// #include <mpi.h>
#include <cstdlib>
#include <ctime>

int BER = 1e6;

int ERRCASE;
int errcnt;

bool GetError()
{
    int res = rand() % BER;
    if (res == 0)           // ERROR HAPPENS
    {
        errcnt++;
        return false;
    }
    else
        return true;
}
bool GetBit()
{
    int res = rand() % 2;
    return res;
}

namespace checksum
{
    struct Packet
    {
        int length;
        int *data;
        int checksum;


        Packet(int L) : length(L)
        {
            data = new int[length];

        }
        ~Packet()
        {
            delete[] data;
        }
    };


    void PrintData(Packet* P)
    {
        for (int i = 0; i < P->length; i++)
            std::cout << std::bitset<32> (P->data[i]) << std::endl;
    }
    void GenerateZero(Packet* P)                    // for error detection rate
    {
        memset(P->data, 0, P->length * sizeof(int));
    }
    void GenerateRandom(Packet* P)                  // for homomorphic correctness
    {
        for (int i = 0; i < P->length; i++)
        {
            int ans = 0;
            for (int j = 0; j < 31; j++)
                ans |= (int)GetBit() << j;
            P->data[i] = ans;
        }
    }

    // CHECK
    void(*check_func[10])(Packet*);
    void Trivial(Packet* P)                         // just add
    {
        int ans = 0;
        for (int i = 0; i < P->length; i++)
            ans += P->data[i];
        P->checksum = ans;
    }
    void CRC(Packet* P)                             // bitxor
    {

    }
    void Dimwise(Packet* P)
    {

    }
    void Fletcher(Packet* P)
    {
        
    }
    void Adler(Packet* P)
    {
        
    }

    void Fill_func()
    {
        check_func[0] = Trivial;
        check_func[1] = CRC;
        check_func[2] = Dimwise;


    }





}

void Init()
{
    srand(time(0));
    errcnt = 0;
}

bool Check_Homomorphic(int length, int N, void(*func)(void))
{
    checksum::Packet* A = new checksum::Packet(length);
    checksum::Packet* B = new checksum::Packet(length);
    for (int i = 0; i < N; i++)
    {
        checksum::GenerateRandom(A);
        checksum::GenerateRandom(B);
        
    }
}
bool Check_Ratio()
{

}



int main(int argc, char **argv)
{
    double ans;
    Init();
    for (int i = 0; i < 1e8; i++)
    {
        GetError();
    }
    std::cout << errcnt;
    checksum::Packet* P = new checksum::Packet(8);

    // P->GenerateZero();
    // P->PrintData();
    // P->GenerateRandom();
    // P->PrintData();


    return 0;
}