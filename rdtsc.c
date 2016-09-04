#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
      "xorl %%eax, %%eax\n"
      "cpuid\n"
      "rdtsc\n"
      : "=a" (lo), "=d" (hi)
      :
      : "%ebx", "%ecx");
    return (uint64_t)hi << 32 | lo;
}

int main(int argc, char **argv)
{
    unsigned long long x;
    unsigned long long y;
    int i = 0;
    int iterations = 10000000;

    x = rdtsc();
    for (i; i < iterations; i++)
    { ;
    }
    y = rdtsc();
    long double time = (long double)(y-x) / 2600000000;
    printf("It took this time to do the for-loop: %Lf \n", time);
}



