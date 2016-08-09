class Treap {
    struct Node { 
        int key, priority, size;
        Node *left, *right;

        Node (int k, int p): key(k), priority(p), size(1), left(NULL), right(NULL) {};
        ~Node() { delete left; delete right; }
    };

    inline static int get_size(Node *node) { return node? node->size : 0;}    
    
    void rotate(Node*& gref, Node*& pref, Node* &cref) {
        //rotate 
        Node* const g = gref, * const p = pref, * const c = cref;
        pref = c, cref = g;  

        //update size       
        swap(p->size, g->size);
        g->size = get_size(g->left) + get_size(g->right) + 1;        

        gref = p; //update reference to new root
    }

    int insert_recur(Node* &nref, int key, int priority) {
    	int count = 0;

    	if (nref == NULL) nref = new Node(key, priority);
       else {
           bool go_left = key <= nref->key;

           //insert and count #nodes < key 
           Node*& child = go_left? nref->left : nref->right;
           count += insert_recur(child, key, priority);
      	    if (!go_left) count += get_size(nref->left)+1;

           //rotate and update size 
           ++nref->size;
           if (child->priority > nref->priority) 
               rotate(nref, child, go_left? child->right : child->left); 
        }
        return count;
    }

    Node* root = NULL;
    
public:
    ~Treap() { delete root; }
    int insert(int key) { return treap_insert(root, key, rand()); }

    void delete(int key) {
      //todo: 
    }
};
