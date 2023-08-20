# checksum
https://za4d3zvmtl1.feishu.cn/docx/DfrhdvOUBonPMtxlwGmcNDpQnVd
框架结构迁移到飞书文档上，本文档负责代码实现部分
## TODOLIST


## LOG
```txt
8.21
    1. CRC多项式
        0xA001: 41476 41477
        0x1021: 41478 41479
        0x8408: 41480 41481
        0x8005: 41482 41483
    2. Moduli
        0b1111000010110001u: 41484 41485
        QOSLimit

8.20
    我脑子瓦特了，去测BER=1e-4干嘛？
    为什么不该测：
        1. 在高频错误的测试结果可以【外推】到低频错误
        2. 低频测试效率太低了，大部分情况连1bit错误都没发生，计算浪费（即使这更贴合实际）

8.19
    多测几次代表性长度：10 100

8.18
    测试各种长度（10-100，跨度10）下，CRC16和BigMod16的校验能力

```

## Problem


## Algorithm
1. Trivial  
简单累加

2. LRC  
异或叠加

3. CRC  
模2的【多项式】除法

4. None
对照实验

5. Fletcher
初值0，模数0xFFFF

6. Alder
初值1，模数0xFFF1

7. 内部Trivial外部Alder
不可能保证同态。
证明：
    设 C(1 << i) = Table[i]
    由同态性，C(1 << (i + 1)) = Table[i + 1] = 2 * Table[i] = Table[i] << 1。
    即Table[k] = Table[0] << k, 高位信息丢失。

8. 左移并除常数
有点奇怪

9. 乘以原根
尝试在Hamming Code下构造会更好


10. Pailler算法



## Parameters
1. Bit Error Ratio

1. 数据元长度  
sizeof(int)
2. 向量长度  
length
3. 算法

