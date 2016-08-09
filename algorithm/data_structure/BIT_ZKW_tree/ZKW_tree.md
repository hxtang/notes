#ZKW Segment tree

线段树是一棵二叉树。每个结点对应一个区间[l,r),结点的两个子结点把这个区间的值分为均匀的两段。每个结点还存储这个区间里的sum。根结点对应[0, n)。

这里我们使用数组存储一棵二叉完全树。为index方便，我们把n扩展到2的整数次幂M = 2k. 上面的例子对应的Segment Tree如下图。
TODO: add image
其中的红字表示结点的index。

###结点关系映射：
注意到index的父结点是子结点的前缀。因此 

* parent(n) = n>>1; 
* left_child(n) = n*2; 
* right_child(n) = n*2+1;

###数据映射：
define BASE = next_power_of_two(size of input)

* tree[n] = tree[left_child[n)] + tree[right_child(n)] for n < BASE
* tree[n] = input[n-BASE) for n >= BASE 

###算法
* 初始化：构造树时，则把输入复制从tree[M]开始的空间。然后从tree[M-1]开始，用tree[n] = tree[n*2] + tree[n*2+1]构造。
* 检索： 当我们需要检索区间(j, k)时(即区间[j+1, k-1])，相关结点都在j, k的最小公共子树里，然后寻找到j的path上所有左结点的右兄弟之和，和到k的path上所有右结点的左兄弟之和。我们只需要迭代指向j, k的父亲，直到两者指向同一结点。 
* 更新：当更新A[k]时，只要相应更新根结点到结点[k,k]路径上所有结点的value.这通过迭代访问parent(n)即可做到。

###边界条件
开区间访问需要检索A[-1]和A[end+1]，因此我们增加两个元素，同时把index向右平移1.

