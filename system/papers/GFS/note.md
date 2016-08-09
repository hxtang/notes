##Overview
###Challenges
* Distributed file system with high throughput:
  How to avoid (1) locking (2) network bottleneck (3) data flow blocking control flow
* High availability with low cost servers:
  Cannot afford expensive servers, availability has to be solved by fault management

###Interface
* GFS is a File system build on top of local linux file systems
* Client sees a typical file system tree.
* GFS servers help client to manage the linux files relevant to the GFS file of interest

Idea: solve the above problem around Google's specific need (search data)

###Assumptions
* modest number (a few M) of large files (100MB+)
* random writes is (almost) non-existence. new data overwrites old data by creating new files and deleting old files
* data are records, so order does not matter

##Architecture
![](https://www.researchgate.net/profile/Marta_Mattoso/publication/278629912/figure/fig14/AS:282262210924544@1444307963060/Figure-15-GFS-architecture-45.png)
* __master__ maintains control information (metadata)
* __slaves__(chunk servers/clients) maintain the data (chunks)
* __control flow__(thin lines) mostly betweem master & slave
* __data flow__(thick lines) always among slaves
* __chunks__ (cylindrals) building block of files, each is 64M, has 3 replica, are local linux files

###Questions to be answered
* How to implement read, write, append, snapshot regardless of concurrency?
* Is data consistency achieved with concurrent write requests?
* Are files ever moved/deleted?
* What happens when master is down
* What happens when chunk server is down (can be as catastrophic as blackout in a city) 
* How to handle data corrupted due to write failure?

##High throughput file operations

###Data structure
* Master:
  * file/chunk namespaces
  * file-to-chunk mapping
  * chunk locations (not logged, but pulled from chunk server at startup)
* Server:
  * chunks (with checksum)
  * buffer
  * (maybe) client applications

###Read
TODO: add image

(1) chunk index & range
(2) chunk location and handle
(3) chunk handle & range
(4) data

###Write
TODO: add image

(1) chunk index & range
(2) chunk location, handle, primary server
(3) data (cached in chunk server buffer)
(4) write request
(5) serial order of mutations
(6) notificiation of completion
(7) notification of success/failure

* **when write go beyond boundary**: handled by client code
* **leasing:** client writes to chunk servers are managed by the lease mechanism with 60s timeout.

###Record append
in addition to the steps of random read
* push data to last chunk, record size is limited to < ¼ chunk size
* primary determines if write go beyond chunk boundary and notifies client

###Snapshot
in addition to the steps of random read
* master removes lease to chunks to be snapshoted
* master creates a duplicate of metadata pointing to source chunks
* upon next write, master notice reference count of a chunk is more than 1, and requests all chunk servers with the chunk handle to copy the chunk in the same local machine

###Create/Deletion
see "optimization" section

###Design choices
* chunks

  Q: chunk introduces many internal fragments, is it OK?
  
  A: we never need to fit all the chunks into memory, so it's all about buy new disks, so it's OK.

  Q: why 64M per chunk? i.e. why not variable-size, why not smaller size?

  A: chunk is used as the unit of allocation, data transfer and replica
  * Fixed-size chunks is easier to manipulate than variable-size, it's like blocks in files
  * 64M chunk size means chunk descriptor (<64byte) requires 1/M of datasize. 
  * This chunk descriptor capacity is small enough to almost fit into master's memory.

* master

  Q: why master-slave? wouldn't there be a network bottleneck around the master?
  
  A: It's really about separating data flow and control flow communications.
  * Master is dedicated to control, so such info would not need to wait for data transfer.
  * Control flow are really short, so does not create bottlenecks.

* chunk servers

  Q: where are the client servers?

  A: the chunk servers (e.g. in map-reduce) may host both data and applications if resource/reliability not an issue.

  Q: why aren't there any caches?

  A: most client applications stream through large files
     chunk servers save files locally, so freq. Accessed data already cached by linux

  Q: why serial reads?
  
  A: serial reads are the means to ensure data consistency when multiple writes occur to the same chunk. I do wonder if it slows things down when a chunk server gets lots of concurrent writes.

##Optimization
###Chunk creation/rebalancing
* Chunks may be migrated to those with lower utility
* Chunks created at similar time are preferred to be placed at different locations, as this is a good implication that these chunks will be visited at similar times.
* Also see discussion about chunk placement in Fault-managment section

###Deletion
* Stale chunks are identified and deleted by a regular scan by master
* Deleted files are kept on disk for a few days in case of recovery. 
* But no worry about running out of space since disk is cheap

##Fault-management

###Chunk server failure
* Heart-beat monitoring: the master send instructions and servers send states periodically
* Chunk replica:
  * Each chunk has 3 replica to (1) have a backup (2) reduce time window of silent loss of data (e.g. when a sector of disk is scratched, you don't know until the next read/master scan)
  * Replica should be distributed at different racks (or even different buildings) in case of a catastropic event
  * Master moniters the number of replicas for each chunk. Those with two replicas will be scheduled to be copied, those with only one copy are put on urgent list and have high priority to be replicated

###Master failure
* Log/checkpoints
  * Metadata mutations are logged and checkpoint are created 
  *exception: no log for chunk locations, they are uploaded from chunks at startup
* Master-replica
  * A master-replica take snapshot of master periodically. When master is down, the replica replaces master

###Data corruption
* Each chunk server independently detects data corruption by checksum
  * Checking consistency among replica too slow
  * 4bytes for every 64KB
  * "Eager" checksum for read/overwrite, "lazy" checksum for append, scan for inactive chunks
  * When checksum fail, error message returned and client requests read from another replica
