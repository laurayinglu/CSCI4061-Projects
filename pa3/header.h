/*test machine: CSEL-KH4250-27 * date:  11/13/2019
* name: Ying Lu , Junjie Ma
* x500: lu000097 , ma000098 */

/*
header.h, header for all source files
it will: 
- structure definition
- global variable, lock declaration (extern)
- function declarations
*/

#ifndef _HEADER_H_
#define _HEADER_H_

//#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>

#define MAX_CAPACITY 26
#define MAX_LINESIZE 1024
#define END -1
// shared buffer for producer and consumer
sem_t Space;
sem_t Item;
sem_t buffLock;
pthread_mutex_t mutex;

int freq[MAX_CAPACITY];
int linenum;
int consumerNum;

struct Record
{
    char line[MAX_LINESIZE];
    int lineNum;
    struct Record *next;
};

struct Queue
{
    //int size;
    bool isEof;
    bool printLog;
    char *fname;
    struct Record *front, *rear;
};

struct Queue *buff;

struct ConsumerPara
{
    int consumerId;
    struct Queue *buff;
};

struct Record *NewRecord(char line[MAX_LINESIZE], int linenum);
struct Queue *createQueue();
void enqueue(struct Queue *q, char line[MAX_LINESIZE], int linenum);
struct Record *dequeue(struct Queue *q);
bool isEmpty(struct Queue *q);

void *consumerThread(void *arg);
void *producerThread(void *arg);

void printLogProducer(int linen);
void printLogConsumer(int consumerId, struct Record *buff);

// count the word in one line
void MapLine(char line[MAX_LINESIZE]);

// write result into file "FinalResult.txt"
void WriteFinalResult();

#endif
