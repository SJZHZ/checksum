# checksum
## Algorithm
1. Trivial  
简单累加

2. LRC  
异或叠加

3. CRC  
模2的【多项式】除法

4. 独立CRC
在每个向量上分配一个CRC值

5. Fletcher
6. Alder
7. 内部LRC外部Alder

## Evaluaion
1. 校验性
    1. Hamming Distance
    2. P_ud(Probability of undetected errors)
2. 同态性
3. （实验处理延迟）
    不是首要要关注的
4. 这里不是加密，不需要防伪性

## Parameters
1. Bit Error Ratio

1. 数据元长度  
sizeof(int)
2. 向量长度  
length
3. 算法

## Performance

## Conclusion