#Fenwick tree

###设计思想
通过把cumsum的值分散保存在idx在树中的路径上，并用位运算映射树结构，使检索更新cumsum快速简单

###例子
下图圆圈中蓝字为下标idx，黑字为值tree[idx]。红线指向后继next，黑线指向parent.
TODO: add image

###结点关系映射
* 第k层的index有k位1. 
* 黑线－从子结点到父结点index的映射: parent(n) = n & (n-1)，即最后一位置0.
* 红线－从结点到它后继结点的映射：   next(n) = n + (n & -n).  左移最后一位1,或把最后一段0111...11结构变为1000...00

###对后继映射的进一步解释
后继结点定义为下一个兄弟结点，或如果当前结点n是父亲的最右结点，后继是n第一个非最右结点祖先的后继结点。

对第一种情况，注意到(n&-n)取了n最右一位就可以得到结论。
对第二种情况，注意到所有最右结点都以x011...1100..00结尾，它的祖先结点中最低的不是最右结点的key是x0 10...0，而它的右结点值是x1 00...1100..00，正好还是n + (n & -n)。

###数据映射
tree[idx] = cumsum[idx] - cumsum[parent[idx]]

###算法
* 初始化：tree[idx]初始化为长为数组长度+1的全零数组。
* 检索：检索cumsum只需把node n+1路径上所有结点加起来，求range sum就是两个cumsum之差。
* 更新input[n]：假设增加d, 影响的元素是结点n+1以及它到根结点路径上所有结点的后续兄弟结点。这些元素都需要加d
  例如更新input[4]，则需更新结点5(对应input[4]), 6, 8.

###注意事项
Fenwick tree不直接存储data，因此如果问题需要access data方便起见最好另存一份。
