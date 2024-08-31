#include <pthread.h>
// #include "jobExecutorServer.h"

struct buffer
{
    struct triplet *arr;
    int start;
    int end;
    int count;
    int bsize;
};

void initializeBuffer(struct buffer *, int bsize);
void insertBuffer(struct buffer *, struct triplet);
struct triplet removeBuffer(struct buffer *);
int searchBuffer_and_remove(struct buffer *, char *);

extern pthread_mutex_t mutexBuffer;
extern pthread_cond_t conditionEmptyBuffer;
extern pthread_cond_t conditionFullBuffer;