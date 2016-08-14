code snippets illustrating the idea some well-known algorithms in system

the code is designed to be simple so that ideas appear intuitive, rather that flexible/robust for production code

* bloom filter
  * idea: maintain n hash functions outputing hash codes in [0, l) and a bit map of length l
  * when a key is added, it sets the corresponding n bits in the bitmap
  * when a key is checked, the class checks if all n bits in the bitmap are set
* consistent hashing
  * idea: maintain a circular list of nodes, which can be dynamically added/removed
  * when a key arrives, find the first node after it in circular order.
* leaky bucket (sends at most N packets per second)
  * maintain the number of packets that can be sent at the current second, update this number to N every second
  * put all incoming packets in a waiting queue
  * in an infinite loop, send the first packet in the queue when its size is less than the number of packets that can be sent
* token bucket (sends at most N packets, and on average not greater than M packets per second)
  * generate M tokens every second, but limit total number of tokens to at most N
  * put all incoming packets in a waiting queue
  * in an infinite loop, send the first packet in the queue when its size is less than the number of tokens
