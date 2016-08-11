class Trie {

class TrieNode {
public:
    // Initialize your data structure here.
    TrieNode() {};
    ~TrieNode() {
        for (int k = 0; k < 26; ++k) delete children[k];
    }

    TrieNode *children[26] = {NULL};
    bool is_word = false;
};

public:
    Trie() { root = new TrieNode(); }
    
    ~Trie() { delete root; }

    // Inserts a word into the trie.
    void insert(string word) {
        TrieNode *cur = root;
        for (char c : word) {
            if (cur->children[c-'a'] == NULL) cur->children[c-'a'] = new TrieNode();
            cur = cur->children[c-'a'];
        }
        cur->is_word = true;
    }

    // Returns if the word is in the trie.
    bool search(string word) {
        TrieNode *cur = root;
        for (char c : word) {
            if (cur->children[c-'a'] == NULL) return false;
            cur = cur->children[c-'a'];
        }
        return cur->is_word;
    }

    // Returns if there is any word in the trie
    // that starts with the given prefix.
    bool startsWith(string prefix) {
        TrieNode *cur = root;
        for (char c : prefix) {
            if (cur->children[c-'a'] == NULL) return false;
            cur = cur->children[c-'a'];
        }
        return true;        
    }

private:
    TrieNode* root;
};
