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
#include <fcntl.h>
#include "../include/jobExecutorServer.h"
#include "../include/buffer.h"

// removes the first word of the string
char *string_rem_first(char *st)
{
    char *first_space = strchr(st, ' ');
    char *remaining_string = first_space + 1;

    return strdup(remaining_string);
}

// counts how many words the string has
int count_word_of_string(char *st)
{
    char *dupl = strdup(st);
    int count = 0;
    char *word;

    word = strtok(dupl, " ");
    while (word != NULL)
    {
        count++;
        word = strtok(NULL, " ");
    }
    free(dupl);
    return count;
}

void *controller_thread_act(void *clSocket)
{
    printf("Accepted Connection\n");
    int n;
    int clientSocket = *((int *)clSocket);
    char *command;
    char *input; // input from the client, we get it trhough the socket
    char *input_wo_firstword;
    char *unchanged;
    char *return_message;
    if (read(clientSocket, &n, sizeof(n)) < 0)
        perror_exit("read"); // but first we get size of the input
    input = malloc((sizeof(char) * n + 1));
    if (read(clientSocket, input, n) < 0)
        perror_exit("read");
    input[n] = '\0';
    unchanged = strdup(input); // unchanged gets the input so we can use it without it being tokenized
    command = strtok(input, " ");
    if (!strcmp(command, "issueJob"))
    {
        input_wo_firstword = string_rem_first(unchanged); // removed the first word of the input (issueJob)
        pthread_mutex_lock(&mutexJobID);
        jobID++;
        pthread_mutex_unlock(&mutexJobID);
        // creates the triplet that puts into the buffer
        struct triplet tr;
        tr.clientSocket = clientSocket;
        tr.job = strdup(input_wo_firstword);
        sprintf(tr.jobID, "job_%d", jobID);
        insertBuffer(buffer, tr);
        // if concurrency accepts it the worker thread unblocks so the job in the buffer will run
        if (running_threads < Concurrency)
        {
            pthread_cond_signal(&conditionWorker);
        }
        // creates the return message and weites it into the buffer
        return_message = malloc(sizeof(char) * (20 + strlen(tr.jobID) + strlen(tr.job)));
        if (return_message == NULL)
            perror_exit("malloc");
        sprintf(return_message, "JOB <%s,%s> SUBMITTED\n", tr.jobID, tr.job);
        if (write(clientSocket, return_message, strlen(return_message)) < 0)
            perror_exit("write");
        free(input_wo_firstword);
        free(return_message);
        if (!Concurrency)
        { // if concurrency is 0 no job can run so the client have to wait until the concurrency changes to something bigger than 0
            char str[] = "Your job can't run right now.Please wait...\n";
            write(clientSocket, str, strlen(str));
        }
    }
    else if (!strcmp(command, "setConcurrency"))
    {
        char *newCon_string = strtok(NULL, " "); // takes the new concurrency which is a string
        int newCon = atoi(newCon_string);        // makes it an integer
        pthread_mutex_lock(&mutexConcurrency);
        // if the new concurrency is bigger than the old one new worker threads have to unblock
        if (newCon > Concurrency)
        {
            for (int i = 0; i < newCon - Concurrency; i++)
            {
                if (buffer->count == 0)
                    break;
                pthread_cond_signal(&conditionWorker);
            }
        }
        // concurrency changes and a message is returned to the client
        Concurrency = newCon;
        pthread_mutex_unlock(&mutexConcurrency);
        return_message = malloc(sizeof(char) * (20 + strlen(newCon_string)));
        if (return_message == NULL)
            perror_exit("malloc");
        sprintf(return_message, "CONCURRENCY SET AT %d\n", newCon);
        if (write(clientSocket, return_message, strlen(return_message)) < 0)
            perror_exit("write");
        free(return_message);
        close(clientSocket);
    }
    else if (!strcmp(command, "stop"))
    {
        char *job = strtok(NULL, " ");                     // takes the job_id
        int exists = searchBuffer_and_remove(buffer, job); // returns 1 if exists and 0 if not
        // return message is created and goes to the client,if job exists is removed in the function
        char *return_message = malloc(sizeof(char) * (15 + strlen(job)));
        if (exists)
        {
            sprintf(return_message, "JOB %s REMOVED\n", job);
        }
        else
        {
            sprintf(return_message, "JOB %s NOTFOUND\n", job);
        }
        if (write(clientSocket, return_message, strlen(return_message)) < 0)
            perror_exit("write");
        free(return_message);
        close(clientSocket);
    }
    else if (!strcmp(command, "poll"))
    { // for every job in the buffer return the triplet and send it to the client
        pthread_mutex_lock(&mutexBuffer);
        int jobs = 0;
        int i = buffer->start;
        while (jobs < buffer->count)
        {

            char *couple = malloc(sizeof(char) * (strlen(buffer->arr[i].job) + strlen(buffer->arr[i].jobID) + 4));
            sprintf(couple, "<%s,%s>\n", buffer->arr[i].jobID, buffer->arr[i].job);

            if (write(clientSocket, couple, strlen(couple)) < 0)
                perror_exit("write");
            free(couple);
            jobs++;
            i = (i + 1) % buffer->bsize;
        }
        pthread_mutex_unlock(&mutexBuffer);

        close(clientSocket);
    }
    else if (!strcmp(command, "exit"))
    {
        char *return_message = "SERVER TERMINATED\n";
        // for every job in the buffer return to the client that send it SERVER TERMINATED BEFORE EXECUTION and remove it
        char *jobsInBuffer_message = "SERVER TERMINATED BEFORE EXECUTION\n";
        server_terminating = 1;
        pthread_cond_broadcast(&conditionFullBuffer); //wakes up all the jobs that are blocked and waiting for a space in the buffer 
        while (buffer->count > 0)
        {   
            struct triplet tr = removeBuffer(buffer);
            free(tr.job);
            write(tr.clientSocket, jobsInBuffer_message, strlen(jobsInBuffer_message));
            close(tr.clientSocket);
        }
        pthread_mutex_lock(&mutexWorker);
        while (running_threads != 0); // wait for all the jobs that are running to finish
        pthread_mutex_unlock(&mutexWorker);
        if (write(clientSocket, return_message, strlen(return_message)) < 0)
            perror_exit("write"); // send message to the client SERVER TERMINATED
        if (pthread_kill(main_threadID, SIGUSR1) != 0)
            perror_exit("pthread_kill"); // send SIGUSR1 to the server
        close(clientSocket);
    }
    free(unchanged);
    free(input);
}

