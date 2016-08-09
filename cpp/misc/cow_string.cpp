// a toy example to help understanding reference count

// a copy on write string which supports to_upper()
// its copy constructor and copy assignment operator does not create a real c++ string until to_upper is called
class cow_string {
    //pimp class encapsulating a real c++ string and a reference count
    struct cow_string_impl {
        string buffer;
        int ref_count;        
    };

    cow_string_impl* impl;
    
    void decrease_count() {
        --impl->ref_count;
        if (impl->ref_count == 0) delete impl;    
    }
    
    void increase_count() {
        ++impl->ref_count;
    }
    
    static void toupper(string &s) {
        for (auto& c : s) c = (c>='a' && c<='z')?c + 'A' -'a' : c;  
    }
    
public:   
    //return a copy of the c++ string reference
    string operator() () { return impl->buffer; };

    //constructor
    cow_string(const string& str) {
        impl = new cow_string_impl{str, 1};        
    }

    //copy constructor
    cow_string(const cow_string& str) {
        impl = str.impl;
        increase_count();
    }

    //no move constructor
    cow_string(cow_string&& str) = delete;

    ~cow_string() {
        decrease_count();
    }

    //return size of the string
    size_t size() const {
        return impl->buffer.size();
    }

    //making all lower-case alphabetical characters to upper case
    void toupper() {
        if (impl->ref_count != 1) {
            cow_string_impl *new_impl = new cow_string_impl{impl->buffer, 1};
            toupper(new_impl->buffer);       
             
            decrease_count();
            impl = new_impl;
        }
        else {
            toupper(impl->buffer);
        }
    }

    //copy assignment    
    cow_string& operator= (const cow_string& str) {
        decrease_count();
        impl = str.impl;
        increase_count();
        return *this;
    }    
};
