#include <cstdio>
#include <cmath>
#include <string>
#include <string.h>
#include <iostream>
#include <bitset>
// #include <mpi.h>
#include <cstdlib>
#include <ctime>

#define bs32 std::bitset<32>
#define BITERRORRATIO 1e6
#define CRC32_POLY_HEX 0x04C11DB7

int BER = BITERRORRATIO;

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


bool CRC32_avail = false;
unsigned int CRC32_table[256];
unsigned int CRC32_pre = CRC32_POLY_HEX;    // 正向
unsigned int CRC32_post = 0;                // 反向

unsigned int reverse(unsigned int temp)
{
    unsigned int ans = 0;
    for (int i = 0; i < 32; i++)
        if (temp & (1 << i))
            ans |= (1 << (31 - i));
    return ans;
}
void MakeCRC32()                            // 8bit复用存表
{
    CRC32_post = reverse(CRC32_pre);
    printf("%X\n", CRC32_pre);
    printf("%X\n", CRC32_post);
    std::cout << std::bitset<32>(CRC32_pre) << std::endl;
    std::cout << std::bitset<32>(CRC32_post) << std::endl;

    for (int i = 0; i < 256; i++)
    {
        unsigned int temp = i;
        for (int j = 0; j < 8; j++)
        {
            if (temp & 1)
                temp = (temp >> 1) ^ CRC32_post;
            else
                temp = temp >> 1;
        }
        CRC32_table[i] = temp;
    }
    CRC32_avail = true;
}
unsigned int CalcCRC32(unsigned int crc, char* buff, int len)
{
    if (!CRC32_avail)
        MakeCRC32();
    crc = ~crc;
    for (int i = 0; i < len; i++)
    {
        int temp = CRC32_table[(crc ^ buff[i]) & 0xFF];     // 逆序，crc物理最低8bit正好是逻辑上最头部的
        crc = (crc >> 8) ^ temp;                            // 转移位置，并使用预处理的表
    }
    return ~crc;                                            // 逆序
}
unsigned int CalcForA(unsigned int init_sum1, uint16_t* ptr, int len, int modnum)       // Fletcher or Adler
{
    unsigned int sum1 = init_sum1;
    unsigned int sum2 = 0;
    for (int i = 0; i < len; i++)
    {
        sum1 = (sum1 + ptr[i]) % modnum;
        sum2 = (sum2 + sum1) % modnum;
    }
    return (sum2 << 16) | sum1;
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

    void Aggregate(Packet* P1, Packet* P2, Packet* P3)
    {
        if (P1->length != P2->length || P1->length != P3->length)
        {
            std::cout << "!invalid source data!" << std::endl;
            return;
        }
        int length = P1->length;
        for (int i = 0; i < length; i++)
            P3->data[i] = P1->data[i] + P2->data[i];
        P3->checksum = P1->checksum + P2->checksum;
    }

    // CHECK
    void(*check_func[10])(Packet*);
    void Trivial(Packet* P)                         // ADD
    {
        int ans = 0;
        for (int i = 0; i < P->length; i++)
            ans += P->data[i];
        P->checksum = ans;
    }

    void LRC(Packet* P)                             // XOR
    {
        int ans = 0;
        for (int i = 0; i < P->length; i++)
            ans ^= P->data[i];
        P->checksum = ans;
    }
    void CRC(Packet* P)                             // polynomial
    {
        unsigned int ans = CalcCRC32(0, (char*)P->data, P->length * sizeof(int) / sizeof(char));
        P->checksum = ans;
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
        check_func[1] = LRC;
        check_func[2] = CRC;
        check_func[3] = Dimwise;


    }





}

void Init()
{
    srand(time(0));
}

double Test_Homomorphic(int length, int Ntimes, void(*func)(checksum::Packet*))
{
    checksum::Packet* A = new checksum::Packet(length);
    checksum::Packet* B = new checksum::Packet(length);
    checksum::Packet* C = new checksum::Packet(length);
    double cnt = 0;
    for (int i = 0; i < Ntimes; i++)
    {
        checksum::GenerateRandom(A);
        checksum::GenerateRandom(B);
        func(A);
        func(B);
        int Asum = A->checksum;
        int Bsum = B->checksum;

        checksum::Aggregate();
        
        
    }
    delete A;
    delete B;
    delete C;
    return cnt / Ntimes;
}
double Test_ErrorRatio(int length, int Ntimes, void(*func)(checksum::Packet*))
{
    double cnt = 0;
    checksum::Packet* A = new checksum::Packet(length);
    for (int i = 0; i < Ntimes; i++)
    {
        checksum::GenerateZero(A);
        func(A);
        unsigned int Asum_pre = A->checksum;
        bool errorflag = 0;
        for (int j = 0; j < length; j++)
        {
            for (int k = 0; k < 32; k++)
            {
                if (GetError())
                {
                    A->data[j] = A->data[j] ^ (1 << k);             // bit flip
                    errorflag = 1;
                }
            }
        }
        if (!errorflag)                                             // error or not
            continue;
        func(A);
        unsigned int Asum_post = A->checksum;
        if (Asum_post != Asum_pre)                                  // undetected error
            cnt++;
    }
    delete A;
    return cnt / Ntimes;
}



int main(int argc, char **argv)
{


    return 0;
}