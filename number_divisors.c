﻿#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int sumDivisiors(int start, int end)
{
    unsigned long sumDivisiors = 0;

    for (int i = start; i <= end; i++)
    {
        unsigned long countDivisiors = 0;
        for (int j = 1; j <= sqrt(i); j++)
        {
            if (!(i%j))
            {
                if (j!=i/j)
                {
                    countDivisiors += 2;
                }
                else
                {
                    countDivisiors++;
                }
            }
        }

        sumDivisiors += countDivisiors;
    }
    return sumDivisiors;
}

int main(int argc, char* argv[])
{
    printf("%d", sumDivisiors(1, 1000000));

}

