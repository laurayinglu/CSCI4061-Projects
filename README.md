# CSCI4061-Projects

This is all projects I did for Csci4061 - Intro to Operating System

### Project 1
Given a makefile filename and an optional target as command line arguments, your task is to generate an executable ’mymake’, from a C program. It must parse the makefile and build a dependency graph for the (optional) provided target. Also, execute the recipes for the said target, by spawning a new process per recipe, via fork and exec, as determined by the graph traversal. If no target is specified as a command line argument, the first target in the makefile is chosen.

Alternatively, if your first argument is a print flag (-p) followed by the makefile, you must only print the target, dependencies, and recipes in the makefile. A second type of flag (-r) followed by the makefile must print the recipe order of the target. Do not fork/exec the recipes when running either of the flags. 

### Project 3

In multithreading “word count statistics” application, the program contains: a master thread, one producer thread and many consumer threads (shown in Fig. 2 below). The producer and consumers will share a queue for data transfering. For program execution flow, the entire program contains 4 parts: 

1) the master thread initializes the shared queue, result histogram, producer thread and consumer threads; 

2) the producer thread will read the input file, cut the data into smaller pieces and feed into the shared queue; 

3) the consumers will read from the shared queue, compute the “word count statistics” for its data pieces and synchronize the result with a global histogram; 

4) after producer and all consumers complete their work, the master thread writes the final result into the output file.

### Project 4

Making two executables (two separate independent programs), server program and client program. Master process is the main process of the client program. Master process spawns multiple mapper processes similar to PA2. Reducer process is the main process of the server program. Note that the reducer process is no longer spawned by the master process. The reducer process spawns a thread whenever it establishes a new TCP connection with a client. It is called a multi-threaded server. Server, Reducer and Reducer process are interchangeably used in this document.

The client program has two types of clients - Master clients and Mapper clients. Master process and mapper processes can be a client. Master client implementation is extra credit. Details of master client can be found in the section 5. Extra credit. Now, Let’s fist focus on the mapper clients. Each mapper process initiates a TCP connection to the server. Once the connection is established, the mapper client sends deterministic requests, a fixed set of requests, to the server. Mappers, Mapper clients, and Master’s child processes indicate the same thing.
