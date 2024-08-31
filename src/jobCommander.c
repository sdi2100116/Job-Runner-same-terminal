
#include <stdio.h>
#include <sys/wait.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <ctype.h> 
#include <signal.h>
#include <string.h>

void perror_exit(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}


int main(int argc,char** argv){
    int sock;
    int port=atoi(argv[2]);
    char* serverName=strdup(argv[1]);
    char* inputCommand;
    int x;
    char* messageBuffer[50];
    int inp_size = 0;
    for (int i = 1; i < argc; i++)
    {
        inp_size += strlen(argv[i]) + 1; //count the size of the input message
    }
    inputCommand=malloc((sizeof(char)*inp_size)+1);
    if (inputCommand == NULL) perror_exit("malloc");
    inputCommand[0] = '\0';
    for (int i = 3; i < argc; i++) //put the input message in a seperate variable
    {
        strcat(inputCommand, argv[i]);
        if (i != argc - 1)
        {
            strcat(inputCommand, " ");
        }
    }

    // check if input is correct,if not exit with 1
    if (!strcmp(argv[3], "issueJob"))
    {
        if (argc < 5)
        {
            printf("Wrong Usage.\n");
            exit(1);
        }
    }
    else if (!strcmp(argv[3], "setConcurrency"))
    {
        if (argc != 5)
        {
            printf("Wrong Usage.\n");
            exit(1);
        }
    }
    else if (!strcmp(argv[3], "stop"))
    {
        if (argc != 5)
        {
            printf("Wrong Usage.\n");
            exit(1);
        }
    }
    else if (!strcmp(argv[3], "poll"))
    {
        if (argc != 4)
        {
            printf("Wrong Usage.\n");
            exit(1);
        }
    }
    else if (!strcmp(argv[3], "exit"))
    {
        if (argc != 4)
        {
            printf("Wrong Usage.\n");
            exit(1);
        }
    }
    else
    {
        printf("Unknown command.\n");
        exit(1);
    }
    int n=strlen(inputCommand);
    struct sockaddr_in server ;
    struct sockaddr* serverptr =(struct sockaddr*)&server;
    struct hostent* rem;
    
    //creates socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) perror_exit("socket");

    if ((rem = gethostbyname(argv[1])) == NULL) {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET; 
    memcpy(&server.sin_addr , rem->h_addr_list[0] , rem->h_length ) ;
    server.sin_port =htons(port) ; 
    //connects to the server with serverName and port
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) perror_exit("connect");
    printf("Connecting to server %s port %d...\n",serverName,port);
    if (write(sock, &n, sizeof(n)) < 0) perror_exit("write"); //first send the number of the message
    if(write(sock,inputCommand,strlen(inputCommand))<0) perror_exit("write"); //then send the message


    
    
    int number_of_bytes=50;
    while((x=read(sock,messageBuffer,number_of_bytes))){
        if (x == -1) {
            perror("read");
        } else {
            messageBuffer[x] = '\0';

            //write the message in the screen of the client
            if(write(1, messageBuffer, x)<0) perror_exit("write");
        }
    }
    
    
    free(inputCommand);
    free(serverName);
}
   
