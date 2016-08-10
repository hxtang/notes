// a simple demo illustrating the idea of bloom filter

// a container of n hash functions, which are computed by double hashing C++'s native hash function
// the container is configured by two parameters:
// max_val:  generated hash code has to be in range [0, max_val)
// num_hash: number of hash functions n
class hash_funcs {
public:
    hash_funcs (int maxv, int nh) : max_val(maxv), num_hash(nh) {};
    
    static size_t simple_hash(string str) {
        std::hash<std::string> str_hash;
        return str_hash(str);
    }

    size_t kth_hash(size_t code, int k) {
        size_t code_high = code >> 16;
        size_t code_low = code & 0xFF;
        return (code_high * k + code_low)%max_val;
    }

    const size_t max_val;
    const size_t num_hash;
};

// a bloom filter for strings, it supports two operations
// set:          add string to the bloom filter
// may_exist:    check if string may have been added before
class bloom_filter {
public:
    bloom_filter(int len, int nh) : bitmap(len, 0), hfun(len, nh) {};
    void set(string str) {
        size_t code = hfun.simple_hash(str);
        for (int k = 0; k < hfun.num_hash; ++k) 
            bitmap[hfun.kth_hash(code, k)] = true;
    }

    bool may_exist(string str) {
        size_t code = hfun.simple_hash(str);
        for (int k = 0; k < hfun.num_hash; ++k) 
            if (!bitmap[hfun.kth_hash(code, k)]) return false;
        return true;
    }
    
private:
    vector<bool> bitmap;
    hash_funcs hfun;
};
