#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>


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
    int q = atoi(argv[1]); // Первое число
    int p = atoi(argv[2]); // Второе число
    if ( q > p )
    {
    printf("Sum of divisors = %ld",sum);
    return 0;
    }
    unsigned long countCPU = sysconf(_SC_NPROCESSORS_CONF); // Количество физических ядер
    pthread_t* threads = (pthread_t*) malloc(countCPU * sizeof(pthread_t)); // Динамическое выделение памяти под идентификаторы потоков
    pthrData* threadData = (pthrData*) malloc(countCPU * sizeof(pthrData)); // Диамическое выделение памяти под структуры данных
    pthread_mutex_init(&mutex,NULL); // Мьютекс, для синхронизации потоков
    int interval_t = p/countCPU;  // Рассчитываем интервал для потока
    if (interval_t <= countCPU)
    {
    threadData[0].start = q; // Для первого элемента массива структур задаем начало
    threadData[0].end = p; // и конец отдельно
    printf("%d %d %d\n", 0, threadData[0].start, threadData[0].end);
    pthread_create(&(threads[0]), NULL, sumDivisors, &threadData[0]); // Создаем поток
    pthread_join(threads[0], NULL);  // Получаем результаты
    }
    else
   {
     interval_t = p / countCPU;
     pthread_create(&(threads[0]), NULL, sumDivisors, &threadData[0]); // Создаем поток
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
        printf("%d %d %d\n", i, threadData[i].start, threadData[i].end);
        pthread_create(&(threads[i]), NULL, sumDivisors, &threadData[i]);
    }
    for(int i = 0; i < countCPU; i++)
    {
        pthread_join(threads[i], NULL);  // Получаем результаты
    }
    }

    pthread_mutex_destroy(&mutex); // Уничтожаем мьютекс
    free(threads); // Освобождаем память из-под массива идентификаторов
    free(threadData); // Освобождаем память из-под массива структур
    printf("Sum of divisors = %ld\n",sum);
    return 0;
}
