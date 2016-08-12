#Ch3 Sharing data between threads

##Problems with sharing data between threads

### Problems
* broken invariants: conditions become invalid in multithread environments
* race conditions: result depends on order of arriving threads
  * data race: multiple threads simultaneously attempt to write the same data

### Solutions
* wrap data with a protection mechanism, such as mutex (the basic method)
* lock-free programming: often involves complex design
* transaction: log all operations, and commit a merge of logs

##Protecting shared data with mutexes

syntax
```c++
#include <mutex>
std::mutex some_mutex;

void protected_function {
    std::lock_guard<std::mutex> guard(some_mutex);
    // the remainder of work
};
```

### structuring code for protecting shared data
Note: the mutex-lock_guard combination working by protecting a scope, rather than a data. Therefore

1. all pieces of code that access the protected data should be marked as mutual exclusive
2. should not pass pointer/reference to the protected data to outside of the locked scope
Special care should be taken about user functions.

### spotting race conditions inherent in interface

The way we call member function g() relies on result of function f() we call earlier, 
but the result of f() is out-of-date when g() is called

Example: a C++ stack is not thread-safe even if all its data is protected by mutex

* thread t1 calls stack.empty() and the stack has a single element, therefore returns false
* thread t2 calls stack.empty() too, and the stack returns false
* t1 calls stack.pop(), the stack is now empty
* t2 calls stack.pop(), the stack 

Solutions

* pass in a reference for pop()
  issue: the datatype maynot support return-by-value
* require a no-throw copy constructor or move constructor from user
  issue: not always possible
* return a smart pointer (shared) to the data
* method 1 + method 2 or method 1 + method 3

```c++
#include <exception>
#include <memory>
#include <mutex>
#include <stack>

struct empty_stack: std::exception {
    const char* what() const throw();
};

template<typename T> class threadsafe_stack {
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack(){}
    threadsafe_stack(const threadsafe_stack& other) {
        std::lock_guard<std::mutex> lock(other.m);
        data=other.data;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;
    
    void push(T new_value) {
        std::lock_guard<std::mutex> lock(m);
        data.push(new_value);
    }
    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        std::shared_ptr<T> const res(std::make_shared<T>(data.top()));
        data.pop();
        return res;
    }
    
    void pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) throw empty_stack();
        value=data.top();
        data.pop();
    }
    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};
```
