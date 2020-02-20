###### test machine: CSEL-KH4250-27
###### date: 11/13/2019
###### name: Ying Lu
###### x500: lu000097
***
* The purpose of our program:
    * In this program we will use multithreading to calculate the number of word starting letter of a file.
    * The base model of this program is Producer- Consumer problem.

***

* How to compile the program:
    * make clean
    * make wcs
    * On the commend line, we type "./wcs #consumer filename [option] [#queue_size]". 

    * "#consumer" is the number of consumers the program will create.
    * "filename" is the name of input file.
    * "option" has three situations: "-p", "-b", "-bp". ("-p" means printing, the program will generate log in this case; "-b" means use bounded buffer; "-bp" means both bounded buffer and log printing.)
    * "queue_size" is the queue size if using bounded buffer.
    * In our case, since we used bounded buffer, option only has two cases: "-b" or "-bp"

***


* What exactly the program does:
    * First, we will create a thread-safe shared queue. This queue is as linked-list bounded buffer. We will inserts the data in the tail of the linked-list and the consumer extracts the data from the head.
    
    
    
    * Second, we need to create a master thread, and we need to do initialization and check inputs and errors. This thread control whole process of program and all threads. 
    
    
    
    * Third, the producer part. The producer will read the file line by line, one line at a time. The producer also will work on one line at a time. Once the file reached EOF, the producer will send notifications to consumers and the producer terminates after sending those notifications.
    
    
    
    * Fourth, the consumer part. The consumer will check the queue, and work on it. Then it go back to get the next part of the queue. It will terminate until it receives the EOF notification. Consumers will work for the word, and all other characters that are not words seemed as blank. After all consumers finish their work, the master thread could summarize it and generate the output.
    
    
    
    * Finally, the producer and consumers finish, the master thread will write the final result in to a new file named "result.txt". The program also creates a file named "log.txt" if the argument option is specified. The log file will include the using information of producer and consumers.

***

* Individual contributions:
  * We design and discuss about the whole algorithm of this program, write code and README together.


***


* Assumptions outside the document:
  * NONE


***

* For extra credit:
  * We have attempted extra credit.
  * Used linked list implemented bounded buffer, in this case, we use three semaphores to check if the buffer is full/empty/locked, also we use a pthread mutex when each consumer deals with line and updated it into global freq array
