#include <pthread.h>

struct triplet
{
    char jobID[20];
    char *job;
    int clientSocket;
};

void perror_exit(char *message);
void *controller_thread_act(void *clSocket);
void *worker_thread_act(void *controlerSocket);
char *string_rem_first(char *st);
int count_word_of_string(char *st);

extern int jobID;
extern int Concurrency;
extern int running_threads;
extern int server_running;
extern  int server_terminating;
extern pthread_t main_threadID;
extern struct buffer *buffer;
extern pthread_mutex_t mutexJobID;
extern pthread_mutex_t mutexConcurrency;
extern pthread_mutex_t mutexWorker;

extern pthread_cond_t conditionWorker;
