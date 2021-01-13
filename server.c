#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/file.h>
#include <limits.h>
#include <sys/stat.h>
#define LOCK_FILE "daemond.lock"
#define MAX 80
#define SA struct sockaddr

struct stack{
    long double number;
    struct stack* next;
};

struct dict{                //this should be a hash table
    char* ip;
    struct stack* stack;
};

char *version = "0.1";
static int wait_time = 0;
// Function designed for chat between client and server.
void func(int sockfd)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);

        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n');

        // and send that buffer to client
        write(sockfd, buff, sizeof(buff));

        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}


// Driver function
int main(int argc, char **argv, char* env[])
{
    int sockfd, connfd, communism = 0;
    in_addr_t addr = INADDR_ANY;
    int serv_port = 8080;
    unsigned int len;
    struct sockaddr_in servaddr, cli;
    char buffer[MAX];
    char logname[PATH_MAX];

    //checking the env
    if (getenv("L2WAIT"))
        wait_time = strtol(getenv("L2WAIT"), NULL, 10);

    if (getenv("L2LOGFILE"))
        strcpy(logname, getenv("L2LOGFILE"));

    if (getenv("L2ADDR"))
        addr = inet_addr(getenv("L2ADDR"));

    if (getenv("L2PORT"))
        serv_port = strtol(getenv("L2PORT"), NULL, 10);

    //getopt, old friend
    int c;
    int digit_optind = 0;
    while ((c = getopt(argc, argv, "w:dl:a:p:vhc")) != -1) {
        int this_option_optind = optind ? optind : 1;
        switch (c) {
            case 'l':
                strcpy(logname, optarg);
                break;
            case 'w':
                wait_time = strtol(optarg, NULL, 10);
                break;
            case 'd':
                daemon(0, 0);
                break;
            case 'a':
                addr = inet_addr(optarg);
                break;
            case 'p':
                serv_port = strtol(optarg, NULL, 10);
                break;
            case 'v':
                printf("%s \n", version);
                break;
            case 'c':
                communism = 1;
                break;
            case 'h':
                printf("This is help message placeholder\n");
                break;
            case '?':
                break;
            default:
                continue;
        }
    }

    FILE* log = fopen(logname, "w");

    // socket create and verification

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(log, "socket creation failed...\n");
        exit(0);
    }
    else
        fprintf(log, "Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(addr);
    servaddr.sin_port = htons(serv_port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        fprintf(log, "socket bind failed...\n");
        exit(0);
    }
    else
        fprintf(log, "Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        fprintf(log, "Listen failed...\n");
        exit(0);
    }
    else
        fprintf(log, "Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        fprintf(log, "server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");

    // Function for chatting between client and server
    func(connfd);

    // After chatting close the socket
    close(sockfd);
}
