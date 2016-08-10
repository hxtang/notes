code snippets illustrating the idea some well-known algorithms in system
the code is designed to be simple so that ideas appear intuitive, rather that flexible/robust for production code

* bloom filter
  * idea: maintain n hash functions outputing hash codes in [0, l) and a bit map of length l
  * when a key is added, it sets the corresponding n bits in the bitmap
  * when a key is checked, the class checks if all n bits in the bitmap are set
* consistent hashing
  * idea: maintain a circular list of nodes, which can be dynamically added/removed
  * when a key arrives, find the first node after it in circular order.
  

