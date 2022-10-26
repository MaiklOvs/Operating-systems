﻿#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define P 1
#define Q 1000000

unsigned long sum = 0;
pthread_mutex_t mutex;
struct interval
{
    int start;
    int end;
};


void *sumDivisiors(void* interv)
{
    struct interval* data = (struct interval*) interv;
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
    struct interval intrvl;
    struct interval intrvl_2;
    intrvl.start = P;
    intrvl.end = Q/2;
    intrvl_2.start = (Q / 2)+1;
    intrvl_2.end = Q;
    pthread_t tid1,tid2;
    pthread_mutex_init(&mutex,NULL);
    pthread_create(&tid1,NULL,sumDivisiors,&intrvl);
    pthread_create(&tid2,NULL,sumDivisiors,&intrvl_2);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_mutex_destroy(&mutex);
    printf("%ld",sum);
    return 0;
}
