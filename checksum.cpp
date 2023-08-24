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

// DEFAULT CONFIG
#define Vector_Length 100
#define Bit_Error_Ratio 1e3         // 取倒数
#define Homomorphic_Ntest 1e7
#define Prob_Undetected_Ntest 1e7
#define threshold 15
#define Test_H 0
#define Test_P 1
#define Test_Default Test_H
#define Start_Default 0
#define End_Default 10


#define bs32 std::bitset<32>
#define bs64 std::bitset<64>

// MAGIC NUMBER
#define CRC32_POLY_HEX 0x04C11DB7
#define CRC16_POLYNOMIAL 0x8005
// #define CRC16_POLY_HEX 0xA001
// #define CRC16_POLY_HEX 0x1021
// #define CRC16_POLY_HEX 0x8408
// #define CRC16_POLY_HEX 0x8005
// #define CRC16_POLY_HEX 0b1000101001100001
// #define CRC16_POLY_HEX 0b1000010001101001
#define CRC16_POLY_HEX 0b1010010101101001


#define None_Moduli 1ull << 32
#define Devide_Moduli 0xFF03C0FFull
#define Paillier_Moduli 0xF000FF06ull

#define Big_Moduli 0b11110000110000111010010101101001ull
// #define Big_Moduli_16 0b1111000010110001u
// #define Big_Moduli_16 0b1101010001101001u
// #define Big_Moduli_16 0b1111001010110001u
// #define Big_Moduli_16 0b1101001010110001u
// #define Big_Moduli_16 0b1101001010111001u
// #define Big_Moduli_16 0b1100011010010101u
// #define Big_Moduli_16 0xFFFF
#define Big_Moduli_16 0xF141
// #define Big_Moduli_16 0xA2C1
// #define Big_Moduli_16 0xA0C9

#define Big_Moduli_8 0b11100101u


// Bit Op
int BER = Bit_Error_Ratio;
bool GetError()         // P = BER
{
    int res = rand() % BER;
    return res == 0;
}
bool GetBit()           // P = 1 / 2
{
    int res = rand() % 2;
    return res;
}

// Prime Number
bool PrimeFilter[100000] = {0};
unsigned int PrimeNum[10000];
void CalcPrime()
{
    // Filter
    int FilterSize = sizeof(PrimeFilter);
    for (int i = 2; i < FilterSize; i++)
        PrimeFilter[i] = true;
    for (int i = 2; i < FilterSize; i++)
        if(PrimeFilter[i])
            for (long long j = i; i * j < FilterSize; j++)
                PrimeFilter[i * j] = false;
    
    // Array    
    for (int i = 0, cnt = 0; i < FilterSize && cnt < sizeof(PrimeNum) / sizeof(int); i++)
        if (PrimeFilter[i])
        {
            PrimeNum[cnt] = i;
            cnt++;
        }
}
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

// CRC
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
void MakeCRC32()                            // 8bit Table
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
unsigned int CalcCRC32(unsigned int crc, char* buff, int len)       // CRC是字节流校验，无需关注大小端
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

unsigned int ModBusCRC16(unsigned char *data, unsigned int len)
{
    unsigned int i, j, tmp, CRC16;

    CRC16 = 0xFFFF;             //CRC寄存器初始值
    for (i = 0; i < len; i++)
    {
        CRC16 ^= data[i];
        for (j = 0; j < 8; j++)
        {
            tmp = (unsigned int)(CRC16 & 0x0001);
            CRC16 >>= 1;
            if (tmp == 1)
            {
                CRC16 ^= CRC16_POLY_HEX;    //异或多项式
            }
        }
    }
    return CRC16;
}


