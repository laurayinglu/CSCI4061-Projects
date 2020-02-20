/*test machine: CSEL-KH4250-27 * date:  11/13/2019
* name: Ying Lu , Junjie Ma
* x500: lu000097 , ma000098 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "header.h"
// pthread.h included in header.h

// implement shared queue, final histogram here..
struct Record *NewRecord(char line[MAX_LINESIZE], int linenum)
{
    struct Record *temp = (struct Record *)malloc(sizeof(struct Record));
    strcpy(temp->line, line);

    temp->next = NULL;
    temp->lineNum = linenum;
    return temp;
}

struct Queue *createQueue()
{
    struct Queue *q = (struct Queue *)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    q->printLog = false;
    q->fname = "";
    q->isEof = false;
    return q;
}

// enqueue at the rear
void enqueue(struct Queue *q, char line[MAX_LINESIZE], int linenum)
{
    struct Record *temp = NewRecord(line, linenum);

    if (q->rear == NULL)
    {
        q->front = q->rear = temp;
        // q->size++;
        return;
    }

    q->rear->next = temp;
    q->rear = temp;
}

// dequeue from the front
struct Record *dequeue(struct Queue *q)
{
    if (q->front == NULL)
        return NULL;

    struct Record *temp = q->front;

    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    return temp;
}

/*
bool isEmpty(struct Queue* q)
{
    return (q->size == 0);
}
*/
void WriteFinalResult()
{
    FILE *fp;

    fp = fopen("result.txt", "a+");

    if (fp == NULL)
    {
        fprintf(stderr, "Can't open the file \n");
        exit(1);
    }

    for (int i = 0; i < MAX_CAPACITY; i++)
    {
        fprintf(fp, "%c: %d\n", i + 'a', freq[i]);
    }

    fclose(fp);
}

void printLogProducer(int linen)
{
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

        fprintf(logfp, "producer\n");
        //printf("producer\n");
        fprintf(logfp, "producer: %d\n", linen);
        //printf("producer: %d\n", linen);

        fclose(logfp);
    }
}

void printLogConsumer(int consumerId, struct Record *result)
{
    // if(buff->printLog)
    //{
    // print info into log.txt
    FILE *logfp;

    logfp = fopen("log.txt", "a+");

    if (logfp == NULL)
    {
        fprintf(stderr, "Can't open the file \n");
        //exit(1);
    }

    fprintf(logfp, "consumer %d\n", consumerId);
    fprintf(logfp, "consumer %d: %d\n", consumerId, result->lineNum);

    //printf("consumer %d\n", consumerId);
    //printf("consumer %d: %d\n", consumerId, result->lineNum);

    fclose(logfp);
}
