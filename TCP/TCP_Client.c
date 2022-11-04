#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

void main (int argc, char **argv)
{
    int sockfd, anothersockfd;
    int n, i;
    char sendline[1000], recvline[1000];
    struct sockaddr_in servaddr;
    if (argc != 2) {
        printf("Usage: a.out <IP address>\n");
        exit(1);
    }
    memset(sendline, 0, 1000);
    memset(recvline, 0, 1000);
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror(NULL);
        exit(1);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(51000);
    if (inet_aton(argv[1], &servaddr.sin_addr) == 0) {
        printf("Invalid IP address\n");
        close(sockfd);
        exit(1);
    }
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0) { // Ребенок отправляет сообщение
        for (;;) {
            printf("Message => ");
            fflush(stdin);
            memset(sendline, 0, 1000);
            fgets(sendline, 1000, stdin);
            if ((n = write(sockfd, sendline, strlen(sendline) + 1)) < 0) {
                perror("Write is fall\n");
                close(sockfd);
                exit(1);
            }
        }

    } else { // Родитель принимает сообщение
        for (;;) {
            memset(recvline, 0, 1000);
            while ((n = read(sockfd, recvline, strlen(sendline)-1)) > 0) {
                printf("\nMessage is reserved:%s\n", recvline);
            }
            if (n < 0) {
                perror("Read is fall\n");
                close(sockfd);
                exit(1);
            }
            printf("%s", recvline);
        }
    }
}
