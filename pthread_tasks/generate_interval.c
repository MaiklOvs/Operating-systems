#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#define filename "interval.txt"


typedef struct // Структура для интервала
{
    int start;
    int end;
} pthrData;

int main (int argc, char* argv[])
{
    FILE* fd;
    unsigned long countCPU = sysconf(_SC_NPROCESSORS_CONF); // Количество физических ядер
    int q = atoi(argv[1]); // Первое число
    int p = atoi(argv[2]); // Второе число
    fd = fopen(filename, "w+");
    pthrData* threadData = (pthrData*) malloc(countCPU * sizeof(pthrData)); // Диамическое выделение памяти под структуры данных
    int interval_t = p/countCPU;  // Рассчитываем интервал для потока
    if (interval_t <= countCPU)
    {
        threadData[0].start = q; // Для первого элемента массива структур задаем начало
        threadData[0].end = p; // и конец отдельно
        fprintf(fd, "%d %d\n", threadData[0].start, threadData[0].end);
    }
    else
    {
        interval_t = p / countCPU;
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
            fprintf(fd, "%d %d\n", threadData[i].start, threadData[i].end);
        }
    }
    free(threadData);
    fclose(fd);
}
