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
