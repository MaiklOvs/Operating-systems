#include <stdio.h>
#include <stdlib.h>
#include <math.h>


//Задача:
// На заданном интервале чисел найти сумму всех
// делителей чисел. Написать программу без использования потоков и с использованием потоков 

int atoi(const char* nprt);
int sumDivisors(int start, int end)
{
    unsigned long sumDivisors = 0;

    for (int i = start; i <= end; i++)
    {
        unsigned long countDivisors = 0;
        for (int j = 1; j <= sqrt(i); j++)
        {
            if (!(i%j))
            {
                if (j!=i/j)
                {
                    countDivisors += 2;
                }
                else
                {
                    countDivisors++;
                }
            }
        }

        sumDivisors += countDivisors;
    }
    return sumDivisors;
}

int main(int argc, char* argv[])
{
    int q = atoi(argv[1]);
    int p = atoi(argv[2]);
    printf("%d", sumDivisors(q, p));

}
