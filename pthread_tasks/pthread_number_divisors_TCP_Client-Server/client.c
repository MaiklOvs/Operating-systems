#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

//Задача:
// На заданном интервале чисел найти сумму всех
// делителей чисел. Написать программу без использования потоков и с использованием потоков
// Реализовать клиент - сервер. У клиента расчитываются интервалы и отправляются на сервер.
// На сервере производится расчет и возвращается конечная сумма делителей

typedef struct // Структура для интервала
{
    int start;
    int end;
} pthrData;

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("Неверное количество аргументов\nВведите IP-адрес и две границы интервала\n");
        return -1;
    }

    if (atoi(argv[2]) > atoi(argv[3]))
    {
        printf("Первый аргумент должен быть меньше второго!\n");
        return -1;
    }
    //структура для размещения адреса сервера
    struct sockaddr_in servaddr;
    int sockfd;
    int n;
    unsigned long countCPU = sysconf(_SC_NPROCESSORS_CONF); // Количество физических ядер
    long int q = atoi(argv[2]);
    long int p = atoi(argv[3]);


    //создаем TCP-сокет
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(1);
    }
    //перед заполнением обнуляем всю структуру
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //семейство
    servaddr.sin_port = htons(51000); //хост

    //преобразует адрес в нужную форму и сохраняет в поле структуры
    if (inet_aton(argv[1], &servaddr.sin_addr) == 0)
    {
        printf("Неверный IP-адрес\n");
        close(sockfd);
        exit(1);
    }
    //устанавливаем соединение через созданный сокет с сокетом сервера, адрес которого в структуре
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    pthrData* threadData = (pthrData*) malloc(countCPU * sizeof(pthrData)); // Диамическое выделение памяти под структуры данных
    int interval_t = p/countCPU;  // Рассчитываем интервал для потока
    if (interval_t <= countCPU)
    {
        threadData[0].start = q; // Для первого элемента массива структур задаем начало
        threadData[0].end = p; // и конец отдельно

    }
    else
    {
        interval_t = p / countCPU;
        threadData[0].start = q;
        threadData[0].end = interval_t;
        for (int i = 1; i < countCPU; i++) // Циклично заполняем структуры интервалами для потоков
        {
            threadData[i].start = threadData[i-1].end+1;
            if (i!=(countCPU-1))
            {
                threadData[i].end = threadData[i-1].end + interval_t;
            }
            else
            {
                threadData[i].end = p;          // На послденем интервале конец будет равен заданному числу вначале программы
            }

        }
    }

    for (size_t i = 0; i < countCPU; i++)
    {
        printf("%ld %d %d\n",i,threadData[i].start, threadData[i].end);
        if ((n = write(sockfd, &threadData[i], sizeof(threadData[i]))) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }
    }
    close(sockfd);
    free(threadData);
    return 0;
}