// Fletcher or Adler
unsigned int CalcForA(unsigned int init_sum1, uint16_t* ptr, int len, unsigned long long moduli)
{
    unsigned int sum1 = init_sum1;
    unsigned int sum2 = 0;
    for (int i = 0; i < len; i++)
    {
        sum1 = (sum1 + ptr[i]) % moduli;
        sum2 = (sum2 + sum1) % moduli;
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
        unsigned int checksumEx;

        Packet(int L) : length(L)
        {
            data = new int[length];
        }
        ~Packet()
        {
            delete[] data;
        }
    };

    // ALGORIGTHM TABLE
    std::string check_func_name[100];
    void(*check_func[100])(Packet*);
    unsigned int(*aggregate_checksum[100])(unsigned int, unsigned int, unsigned long long);
    unsigned long long checksum_Moduli[100];

    void PrintData(Packet* P)
    {
        for (int i = 0; i < P->length; i++)
            std::cout << bs32(P->data[i]) << " ";
        std::cout << std::endl;
    }
    void GenerateZero(Packet* P)
    {
        memset(P->data, 0, P->length * sizeof(int));
    }
    int SimulateError(Packet* P)                   // for error detection rate
    {
        int errorflag = 0;
        for (int j = 0; j < P->length; j++)
        {
            for (int k = 0; k < 31; k++)            // 不溢出
            {
                if (GetError())
                {
                    P->data[j] = P->data[j] ^ (1 << k);             // bit flip
                    errorflag++;
                }
            }
        }
        for (int k = 0; k < 32; k++)
        {
            if (GetError())
            {
                P->checksum ^= (1 << k);                            // bit flip
                errorflag++;
            }
        }
        return errorflag;
    }
    void GenerateRandom(Packet* P)                  // for homomorphic correctness
    {
        for (int i = 0; i < P->length; i++)
        {
            int ans = 0;
            for (int j = 0; j < 31; j++)            // 不溢出
                ans |= (int)GetBit() << j;
            P->data[i] = ans;
        }
    }
    unsigned int Add_checksum(unsigned int a, unsigned int b, unsigned long long moduli)
    {
        return (unsigned int)(((unsigned long long)a + (unsigned long long)b) % moduli);
    }
    unsigned int Mul_checksum(unsigned int a, unsigned int b, unsigned long long moduli)
    {
        return (unsigned int)(((unsigned long long)a * (unsigned long long)b) % moduli);
    }
    unsigned int uint16_checksum(unsigned int a, unsigned int b, unsigned long long moduli)
    {
        unsigned int temp1 = ((a >> 16) + (b >> 16)) % moduli;
        unsigned int temp2 = ((a & 0xFFFF) + (b & 0xFFFF)) % moduli;
        // std::cout << bs32(temp1) << '\n' << bs32(temp2) << '\n' << bs32((temp1 << 16) | (temp2 & 0xFFFF)) << '\n';
        return (temp1 << 16) | (temp2 & 0xFFFF);
    }


    void Trivial(Packet* P)                         // ADD
    {
        int ans = 0;
        for (int i = 0; i < P->length; i++)
            ans += P->data[i];
        P->checksum = ans;
    }
    void LRC(Packet* P)                             // BITXOR
    {
        int ans = 0;
        for (int i = 0; i < P->length; i++)
            ans ^= P->data[i];
        P->checksum = ans;
    }
    void CRC(Packet* P)                             // Polynomial Division
    {
        unsigned int ans = CalcCRC32(0, (char*)P->data, P->length * sizeof(int) / sizeof(char));
        P->checksum = ans;
    }
    void None(Packet* P)
    {
        P->checksum = 0;
    }
    void Fletcher(Packet* P)
    {
        int ans = CalcForA(0, (uint16_t*) P->data, P->length * sizeof(int) / sizeof(uint16_t), 0xFFFF);     // one's complement
        P->checksum = ans;
    }
    void Adler(Packet* P)
    {
        int ans = CalcForA(0, (uint16_t*) P->data, P->length * sizeof(int) / sizeof(uint16_t), 0xFFF1);
        P->checksum = ans;
    }
    void AdlerPro(Packet* P)                        //
    {
        unsigned int sum1 = 0;
        unsigned int sum2 = 0;
        for (int i = 0; i < P->length; i++)
        {
            unsigned temp = P->data[i];
            sum1 = (sum1 + temp) % 0xFFF1;
            sum2 = (sum2 + sum1) % 0xFFF1;
        }
        P->checksum = (sum2 << 16) | sum1;
        //TODO: 聚合方式要改
    }
    void Devide(Packet* P)
    {
        unsigned long long ans = 0;
        for (int i = 0; i < P->length; i++)
        {
            unsigned long long temp = P->data[i];
            temp = (temp << 32) % Devide_Moduli;
            temp = (temp * (2 * i + 1001)) % Devide_Moduli;
            ans = (ans + temp) % Devide_Moduli;
        }
        P->checksum = ans;

    }
    void Multiple(Packet* P)
    {
        unsigned int ans = 0;
        for (int i = 0; i < P->length; i++)
        {
            // ans += P->data[i] * PrimeNum[3 * i + 100];
            ans += P->data[i] * (0xFC * i + 1010101);
        }
        P->checksum = ans;
    }
    void Paillier(Packet* P)
    {
        unsigned long long ans = 1;
        for (int i = 0; i < P->length; i++)
        {
            ans = (ans * FastPow(PrimeNum[i + 5], P->data[i], Paillier_Moduli)) % Paillier_Moduli;
        }
        P->checksum = ans;
    }
    void BigMod(Packet* P)
    {
        unsigned long long ans = 0;
        for (int i = 0; i < P->length; i++)
        {
            ans = ((ans << 32) + (unsigned int)(P->data[i])) % Big_Moduli;
        }
        ans = (ans << 32) % Big_Moduli;
        P->checksum = ans;
    }
    void BigMod16(Packet* P)
    {
        unsigned long long ans = 0;
        for (int i = 0; i < P->length; i++)
        {
            ans = ((ans << 32) + (unsigned int)(P->data[i])) % Big_Moduli_16;
        }
        ans = (ans << 32) % Big_Moduli_16;
        P->checksum = ans;
    }
    void BigMod8(Packet* P)
    {
        unsigned long long ans = 0;
        for (int i = 0; i < P->length; i++)
        {
            ans = ((ans << 32) + (unsigned int)(P->data[i])) % Big_Moduli_8;
        }
        ans = (ans << 32) % Big_Moduli_8;
        P->checksum = ans;
    }

    void CRC16(Packet* P)
    {
        uint16_t ans = ModBusCRC16((unsigned char*) P->data, P->length * sizeof(int) / sizeof(char));
        // printf("0x%04X\n", ans);
        P->checksum = ans;
    }


    void Fill_func()
    {
        check_func_name[0] = "None";
        check_func[0] = &None;
        aggregate_checksum[0] = &Add_checksum;
        checksum_Moduli[0] = None_Moduli;

        check_func_name[1] = "LRC";
        check_func[1] = &LRC;
        aggregate_checksum[1] = &Add_checksum;
        checksum_Moduli[1] = None_Moduli;

        check_func_name[2] = "CRC";
        check_func[2] = &CRC;
        aggregate_checksum[2] = &Add_checksum;
        checksum_Moduli[2] = None_Moduli;

        check_func_name[3] = "Trivial";
        check_func[3] = &Trivial;
        aggregate_checksum[3] = &Add_checksum;
        checksum_Moduli[3] = None_Moduli;

        check_func_name[4] = "Fletcher";
        check_func[4] = &Fletcher;
        aggregate_checksum[4] = &Add_checksum;
        checksum_Moduli[4] = None_Moduli;

        check_func_name[5] = "Adler";
        check_func[5] = &Adler;
        aggregate_checksum[5] = &Add_checksum;
        checksum_Moduli[5] = None_Moduli;

        check_func_name[6] = "AdlerPro";
        check_func[6] = &AdlerPro;
        aggregate_checksum[6] = &uint16_checksum;
        checksum_Moduli[6] = 0xFFF1;

        check_func_name[7] = "Devide";
        check_func[7] = &Devide;
        aggregate_checksum[7] = &Add_checksum;
        checksum_Moduli[7] = Devide_Moduli;

        check_func_name[8] = "Multiple";
        aggregate_checksum[8] = &Add_checksum;
        check_func[8] = &Multiple;
        checksum_Moduli[8] = None_Moduli;

        check_func_name[9] = "Paillier";
        check_func[9] = Paillier;
        aggregate_checksum[9] = &Mul_checksum;
        checksum_Moduli[9] = Paillier_Moduli;
        
        check_func_name[10] = "BigMod";
        check_func[10] = BigMod;
        aggregate_checksum[10] = &Add_checksum;
        checksum_Moduli[10] = Big_Moduli;  

        check_func_name[11] = "BigMod16";
        check_func[11] = BigMod16;
        aggregate_checksum[11] = &Add_checksum;
        checksum_Moduli[11] = Big_Moduli_16;

        check_func_name[12] = "BigMod8";
        check_func[12] = BigMod8;
        aggregate_checksum[12] = &Add_checksum;
        checksum_Moduli[12] = Big_Moduli_8;

        check_func_name[13] = "CRC16";
        check_func[13] = CRC16;
        aggregate_checksum[13] = &Add_checksum;
        checksum_Moduli[13] = None_Moduli;

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


double Test_Homomorphic(int length, int Ntimes, int func_id)
{
    std::cout << "-----Testing function " << func_id << ": " << checksum::check_func_name[func_id] << std::endl;

    checksum::Packet* A = new checksum::Packet(length);
    checksum::Packet* B = new checksum::Packet(length);
    checksum::Packet* C = new checksum::Packet(length);
    double cnt = 0;
    for (long long i = 0; i < Ntimes; i++)
    {
        checksum::GenerateRandom(A);
        checksum::GenerateRandom(B);
        (checksum::check_func[func_id])(A);
        (checksum::check_func[func_id])(B);
        unsigned int Asum = A->checksum;
        unsigned int Bsum = B->checksum;
        
        unsigned int Csum_exp = checksum::aggregate_checksum[func_id](Asum, Bsum, checksum::checksum_Moduli[func_id]);
        checksum::Aggregate(A, B, C, func_id);
        int Csum = C->checksum;
        
        if(Csum_exp == Csum)
            cnt++;
        else
        {
            // PrintData(A);
            // std::cout << bs32(Asum) << '\n';
            // PrintData(B);
            // std::cout << bs32(Bsum) << '\n';
            // PrintData(C);
            // std::cout << bs32(Csum_exp) << ' ' << bs32(Csum) << '\n';
        }
    }
    
    std::cout << "Homomorphic: " << cnt << " homomorphic cases under " << Ntimes << " tests" << std::endl;
    delete A;
    delete B;
    delete C;
    return cnt / Ntimes;
}

double Test_ErrorRatio(int length, int Ntimes, int func_id)
{
    std::cout << "-----Testing function " << func_id << ": " << checksum::check_func_name[func_id] << std::endl;

    double udcnt = 0;           // undetected errors
    int errorcnt = 0;           // errors
    int HD[20] = {0};           // Hamming Distance 
    checksum::Packet* A = new checksum::Packet(length);

    for (long long i = 0; i < Ntimes; i++)
    {
        checksum::GenerateRandom(A);
        checksum::check_func[func_id](A);

        int errorflag = checksum::SimulateError(A);
        if (!errorflag)                                             // error or not
            continue;
        errorcnt++;

        unsigned int Asum_pre = (A)->checksum;

        checksum::check_func[func_id](A);
        unsigned int Asum_post = (A)->checksum;

        if (Asum_post == Asum_pre)     // undetected error
        {
            udcnt++;
            if (errorflag <= threshold)
                HD[errorflag]++;
            else
                HD[threshold]++;
            // if (errorflag == 1)
            // {
            //     checksum::PrintData(A);
            // }
        }
    }

    delete A;

    for (int i = 1; i <= threshold; i++)
        std::cout << "HD=" << i << ":" << HD[i] << ". ";
    std::cout << "HD>" << threshold << ":" << HD[threshold] << ". ";
    std::cout << "\nErrorRatio: " << udcnt << " undetected cases in " << errorcnt << " error cases " << "\n-----END\n";


    return udcnt / Ntimes;
}

void Init()
{
    srand(time(0));
    checksum::Fill_func();
    CalcPrime();
    return;


    // SHOW FASTPOW
    for (int j = 0; j < 10; j++)
    {
        for (int i = 0; i < 32; i++)
            std::cout << bs32(FastPow(PrimeNum[j + 100], 1ll << i, 0xF000FF06u)) << " " << FastPow(PrimeNum[j + 100], 1ll << i, 0xF000FF06u) << std::endl;
        std::cout << std::endl;
    }

}

int main(int argc, char **argv)
{
    long long Ntests = 0;
    int HorP = Test_Default, length = Vector_Length;
    int start_id = Start_Default, end_id = End_Default;
    if (argc >= 2)
        HorP = atoi(argv[1]);
    if (HorP == Test_H)
        Ntests = Homomorphic_Ntest;
    if (HorP == Test_P)
        Ntests = Prob_Undetected_Ntest;
    if (argc >= 3)
    {
        start_id = atoi(argv[2]);
        end_id = atoi(argv[2]);
    }
    if (argc >= 4)
        Ntests = atoll(argv[3]);
    if (argc >= 5)
        length = atoi(argv[4]);
    // ./check HorP strat_id end_id Ntests length

    Init();

    std::cout << "\nTESTING ";
    if (HorP == Test_H)
        std::cout << "HOMOMORPHIC";
    if (HorP == Test_P)
        std::cout << "P_ud";
    std::cout << " WITH LENGTH " << length << " FOR " << Ntests << " TIMES.\n";



    time_t T_begin, T_end;
    time(&T_begin);
    for (int i = start_id; i <= end_id; i++)
    {
        time_t T_0, T_1;
        time(&T_0);

        if (HorP == Test_H)
            double H = Test_Homomorphic(length, Ntests, i);
        if (HorP == Test_P)
            double P_ud = Test_ErrorRatio(length, Ntests, i);

        time(&T_1);
        std::cout << "TIME: " << difftime(T_1, T_0) << " second.\n\n";
        
    }
    time(&T_end);
    std::cout << "TOTAL TIME: " << difftime(T_end, T_begin) << " second." << std::endl;

    return 0;
}