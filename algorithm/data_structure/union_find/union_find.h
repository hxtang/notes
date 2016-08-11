// simple weightd union-find implementation with path compression
class union_find {
public:
    union_find(int n) : prev(n, 0), weight(n, 1), count(0) {
        iota(prev.begin(), prev.end(), 0);
    };

    int union (int first, int second) {
        int r1 = get_root(first, prev); 
        int r2 = get_root(second, prev); 
        if (r1 != r2) {
            --count;
            int rs = weight[r1]<weight[r2]? r1:r2; //smaller root
            int rl = weight[r1]<weight[r2]? r2:r1; //larger root
            prev[rs] = rl;
            weight[rl] += rs;
        }
        return count;
    }

    int get_root(int n, vector<int>& prev) {
        int r;
        for (r = n; r != prev[r]; r = prev[r]);
        for (int i = n; r != i; i = prev[i]) prev[i] = r; //compression
        return r;
    }

private:    
    vector<int> prev;
    vector<int> weight;
    int count;
};
