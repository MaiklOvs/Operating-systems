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
    int sockfd, newsockfd; // Дескрипторы для слушающего и присоединненого сокета
    int clilen; // Длина адреса клиента
    int n; // Количество принятых символов
    char line[1000]; // Буфер для приема информации
    struct sockaddr_in servaddr, cliaddr; // Структуры для размещения полных адресов сервера и клиента
    //Создаем ТСР - сокет
    if ((sockfd = socket(AF_INET, SOCK_STREAM,0)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    // Обнуляем всю структуру перед ее заполнением
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(51000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    clilen = sizeof(cliaddr);
    if ((newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen)) < 0 )
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    pid_t pid = fork();
    if (pid == 0)
    {
        for(;;)
        {
            printf("Message =>");
            fflush(stdin);
            memset(line,0,1000);
            fgets(line, 1000, stdin);
            while ((n = write(newsockfd,line,strlen(line)+1)) < 0)
            {
                perror("Write is falls");
                perror(NULL);
                close(sockfd);
                close(newsockfd);
                exit(1);
            }
        }
    }
    else
    {
        for (;;)
        {
            memset(line,0,1000);
            while ((n = read(newsockfd,line, strlen(line)-1)) > 0)
            {
                printf("\n Message is reserved: &s\t", line );
            }
            if (n<0)
            {
                perror(NULL);
                close(sockfd);
                close(newsockfd);
                exit(1);
            }
        }
    }
}
