// efficient per-element update and query of range sum with a Fenwick (Binary index) tree
class NumArray {
public:
    // initialize data with a given vector
    NumArray(vector<int> &nums) : data(nums), tree(nums.size()+1, 0) {
        for (int i = 0; i < data.size(); ++i)
            for (int key = i+1; key < tree.size(); key += (key&-key)) tree[key] += data[i];
    }

    // update i-th elemnent to val
    void update(int i, int val) {
        int diff = val - data[i];
        for (int key = i+1; key < tree.size(); key += (key&-key)) tree[key] += diff;
        data[i] = val;
    }
    
    // return sum of elements in range [i, j]
    int sumRange(int i, int j) {
        return cumSum(j) - cumSum(i-1);
    }

private:
    // return prefix sum till the i-th element
    int cumSum(int i) {
        if (i < 0) return 0;
        int sum = 0;
        for (int key = i + 1; key > 0; key &= (key-1)) sum += tree[key];
        return sum;
    }

    vector<int> tree; // fenwick tree array
    vector<int> data; // the updated data
};
