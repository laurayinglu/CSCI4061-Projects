/*test machine: CSEL-KH4250-27 * date:  11/13/2019
* name: Ying Lu , Junjie Ma
* x500: lu000097 , ma000098 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "header.h"
//#include "consumer.c"
//#include "producer.c"
// pthread.h included in header.h


int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("Wrong input numbers \n");
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    consumerNum = atoi(argv[1]);
    char *filename = argv[2];
    char *option = argv[3];
    linenum = 0;

    if (consumerNum <= 0)
    {
        printf("Invalid consumer number input\n");
        EXIT_FAILURE;
    }

    // declare the pthreads
    pthread_t consumer[consumerNum];
    pthread_t producer;
    buff = createQueue();

    // create the initialize the shared buffer
    int QueueSize = atoi(argv[4]);

    // initialize the array
    for (int i = 0; i < MAX_CAPACITY; i++)
    {
        freq[i] = 0;
    }

    remove("log.txt");
    remove("result.txt");

    // input is "./wcs #consumer filename -p [#queue size]"
    // input is "./wcs #consumer filename -bp [#queue size]"
    if ((!strcmp(option, "-p")) || (!strcmp(option, "-bp")) || (!strcmp(option, "-b")))
    {
        // initialize the semaphores
        sem_init(&Space, 0, QueueSize); // buffer can receive buffSize strings before it becomes full
        sem_init(&Item, 0, 0);          // initially the buffer is empty
        sem_init(&buffLock, 0, 1);      // initially we allow one producer to access the buffer

        buff->fname = filename;
        if ((!strcmp(option, "-p")) || (!strcmp(option, "-bp")))
        {
            // Log Printout
            buff->printLog = true;
        }

        pthread_create(&producer, NULL, producerThread, buff);

        pthread_mutex_init(&mutex, NULL);

        // Create the consumer threads
        for (int i = 0; i < consumerNum; i++)
        {
            struct ConsumerPara *consumerP = (struct ConsumerPara *)malloc(sizeof(struct ConsumerPara));
            consumerP->buff = buff;
            consumerP->consumerId = i;
            pthread_create(&consumer[i], NULL, consumerThread, consumerP);
        }

        pthread_join(producer, NULL);

        for (int j = 0; j < consumerNum; j++)
        {
            pthread_join(consumer[j], NULL);
        }

        //printf("finished\n");
        WriteFinalResult();
        //printf("finished2\n");

        pthread_mutex_destroy(&mutex);
        sem_destroy(&Space);
        sem_destroy(&Item);
        sem_destroy(&buffLock);

        // call logprint function
        return EXIT_SUCCESS;
    }
    else
    {
        printf("Invalid input for option \n");
        return EXIT_FAILURE;
    }

    printf("Wrong input order.\n");
    return EXIT_FAILURE;
}
