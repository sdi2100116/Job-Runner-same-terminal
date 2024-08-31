#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/buffer.h"
#include "../include/jobExecutorServer.h"

pthread_mutex_t mutexBuffer;
pthread_cond_t conditionEmptyBuffer = PTHREAD_COND_INITIALIZER;
pthread_cond_t conditionFullBuffer = PTHREAD_COND_INITIALIZER;

void initializeBuffer(struct buffer *b, int bsize)
{
    b->arr = malloc(sizeof(struct triplet) * bsize);
    b->start = 0;
    b->end = 0;
    b->count = 0;
    b->bsize = bsize;
}
void insertBuffer(struct buffer *b, struct triplet tr)
{ // inserts job into the buffer
    pthread_mutex_lock(&mutexBuffer);
    // blocks till the buffer has at least one free space
    while (b->count >= b->bsize)
    {
        printf("Buffer blocked.Waiting for a job to start running..\n");
        pthread_cond_wait(&conditionFullBuffer, &mutexBuffer);
        printf("Buffer unblocked...\n");
    }
    if (server_terminating) //the client sent exit so all the jobs that were waiting for the buffer to have space have to exit
    {
        char str[]="SERVER TERMINATED BEFORE EXECUTION\n";
        write(tr.clientSocket,str,strlen(str));
        pthread_mutex_unlock(&mutexBuffer);
        pthread_exit(NULL);
    }
    b->arr[b->end] = tr;
    b->end = (b->end + 1) % b->bsize;
    b->count++;
    pthread_mutex_unlock(&mutexBuffer);
}

struct triplet removeBuffer(struct buffer *b)
{ // removes the first element of the buffer
    pthread_mutex_lock(&mutexBuffer);
    struct triplet tr;
    while (b->count <= 0)
    { // blocks till the buffer has at least one element
        pthread_cond_wait(&conditionEmptyBuffer, &mutexBuffer);
    }
    tr = b->arr[b->start];
    b->start = (b->start + 1) % b->bsize;
    b->count--;
    pthread_mutex_unlock(&mutexBuffer);
    return tr;
}

int searchBuffer_and_remove(struct buffer *b, char *job)
{ // searches the buffer and removes a specific job
    pthread_mutex_lock(&mutexBuffer);
    int found = 0;

    int current = b->start;
    int next = b->start;

    // search for the job in the buffer
    for (int jobs = 0; jobs < b->count; jobs++)
    {
        next = (current + 1) % b->bsize;
        if (strcmp(b->arr[current].jobID, job) == 0)
        {
            found = 1;
            break;
        }
        current = next;
    }

    if (found)
    { // if the job exists
        // move all the elements to fill the gap that the deleted job created
        int j = current;
        int next_j;
        for (int k = 0; k < b->count - 1; k++)
        {
            next_j = (j + 1) % b->bsize;
            b->arr[j] = b->arr[next_j];
            j = next_j;
        }

        // change the count and the end of the buffer
        b->end = (b->end - 1 + b->bsize) % b->bsize;
        b->count--;
        pthread_cond_signal(&conditionFullBuffer);
    }

    pthread_mutex_unlock(&mutexBuffer);
    return found; // returns 1 if the job was in the buffer and got deleted and 0 if not
}
