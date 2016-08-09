#Binary indexed trees

###Scenario
应用：维护数组中的连续范围的[a, b]的属性P(a, b)
条件：存在操作符 +, 对任意c: a <= c <= b, P(a, b) = P(a, c) + P(a, b)

###典型问题：
* Range sum: 属性是range sum, 操作符是加法
* Range addition: 属性是包含[a, b]的所有range, +是并操作
* Weighted RAM: 属性是[a, b]返回内的权重和， 操作符是加法

###Overview
Fenwick Tree (Binary Index Tree): 

* update element, retrieve prefix sum (cumsum)
* log(N)时间写
* log(N)时间读
* 空间开销O(N) （如果不需要存储原始数据，为N)
* 动态增加capacity只需要resize数组

ZKW Segment Tree

* 三种使用模式：
* (1) update element, retrieve range sum (2) update range sum, retrive element (3) update element, retrieve range whose sum >= given value 
* log(N)时间修改单个元素
* log(N)时间检索range sum
* 空间开销O(N) (树的最后一层存储了原始数据）
* 动态增加capacity需要在resize数组以后移动数据，amortized cost为O(1)

比较：
* Fenwick Tree是多叉树，自顶向下search比较难
* Fenwick Tree主要针对prefix sum，如果要作range sum必须有合法有效的减法operator
* Fenwick Tree解释起来没有Segment Tree容易
* ZKW Segment Tree 占的空间大一点点（需要是2的整数次，resize的逻辑复杂一点点）
