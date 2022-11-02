#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

#define filename "interval.txt"

//Задача:
// На заданном интервале чисел найти сумму всех
// делителей чисел. Написать программу без использования потоков и с использованием потоков

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

int main(int argc, char* argv[])
{
    FILE* fd;
    fd = fopen(filename, "r");
    int countLines = 0;
    rewind(fd);
    while (!feof(fd))
    {
        if (fgetc(fd) == '\n')
        {
            ++countLines;
        }

    }
    pthread_t* threads = (pthread_t*) malloc(countLines * sizeof(pthread_t)); // Динамическое выделение памяти под идентификаторы потоков
    pthrData* threadData = (pthrData*) malloc(countLines * sizeof(pthrData)); // Диамическое выделение памяти под структуры данных
    pthread_mutex_init(&mutex,NULL); // Мьютекс, для синхронизации потоков
    rewind(fd);
    for (size_t i = 0; i < countLines; ++i)
    {
        fscanf(fd,"%d %d", &threadData[i].start, &threadData[i].end );
        pthread_create(&threads[i], NULL, sumDivisors, &threadData[i]);
    }
    for (int i = 0; i < countLines; ++i)
    {
        printf("%d %d %d\n",i,threadData[i].start, threadData[i].end);
        pthread_join(threads[i], NULL);  // Получаем результаты
    }
    pthread_mutex_destroy(&mutex); // Уничтожаем мьютекс
    free(threads); // Освобождаем память из-под массива идентификаторов
    free(threadData); // Освобождаем память из-под массива структур
    fclose(fd);
    printf("Sum of divisors = %ld\n",sum);
    return 0;
}
