# Ch1 Hello concurrency world

###Concurrency & Multithreading

**Definition of concurrency** -- a single system performing multiple independent activities in parallel

* Task switching: illusion of concurrency on a single processor/single core computer. 
* Hardware concurrency: true concurrency with multiprocessor computers and multicore processors.

Note: 
* Task switching behaves slightly different from true concurerncy in memory model
* Context switching cannot be avoided even with hardware concurrency as # jobs is often more than #cores.

**Approaches to concurrency**
* Multiple processes: processes communicate with signals, sockets, files, pipes, etc.
* Multiple threads: communicate by sharing address space, direct data access with global variables, and by passing parameters

multiprocess safer and easier to scale, but multithread simpler to setup and much more efficient

|          |  multiprocess |  multithread |
|----------|:-------------:|:-------------:|
| safety |  at a higher level with additional OS protection |requires taking care of data consistency|
| scalability | can be between computers in a network  | restricted to a single machine |
| simplicity  | hard to setup; depends on API of OS |easier to program; supported by mainstream progrmaming languages|
| efficiency | Communication and creation both heavy-weight | light-weight|

###When to use concurrency
**Pros:** Separation of concerns and performance

* task parallelism (e.g. MVC): less delay, and simplifies programming logic
* data parallelism (e.g. data pipeline): higher throughput

**Cons:** 

* development/maintainence cost
* consumption of OS resource: Each thread has its own kernel resource & stack space, takes time to start, eats OS stackspace
* communication cost: network connections and context switch

### A minimal example
```cpp
#include <iostream>
#include <thread>

void hello()
{
    std::cout<<"Hello Concurrent World\n";
}

int main()
{
    std::thread t(hello);
    t.join();
}
```

Notes:
1. Include the thread library in STL
2. Write thread implementation in a separate function
3. Start thread by creating a thread object
4. Block the main thread until thread t finishes

###Additional explaination for the need to join (block main thread)

**Short answer:** main thread may finish and kill thread t before t runs

**Long answer:**

The life cycle of a thread includes
* New: thread created
* Runnable: ready to run, but waiting for CPU cycle
* Run: executing steps in the thread
* Suspended: thread sleeping according to CPU scheduling
* Waiting: wait for notification
* Stopped: finish running

The line 
```cpp
std::thread t(hello);
```
creates the thread, and puts it to runnable state, but it is up to the CPU scheduling to determine when the thread is put to run.



