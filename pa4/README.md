
###### test machine: CSEL-KH4250-32
###### date: 12/06/2019
###### name: Ying Lu
###### x500: lu000097
***
* The purpose of our program:
  In this program, we will extend the word counting application of programming assignment 2 using the socket programming. We will also use socket programming to control multi-threading.

***

* How to compile the program:
  This program has two part: server and client.

  * For server part, we need to type "./server < Server Port > ".
      - The <server port> is any unsigned integer to be used as a port number.
      **Compile**:
      - make clean
      - make server
      **Run**:
      - eg ./server 8088

  * For client part, we need to type "./client < Folder Name > < # of Mappers > < Server IP > < Server Port >".
      - <Folder Name> the name of the root folder to be traversed.
      - <# of Mappers> the number of mapper processes spawned by master.
      - <Server IP> IP address of the server to connect to.
      - <Server Port> port number of the server to connect to.
      **Compile**:
      - make clean
      - make client
      **Run**:
      - eg ./client Testcases/TestCase2 5 127.0.0.1 8088

***


* What exactly the program does:
  * In this program, there are two parts: server and client. Similar to the PA2,
    master process is the main process of the client, it spawns multiple mapper
    processes. Reducer process is the main process of the server, and it spawns
    a thread whenever it establishes a new TCP connection with a client.

  * In the server program, the server is responsible for listening and building
    connections from clients. Then the server receives a connection, it should
    spawn a new thread to handle that connection. the thread is responsible for
    handling requests from a client by reading client's messages from a socket
    set up between them, doing necessary computation and sending responses to
    the client back through the socket.

  * The client program has three phases, first part is same with phase 1 of PA2.
    Then master process assigns a unique mapper ID to mapper processes while it
    spawns mapper processes. These processes run in parallel. Each mapper client
    sets up a TCP connection to the server, and sends one of six kinds requests
    sequentially in a deterministic manner.

  * (extra credit?)

  * Finally, the server program prints the following logs in terminal, the client
    print the following logs in a log file "log_client.txt" in the "log" folder.






***

* Individual contributions:
  * We work together to figure out which data structures to use and implement together.


***


* Assumptions outside the document:
  * We code the server based on lab 13.
  

***

* For extra credit:
  * Yes.
  * In the server side, in the thread function, we check if the mapperID is -1, then we deal with it based on the request command
  * In the client side, we implement the mapMaster function and call it after mapClient function to make sure all mapper processes are terminated, then we read the command file line by line, for each command we create a TCP connection then doing the corresponding things based on the command
  * When we compile, it shows connection refused somehow

