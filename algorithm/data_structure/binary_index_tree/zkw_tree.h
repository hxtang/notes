// returns smallest number which is greater or equal to v and is integer powers of 2
inline unsigned int next_power_of_two(unsigned int v) {
    --v;
    v |= (v >> 1);
    v |= (v >> 2);
    v |= (v >> 4);
    v |= (v >> 8);
    v |= (v >> 16);
    return v+1;
}

// supports per-element update and range sum query with a segment tree
// reference: http://wenku.baidu.com/view/f27db60ee87101f69e319544.html
class NumArray {
public:
    NumArray(vector<int> &nums) : M(next_power_of_two(nums.size()+2)) {
        tree.resize(M<<1, 0);
        copy(nums.begin(), nums.end(), tree.begin()+M+1);
        for (int k = M-1; k>0; k--) tree[k] = tree[k<<1] + tree[k<<1|1];
    }

    void update(int i, int val) {
        i++; //number is 1 based
        int diff = val - tree[M+i];
        for (int key = M+i; key > 0; key >>= 1) tree[key] += diff;
    }

    int sumRange(int i, int j) {
        int sum = 0;
        for (i += M, j += M+2; i ^ j ^ 1; i >>= 1, j >>= 1) {
            if (!(i&1)) sum += tree[i^1];
            if (j&1) sum += tree[j^1];
        }
        return sum;
    }

    // double current tree size, the expanded part are set to 0
    void resize() {
        tree.resize(M*4, 0);
        for (int base = M; base; base >>= 1) //move current nodes to new locations
            rotate(tree.begin()+base, tree.begin()+base*2, tree.begin()+base*3);
        tree[1]=tree[2]; //new root
        M<<=1;
    }
	
    // search the first element whose corresponding prefix sum is equal to or larger than v
    int sum_search(int v) {
        int i = 1;
        do {
            i <<= 1;
            if (v > tree[i]) v -= tree[i++];
        } while (i < M);
        return i-M;
    }

private:
    vector<int> tree;  //tree stored in array, size is M*2
    int M; //smallest power of 2 greater or equal than nums.size()+2
};
