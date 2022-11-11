#include <pthread.h>
#include <math.h>
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

unsigned long sum = 0; // Глобальная переменная для подсчета суммы всех делителей
pthread_mutex_t mutex; // Мьютекс

typedef struct // Структура для интервала
{
    int start;
    int end;
} pthrData;

void *sumDivisors(void* thread_data) // Потоковая функция
{
    pthrData *data = (pthrData*) thread_data;
    unsigned long sumOfDivisiors = 0;
    for (int i = data->start; i <= data->end; i++)
    {
        for (int j = 1; j <= sqrt(i); j++)
        {
            if (!(i%j))
            {
                if (j!=i/j)
                {
                    sumOfDivisiors += 2;
                }
                else
                {
                    sumOfDivisiors++;
                }
            }
        }
    }
    pthread_mutex_lock(&mutex);
    sum += sumOfDivisiors;
    pthread_mutex_unlock(&mutex);
    return NULL;
}


int main(int argc, char *argv[]) {

    //дескрипторы для пассивного(слушающего) и присоединенного сокетов
    int sockfd, newsockfd;
    int clilen, n;
    unsigned long countCPU = sysconf(_SC_NPROCESSORS_CONF); // Количество физических ядер
    //структуры для размещения полных адресов сервера и клиента
    struct sockaddr_in servaddr, cliaddr;
    //создаем TCP-сокет
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    //обнуляем всю структуру перед заполнением
    memset(&servaddr, 0, sizeof(servaddr));
    //заполняем структуру для адреса сервера
    servaddr.sin_family = AF_INET; //семейство
    servaddr.sin_port = htons(51000);//номер порта
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //связываем созданный сокет с адресом (3 параметр - размер структуры в байтах)
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    //переводим сокет в пассивное состояние
    // второй параметр - кол-во одновременных подключений
    if (listen(sockfd, 5) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    // заносим в clilen максимальную длину ожидаемого адреса клиента
    clilen = sizeof(cliaddr);
    // принимаем соединение на сокете
    // при нормальном завершении в структуре будет адрес, а в clilen - фактическая длина
    if ((newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
    {
        perror(NULL);
        close(sockfd);
        exit(1);
    }

    pthread_t* threads = (pthread_t*) malloc(countCPU * sizeof(pthread_t)); // Динамическое выделение памяти под идентификаторы потоков
    pthrData* threadData = (pthrData*) malloc(countCPU * sizeof(pthrData)); // Диамическое выделение памяти под структуры данных
    pthread_mutex_init(&mutex,NULL); // Мьютекс, для синхронизации потоков

    //считываем интервалы из сокета и создаем потоки
    for(int i=0; i < countCPU; i++)
    {

        n=read(newsockfd, &threadData[i],sizeof(threadData[i]));
        //если при чтении возникла ошибка - завершаем работу
        if (n < 0)
        {
            perror(NULL);
            close(sockfd);
            close(newsockfd);
            exit(1);
        }
        pthread_create(&threads[i],NULL,sumDivisors,&threadData[i]);
    }

    for(int i=0; i < countCPU ; ++i)
        pthread_join(threads[i],NULL);

    pthread_mutex_destroy(&mutex);
    printf("Sum of divisors = %ld\n",sum);

    //освобождаем выделенную память
    free(threads);
    free(threadData);
    close(sockfd);
    close(newsockfd);
    return 0;
}
