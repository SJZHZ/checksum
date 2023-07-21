#include <cstdio>
#include <cmath>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <bitset>
// #include <mpi.h>
#include <cstdlib>
#include <ctime>

#define bs32 std::bitset<32>
#define CRC32_POLY_HEX 0x04C11DB7

#define BER 1e3
#define Homomorphic_Ntimes 1e6
#define Prob_Undetected_Ntimes 1e6



bool GetError()
{
    int res = rand() % BER;
    if (res == 0)           // ERROR HAPPENS
    {
        // errcnt++;
        return true;
    }
    else
        return false;
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
    // printf("%X\n", CRC32_pre);
    // printf("%X\n", CRC32_post);
    // std::cout << std::bitset<32>(CRC32_pre) << std::endl;
    // std::cout << std::bitset<32>(CRC32_post) << std::endl;

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
        unsigned int checksum;


        Packet(int L) : length(L)
        {
            data = new int[length];

        }
        ~Packet()
        {
            delete[] data;
        }
    };
    void(*check_func[20])(Packet*);
    std::string check_func_name[20];

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
        //这里有个不足，我稍后修改一下
        //错误也会发生在checksum中，必须考虑
    }

    unsigned int Add_checksum(unsigned int a, unsigned int b)
    {
        return a + b;
    }

    // CHECK
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
    void None(Packet* P)
    {

    }
    void Fletcher(Packet* P)
    {
        int ans = CalcForA(0, (uint16_t*) P->data, P->length * sizeof(int) / sizeof(uint16_t), 0xFFFF);     // one's complement
        P->checksum = ans;
    }
    void Adler(Packet* P)
    {
        int ans = CalcForA(1, (uint16_t*) P->data, P->length * sizeof(int) / sizeof(uint16_t), 0xFFF1);
        P->checksum = ans;
    }
    void AdlerPro()
    {

    }
    void Devide()
    {

    }


    void Fill_func()
    {
        check_func[0] = &Trivial;
        check_func[1] = &LRC;
        check_func[2] = &CRC;
        check_func[3] = &None;
        check_func[4] = &Fletcher;
        check_func[5] = &Adler;

        check_func_name[0] = "Trivial";
        check_func_name[1] = "LRC";
        check_func_name[2] = "CRC";
        check_func_name[3] = "None";
        check_func_name[4] = "Fletcher";
        check_func_name[5] = "Adler";

    }





    void Aggregate(Packet* P1, Packet* P2, Packet* P3, int func_id)
    {
        if (P1->length != P2->length || P1->length != P3->length)
        {
            std::cout << "!invalid source data!" << std::endl;
            return;
        }
        int length = P1->length;
        for (int i = 0; i < length; i++)
            P3->data[i] = P1->data[i] + P2->data[i];
        check_func[func_id](P3);
    }
}

void Init()
{
    srand(time(0));
    checksum::Fill_func();
}

double Test_Homomorphic(int length, int Ntimes, int func_id)
{
    std::cout << "-----Testing function " << func_id << ": " << checksum::check_func_name[func_id] << std::endl;
    checksum::Packet* A = new checksum::Packet(length);
    checksum::Packet* B = new checksum::Packet(length);
    checksum::Packet* C = new checksum::Packet(length);
    double cnt = 0;
    for (int i = 0; i < Ntimes; i++)
    {
        checksum::GenerateRandom(A);
        checksum::GenerateRandom(B);
        (checksum::check_func[func_id])(A);
        (checksum::check_func[func_id])(B);
        int Asum = A->checksum;
        int Bsum = B->checksum;
        int Csum_exp = checksum::Add_checksum(Asum, Bsum);

        checksum::Aggregate(A, B, C, func_id);
        int Csum = C->checksum;
        
        if(Csum_exp == Csum)
            cnt++;
    }
    std::cout << "Homomorphic: " << cnt << " homomorphic cases under " << Ntimes << " tests" << std::endl;
    delete A;
    delete B;
    delete C;
    return cnt / Ntimes;
}
double Test_ErrorRatio(int length, int Ntimes, int func_id)
{
    double cnt = 0;
    int errorcnt = 0;
    checksum::Packet* A = new checksum::Packet(length);
    for (int i = 0; i < Ntimes; i++)
    {
        checksum::GenerateZero(A);
        checksum::check_func[func_id](A);
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
        errorcnt++;
        checksum::check_func[func_id](A);
        unsigned int Asum_post = A->checksum;
        if (Asum_post == Asum_pre)                                  // undetected error
            cnt++;
    }
    std::cout << "ErrorRatio: " << cnt << " undetected cases in " << errorcnt << " error cases " << "\n-----END\n\n";


    delete A;
    return cnt / Ntimes;
}



int main(int argc, char **argv)
{
    int length = 50;
    Init();

    for (int i = 0; i < 6; i++)
    {
        int H = Test_Homomorphic(length, Homomorphic_Ntimes, i);
        int P_ud = Test_ErrorRatio(length, Prob_Undetected_Ntimes, i);
        
    }

    return 0;
}