void *worker_thread_act(void *workSocket)
{
    int worker_thread_socket = *(int *)workSocket;
    while (1)
    {
        pthread_mutex_lock(&mutexWorker);
        pthread_cond_wait(&conditionWorker, &mutexWorker); // worker blocks here
        pthread_mutex_unlock(&mutexWorker);
        struct triplet tr = removeBuffer(buffer);

        running_threads++;

        pthread_cond_signal(&conditionFullBuffer); // a job is removed so it sends signal to the buffer to unblock if it was full
        int pid;
        if ((pid = fork()) < 0)
            perror_exit("fork");
        if (pid == 0)
        { // child process
            // file creation pid.output
            int cpid = getpid();
            int cpid_size = snprintf(NULL, 0, "%d", cpid);
            int file;
            char *filename;
            filename = malloc(sizeof(char) * (cpid_size + 8));
            if (filename == NULL)
                perror_exit("malloc");
            sprintf(filename, "%d.output", cpid);
            if ((file = creat(filename, 0644)) == -1)
                perror_exit("file creation");
            dup2(file, 1);
            close(file);
            // create the vector-array for the execvp
            char **exec_args;
            char *job = strdup(tr.job);
            free(tr.job);
            int jobWordNum = count_word_of_string(job);
            exec_args = malloc((jobWordNum + 1) * sizeof(char *));
            exec_args[0] = strdup(strtok(job, " "));
            for (int i = 1; i < jobWordNum; i++)
            {
                exec_args[i] = strdup(strtok(NULL, " "));
            }
            exec_args[jobWordNum] = NULL;
            free(job);
            execvp(exec_args[0], exec_args);
        }
        else
        { // if parent
            int status;
            if (waitpid(pid, &status, 0) == -1)
                perror("wait"); // waits for the child to finish
            int file;
            char message_buffer[50];
            int readBytes;
            int writteBytes;
            int pid_size = snprintf(NULL, 0, "%d", pid); // gets the pid size so it will allocate correct number of memory
            char *filename = malloc(sizeof(char) * (pid_size + 8));
            sprintf(filename, "%d.output", pid);
            if ((file = open(filename, O_RDWR)) == -1)
                perror_exit("open file");
            // sends to client the job inside the job output start-end while it reads it from the pid.output file
            char start[46];
            char end[46];
            sprintf(start, "-----%s output start------\n\n", tr.jobID);
            sprintf(end, "\n\n-----%s output end------\n", tr.jobID);
            if (write(tr.clientSocket, start, 46) == -1)
                perror_exit("write");
            while ((readBytes = read(file, message_buffer, 50)) > 0)
            {
                if ((writteBytes = write(tr.clientSocket, message_buffer, readBytes)) == -1)
                    perror_exit("write");
            }
            if (write(tr.clientSocket, end, 46) == -1)
                perror_exit("write");
            // removes file and closes socket
            if (remove(filename) == -1)
                perror_exit("remove");
            free(filename);
            close(tr.clientSocket);
        }

        running_threads--;

        // if buffer is not empty and if working threads number that running is smaller than concurrency unblock the next worker thread to run the next job
        pthread_mutex_lock(&mutexBuffer);
        if (buffer->count != 0 && running_threads < Concurrency)
        {
            pthread_mutex_lock(&mutexWorker);
            pthread_cond_signal(&conditionWorker);
            pthread_mutex_unlock(&mutexWorker);
        }
        pthread_mutex_unlock(&mutexBuffer);
    }
}
