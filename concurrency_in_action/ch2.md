# Managing threads

### Thread creation, join and detach
```cpp
#include <iostream>
#include <array>
#include <string>
#include <thread>
#include <chrono>

void example_function() {                                                //1
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout <<"Hello from function\n";
}

class Example_Class {                                                    //2
public:
	void operator()() const {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout <<"Hello from class\n";
	}
};

auto example_lambda = [](){                                              //3
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout <<"Hello from lambda function\n";
};

int main() {
	std::cout << "Launching threads...\n";
	std::array<std::thread, 3> threads {                                   //4
		std::thread{example_function}, 
		std::thread{Example_Class()}, 
		std::thread{example_lambda}
	};
	
	std::cout << "Running threads...\n";

	for (auto &t : threads) t.join();                                      //5
	//for (auto &t : threads) t.detach();                                  //6
	
	std::cout <<"Finish.\n";		
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));	         //7
	return 0;
} 
```

Notes:

1. Defines a function for the thread
2. Defines a callable class for the thread
3. Defines a lamda expression for the thread
4. Launching the threads by creating thread objects
5. Wait for threads to finish with join()
6. Alternatively detach the new threads so that they run independently
7. Give time for detached thread to finish before finishing the process.

**(Step 4) Right reference is passed into the initialization constructor of thread**

The function object is copied into the stack space belonging to the newly created thread and invoked from there. Must make sure the copy's behavior equals the behavior of original.

**(Step 4) Avoid "C++'s most vexing parse"**

The following code contains ambiguity:
```cpp
std::thread t(ExampleClass());
```
It may mean to 

1. launch a thread with the functor ExampleClass, 
2. declare a function t which takes in an unamed function pointer (no parameter, return type: ExampleClass) and returns a thread.  

C++ compilers resolve the ambiguity by parsing with the second interpretation.

Solutions:
```cpp
std::thread t((ExampleClass()));  //extra parenthesis
std::thread t{ExampleClass()};    //list initialization
```

**(Step 5) Must join or detach new thread before main thread finishes**

* As noted earlier, not joining/detaching new thread t may cause t to be destroyed before it runs. This will also render std::terminate() when deconstructor of thread is called.
* Need to be caucious about exceptions occuring before joining (see next example).

We can show this really happens by running above example three times:

|Test run 1|Test run 2|Test run 3|
|:---------------------------|:---------------------------|:---------------------------|
|Launching threads...|Launching threads...|Launching threads...|
|Hello from function|Running threads...|Running threads...|
|Hello from class|Hello from lambda function|Hello from class|
|Running threads...|Hello from class|Hello from function|
|Hello from lambda function|Hello from function|Hello from lambda function|
|Finish.|Finish.|Finish.|

Join and detach can happen only once to each thread, otherwise a compiler error will occur.

**(Step 5) Thread cannot be copied and can only be moved.**

In the for loop we take the reference of each thread.
```cpp
for (auto &t : threads) t.join();
```
Not taking the reference will render compiling error because thread is not copy constructable.

**(Step 6, 7) Join v.s. detach**

Detaching will not block the main thread as join, but finishing the main thread before detached threads will kill all remaining threads. Looks like detach is very tricky to use.

### Exception safety

```c++
struct thread_functor_with_reference {
    int& iref;

    thread_functor_with_reference(int& iref_) : iref(iref_) {};

    void operator() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        cout << iref;                                                     //1
    }
};

void exception_prone_function() {
    //do something likely to raise an exception
}

void function_parent () {
    int local_var = 0;
    thread_functor_with_reference func(local_var);
    std::thread t(func);
    try {
        exception_prone_function();                                           //2
    }
    catch {
        t.join();                                                             //2
        throw;
    };
    t.join();                                                                  //1
}
```
1. When parent thread run out of the scope of function_parent, the variable local_var is destroyed, and it causes undefined behavior. Therefore the new thread has to join.
2. To be exception-safe, t.join() need to be added not only at end of function_parent, but also in the catch block (otherwise t may never be joined in case exception occurs).
But this (1) is verbose (2) prone to bugs.

A better way is to design a exception-safe class
```c++
class thread_guard {
    std::thread& t;

public:
    explicit thread_guard(std::thread& t_) : t(t_) {};                      //1
    ~thread_guard() {  if (t.joinable()) t.join(); }                   //2
    thread_guard(thread_guard const&)=delete;                          //3
    thread_guard& operator=(thread_guard const&)=delete;               //3
};

void function_parent (string filename) {
    int local_var = 0;
    thread_functor_with_reference func(local_var);
    std::thread t(func);
    thread_guard g(t);
    try {
        exception_prone_function();
    }
    catch {
        throw;
    };
}
```

1. the class maintains a reference to the thread
2. on exception or exit of the scope (function_parent), the deconstructor is called, and forces join.
3. a thread is managed by a single guard, therefore it is not copyable.

### Passing parameters

Syntax
```c++
void thread_function(int param1, string& param2);

void parent_function() {
    int param1 = 0;
    string param2;
    std::thread t(thread_function, param1, param2);
}
```
Note by default the parameters are __copied__ even if param2 is defined as reference in thread function. Because thread constructor is not aware of that.

Three fitfalls
* implicit conversion in the passed parameters: conversion does not happen at thread construction, and may not even have happened when the parent thread exits. 
  Solution: do explicit conversion before passing, e.g. 

```c++
void thread_function(int param, string& str);

void parent_function() {
    int param = 0;
    char buffer[1024];
    buffer[0] = 1;
    //std::thread t(thread_function, param, buffer);
    std::thread t(thread_function, param, string(buffer));
    t.detach();
}
```
* function parameter is defined as a reference to return values by parameter, but the thread actually makes a copy of the object.

```c++
void thread_function(int param1, int& param2);

void parent_function() {
    int param1 = 0;
    int param2 = 0;
    //std::thread t(thread_function, param1, param2);
    std::thread t(thread_function, param1, std::ref(param2));
    t.join();
}
```
* a movable, named object is passed into the thread, and the thread function intends to copy it.

```c++
void thread_function(int param1, unique_ptr<int> ptr_param2);

void parent_function() {
    int param1 = 0;
    int param2 = 0;
    unique_str<int> ptr(&param2);
    std::thread t(thread_function, param1, unique_str<int>(&param2));
    //std::thread t(thread_function, param1, ptr);
    std::thread t(thread_function, param1, std::move(ptr));
    t.join();
}
```
In above examples, the commented line causes problems, and are fixed by the line following.

### Transferring ownership
scoped thread

```c++
class scoped_thread {
    std::thread t;
public:
    explicit scoped_thread(std::thread t_) : t(std::move(t_)) { //1
        if (!t.joinable) throw logic_error("no thread");        //2
    }
    ~scoped_thread() { t.join(); };
    scoped_thread(scoped_thread const&)=delete;
    scoped_thread& operator=(scoped_thread const&)=delete;
};
```
1. The constructor of the class takes a thread as input, and therefore can be called with an unnamed variable.
2. In this case, we check if the thread is joinable in constructor.
3. Similar to thread_guard, on deconstrutor of the class called, the join function is called.

move support of thread also allows for move-aware containers of threads.

### Choosing the number of threads at run time
The max number of threads at run time can be acquired by
```c++
unsigned long const max_hardware_threads = std::thread::hardware_concurrency();
```

### Identifying threads
```c++
std::thread t;
std::thread::id t_id    = t.get_id();                 
std::thread::id this_id = std::this_thread::get_id();
```
Notes:
std::thread::id is the data type for thread ids
if a thread object is not associated with an active thread, get_id() returns a default id indicating "not any thread"
