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

// read from the shared queue, compute the word count stats for its pieces
// and sychronize the result with a global histogram

// consumer call this function
// strtok is not thread-safe
void MapLine(char *line)
{
    //printf("the line is %s\n", line);
    pthread_mutex_lock(&mutex);
    // deal with special chars
    for (int i = 0; i < strlen(line); i++)
    {
        line[i] = tolower(line[i]);
        if (line[i] < 'a' || line[i] > 'z')
            line[i] = ' ';
    }

    bool startOfWord = true; //ab bs b  bjd  d

    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] == ' ')
        {
            startOfWord = true;
        }
        else
        {
            if (startOfWord)
            {
                ++freq[line[i] - 'a'];
            }
            startOfWord = false;
        }
    }

    pthread_mutex_unlock(&mutex);
}

void *consumerThread(void *arg)
{

    struct ConsumerPara *consumerP = (struct ConsumerPara *)arg;
    int consumerId = consumerP->consumerId;
    struct Queue *buff = consumerP->buff;
    free(consumerP);
    while (1) // && !isEmpty(buff))
    {

        sem_wait(&Item);

        sem_wait(&buffLock);
        if (buff->isEof && buff->front == NULL)
        {
            // printf("consumer %d eof and empty queue\n", consumerId);
            if (buff->printLog)
            {
                // print info into log.txt
                FILE *logfp;

                logfp = fopen("log.txt", "a+");

                if (logfp == NULL)
                {
                    fprintf(stderr, "Can't open the file \n");
                    exit(1);
                }


                fprintf(logfp, "consumer %d: -1\n", consumerId);
                //fprintf(logfp, "producer: %d\n", buff->lineNum);

                fclose(logfp);
            }
            
            sem_post(&buffLock);
            break;
        }

        //critical section below
        // add

        struct Record *result = dequeue(buff);
        //printf("the line is %s\n", result->line);
        if (buff->printLog)
            printLogConsumer(consumerId, result);

        sem_post(&buffLock);
        sem_post(&Space);

        MapLine(result->line);
        //printf("the line is %s\n", result->line);
        free(result);

        usleep(rand() % 10);
    }
    //printf("consumer %d quit\n", consumerId);
}
