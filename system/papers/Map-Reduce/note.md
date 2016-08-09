##Programming model
* Map: (key1, value1) -> list<(key2, value2)>
* Reduce: (key2, list<value2>) -> list<value2>

##Architecture
TODO: add image

1. MR Library splits data into M pieces of 16-64MB data
2. Master assign M map workers and R reduce workers
3. Map workers compute the map step and do a local write to buffer.
4. Map results in buffer written to local disk into R parts, and results informed to master
5. Each reduce worker reads data from M map workers when ready and then sort&combine them
6. Each reduce worker iterate over each key, call reduce function and write results to the global file system
7. Master wakes up user program when all reducers finish

##Fault management
### Worker failure
* Master pins all worker periodically to detect worker failure
* Incomplete jobs will be rescheduled to other workers
* Completed jobs on master will be rescheduled to other workers

###Master failure
* In theory: checkpoints + logs
* In practice: simply restart the system-,-

##Optimization
* The master attempt to assign map workers to be those containing a replica of input, or machines near it
* M and R are chosen to be much larger than #worker machines, but M and R should not be so large to significantly increase #master scheduling decisions of O(M+R), and memory to hold the O(MR) states
* The master starts backup tasks of incomplete jobs whenever the map-reduce operation is nearly complete to avoid straggler.
