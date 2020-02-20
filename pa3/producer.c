/*test machine: CSEL-KH4250-27 * date:  11/13/2019
* name: Ying Lu , Junjie Ma
* x500: lu000097 , ma000098 */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "header.h"
// pthread.h included in header.h

// read the input file, cut the data into samller pieces and feed into shared queue
void *producerThread(void *arg)
{

    struct Queue *buff = (struct Queue *)arg;

    FILE *fp = fopen(buff->fname, "r");

    if (fp == NULL)
    {
        fprintf(stderr, "Can't open the file \n");
        exit(1);
    }

    char line[MAX_LINESIZE];
    int linenum = 0;
    while (fgets(line, MAX_LINESIZE, fp))
    {

        usleep(rand() % 10);
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = 0; // get rid of \n

        sem_wait(&Space);
        sem_wait(&buffLock);

        //critical section below
        // add

        //printf("the line is %s\n", line);
        enqueue(buff, line, linenum);

        //
        //buff->lineNum++;

        printLogProducer(linenum);
        linenum++;
        // printf()

        sem_post(&buffLock);
        sem_post(&Item);
    }

    // when reach eof
    sem_wait(&buffLock);
    buff->isEof = true;
    
    sem_post(&buffLock);
    
    for (int i = 0; i < consumerNum; i++)
    {
        sem_post(&Item);
    }

    //printf("producer quit\n");
}
