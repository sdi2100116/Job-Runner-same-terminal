#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>

#include "../include/jobExecutorServer.h"
#include "../include/buffer.h"

int jobID = 0;
int sock; // sock global so the handler can use it
int Concurrency = 1;
int running_threads = 0;
int server_running = 1;
int server_terminating = 0;
pthread_t main_threadID;
struct buffer *buffer;
pthread_mutex_t mutexJobID = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexConcurrency = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexWorker = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexBuffer = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t conditionWorker = PTHREAD_COND_INITIALIZER;

void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void sigusr1_handler(int signum)
{
    close(sock);
    server_running = 0;
}

int main(int argc, char **argv)
{
    int port = atoi(argv[1]);
    int bufferSize = atoi(argv[2]);
    int threadPoolSize = atoi(argv[3]);
    pthread_t tid;
    pthread_t worker_threads_ids[threadPoolSize];
    int worker_thread_args[threadPoolSize];
    // memory allocation for the global static buffer
    buffer = malloc(sizeof(struct triplet) * bufferSize);
    if (buffer == NULL)
        perror("malloc");
    // create sigaction to handle SIGUSR1
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa, NULL) == -1)
        perror("sigaction");

    main_threadID = pthread_self();
    int newsock;
    int n; // to get the number of characters that client sent
    struct sockaddr_in server, client;
    socklen_t clientlen=sizeof(struct sockaddr_in);
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;
    // mutex initialization
    if (pthread_mutex_init(&mutexJobID, NULL) != 0)
        perror("mutex");
    if (pthread_mutex_init(&mutexConcurrency, NULL) != 0)
        perror("mutex");
    if (pthread_mutex_init(&mutexWorker, NULL) != 0)
        perror("mutex");
    if (pthread_mutex_init(&mutexBuffer, NULL) != 0)
        perror("mutex");
    // creates the threadpool with the workerthreads
    for (int i = 0; i < threadPoolSize; ++i)
    {
        worker_thread_args[i] = i;
        pthread_create(&worker_threads_ids[i], NULL, worker_thread_act, &worker_thread_args[i]);
    }
    // initialize the buffer
    initializeBuffer(buffer, bufferSize);

    // creates socket
    int optval = 1;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        perror_exit("setsockopt");
    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");

    if (listen(sock, 5) < 0)
        perror_exit("listen");

    printf("Listening for connections to port %d\n", port);
   
    while (1)
    {
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0)
        { // accepts the client
            if (!server_running)
                break; // when the sock closes from the signal handler and the server_running changes to 0 so the server will stop
            else
                perror("accept");
        }
         int *client_socket = malloc(sizeof(int)); // client socket is an int pointer so every thread has its own
        if (client_socket == NULL)
        {
            perror_exit("malloc");
            close(newsock);
            continue;
        }
        *client_socket = newsock;
        if (pthread_create(&tid, NULL, controller_thread_act, client_socket) != 0)
        { // creates the thread for the client (controller_thread)
            perror("Failed to create thread");
            close(newsock);
            continue;
        }
        pthread_detach(tid);
    }
    pthread_mutex_destroy(&mutexJobID);
    pthread_mutex_destroy(&mutexWorker);
    pthread_mutex_destroy(&mutexBuffer);
    pthread_mutex_destroy(&mutexConcurrency);
}