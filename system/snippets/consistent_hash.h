// a minimal toy implementation illustrating idea of consistent hashing

// consistent hashing class supporting three ops:
// add_node: add node id at location key, do nothing if there's already a machine at location key
// remove_node: remove machine at location key
// get_node: return the machine id closest to location key
// (key can be any value of size_t)
class consistent_hash {
public:
    void add_node(size_t key, size_t id) {
        if (nodes.find(key) == nodes.end())  nodes[key] = id;
    }
    
    void remove_node(size_t key) {
        nodes.erase(key);
    }
    
    size_t get_node(size_t key) {
        if (nodes.size() == 0) throw "no nodes available";
        
        auto it = nodes.lower_bound(key);        
        if (it == nodes.end()) it = nodes.begin();
        return it->second;
    }

private:
    map<size_t, size_t> nodes;
};
