There are 2 folders in this lab folder

The first, lab5messagequeue, has the code and results for the message queue (part b) of the lab
Relevant code is in the mqueue folder. Perform make clean. *note* make clean can cause errors but this
can be ignored since we are just deleting mailboxes and semaphores if they exist before starting. Then make. Then ./produce <N> <B> <P> <C> 

The second, lab5sharedmemory, has the code and results for the shared memory (part a) of the lab. *note* the 'sender.c' has everything that is relevant for this section, no need to read or do anything with the other files. 
Relevant code is in the mqueue folder. Perform make clean. *note* make clean can cause errors but this
can be ignored since we are just deleting mailboxes and semaphores if they exist before starting. Then make. Then ./produce <N> <B> <P> <C>