#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#define MAX 80
#define SA struct sockaddr

char *version = "0.2";

void func(int sockfd)
{
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n');
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main(int argc, char **argv, char* env[])
{
    int sockfd, connfd;
    struct sockaddr_in servaddr;
    in_addr_t addr = inet_addr("127.0.0.1");
    int serv_port = 8080;
    time_t start_time = time(NULL), current_time;

    if (getenv("L2ADDR"))
        addr = inet_addr(getenv("L2ADDR"));

    if (getenv("L2PORT"))
        serv_port = strtol(getenv("L2PORT"), NULL, 10);

    //getopt, old friend
    int c;
    int digit_optind = 0;
    while ((c = getopt(argc, argv, "w:dl:a:p:vh")) != -1) {
        int this_option_optind = optind ? optind : 1;
        switch (c) {
            case 'a':
                addr = inet_addr(optarg);
                break;
            case 'p':
                serv_port = strtol(optarg, NULL, 10);
                break;
            case 'v':
                printf("%s \n", version);
                break;
            case 'h':
                printf("\n \t This is help message:\n"
                       "This is simple TCP client, it has several startup options \n"
                       "PUSH [number] pushes the number to stack on server \n"
                       "POP returns the number from the top of the stack \n"
                       "user is required to provide one of those options \n \n"
                       "-a specifies server address. The default is 127.0.0.1 \n"
                       "-p specifies server port. The default is 8080 \n"
                       "-v outputs program version \n"
                       "-h prints this message \n");
                break;
            case '?':
                break;
            default:
                continue;
        }
    }

    char buff[MAX];
    for(int i = 0; i < argc; i++){
        if (strncmp(argv[i], "PUSH", 4) == 0){
            strcpy(buff, "PUSH ");
            if (i+1 > argc) printf("Wrong request! Use -h to see available requests \n");
            strcpy(buff+5, argv[i+1]);
            break;
        }
        if (strncmp(argv[i], "POP", 3) == 0){
            strcpy(buff, "POP");
            break;
        }
    }
    if (buff[0] != 'P'){
        printf("Wrong request! Use -h to see available requests \n");
    }

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = addr;
    servaddr.sin_port = htons(serv_port);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
    printf("To Server: %s \n", buff);
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));

    read(sockfd, buff, sizeof(buff));
    printf("From Server: %s \n", buff);

    double wait = difftime(time(NULL), start_time);
    printf("Delay: %f \n", wait);

    // close the socket
    close(sockfd);
}
