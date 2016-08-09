//a simple BST that does not guarantee balanced structure
class BSTree {
    struct Node {
        Node* left, * right;
        int size, val;
        Node(int v) : left(nullptr), right(nullptr), val(v), size(1) {};
        ~Node() { delete left; delete right; left = right = nullptr; }
    };

    static int insert_recur(Node*& r, int v) {
        if (r == NULL) {
            r = new Node(v);
            return 0;
        }
        else {        
            ++r->size;
            if (v > r->val) return insert_recur(r->right, v) + 1
                                  + (r->left? r->left->size : 0);
            else return insert_recur(r->left, v);
        }
    }

    Node *root = nullptr;

public:    
    ~BSTree() { delete root; }

    int insert(int v) {
        return insert_recur(root, v);
    }
    
    void remove(int v) {
        //todo
    }
};
