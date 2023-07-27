# checksum
## TODOLIST
底数改成素数，模数如何选择人、
Devide修改：模数必须相同

## Problem
1. 测试
目前提供的这个测试模型（P_ud=1e-18）表现过好，以致于区分不出来。（我在自己的机器上测一轮10^7数量级都要几分钟）
    1. 用服务器做并行验证（远不够）
    2. 在探索阶段，暂时先调大BER
    3. 在后期数据部分，必须按标准方式测试
        （对于few bit的情况，他们是怎么测试到的？）
        可能是直接分析的
2. 构造
我目前只想到两个比较平凡的编码构造，对于这类问题，是否有一个解析性的表达方式，然后在约束中求最大化？
我正在看格密码学部分，这里是否会有答案？

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