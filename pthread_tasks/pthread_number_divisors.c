#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

int atoi (const char *nprt); // Функция для приведения типов из  char*  в int
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
    for (int i = data->start; i <= data->end; i++)
    {
        for (int j = 1; j <= sqrt(i); j++)
        {
            if (!(i%j))
            {
                if (j!=i/j)
                {
                    pthread_mutex_lock(&mutex);
                    sum += 2;
                    pthread_mutex_unlock(&mutex);
                }
                else
                {
                    pthread_mutex_lock(&mutex);
                    sum++;
                    pthread_mutex_unlock(&mutex);
                }
            }
        }
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    int q = atoi(argv[1]); // Первое число
    int p = atoi(argv[2]); // Второе число
    int countCPU = sysconf(_SC_NPROCESSORS_CONF); // Количество физических ядер
    pthread_t* threads = (pthread_t*) malloc(countCPU * sizeof(pthread_t)); // Динамическое выделение памяти под идентификаторы потоков
    pthrData* threadData = (pthrData*) malloc(countCPU * sizeof(pthrData)); // Диамическое выделение памяти под структуры данных
    pthread_mutex_init(&mutex,NULL); // Мьютекс, для синхронизации потоков
    int interval_t = p / countCPU; // Рассчитываем интервал для потока
    threadData[0].start = q; // Для первого элемента массива структур задаем начало
    threadData[0].end = interval_t; // и конец отдельно
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
        pthread_create(&(threads[i]), NULL, sumDivisors, &threadData[i]);
    }

    for(int i = 0; i < countCPU; i++)
    {
        pthread_join(threads[i], NULL);  // Получаем результаты
    }
    pthread_mutex_destroy(&mutex); // Уничтожаем мьютекс
    free(threads); // Освобождаем память из-под массива идентификаторов
    free(threadData); // Освобождаем память из-под массива структур
    printf("Sum of divisors = %ld",sum);
    return 0;
}