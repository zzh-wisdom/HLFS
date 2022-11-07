#include <time.h>
#include <stdio.h>

int main() {
    struct timespec time_res;
    struct timespec time;

    clock_getres(CLOCK_REALTIME, &time_res);
    printf("CLOCK_REALTIME res             %lds, %ldns\n", time_res.tv_sec, time_res.tv_nsec);
    clock_getres(CLOCK_MONOTONIC, &time_res);
    printf("CLOCK_MONOTONIC res            %lds, %ldns\n", time_res.tv_sec, time_res.tv_nsec);
    clock_getres(CLOCK_MONOTONIC_RAW, &time_res);
    printf("CLOCK_MONOTONIC_RAW res        %lds, %ldns\n", time_res.tv_sec, time_res.tv_nsec);
    clock_getres(CLOCK_BOOTTIME, &time_res);
    printf("CLOCK_BOOTTIME res             %lds, %ldns\n", time_res.tv_sec, time_res.tv_nsec);
    clock_getres(CLOCK_PROCESS_CPUTIME_ID, &time_res);
    printf("CLOCK_PROCESS_CPUTIME_ID res   %lds, %ldns\n", time_res.tv_sec, time_res.tv_nsec);
    clock_getres(CLOCK_THREAD_CPUTIME_ID, &time_res);
    printf("CLOCK_THREAD_CPUTIME_ID res    %lds, %ldns\n", time_res.tv_sec, time_res.tv_nsec);

    clock_gettime(CLOCK_REALTIME, &time);
    printf("CLOCK_REALTIME res             %lds, %ldns\n", time.tv_sec, time.tv_nsec);
    clock_gettime(CLOCK_MONOTONIC, &time);
    printf("CLOCK_MONOTONIC res            %lds, %ldns\n", time.tv_sec, time.tv_nsec);
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    printf("CLOCK_MONOTONIC_RAW res        %lds, %ldns\n", time.tv_sec, time.tv_nsec);
    clock_gettime(CLOCK_BOOTTIME, &time);
    printf("CLOCK_BOOTTIME res             %lds, %ldns\n", time.tv_sec, time.tv_nsec);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
    printf("CLOCK_PROCESS_CPUTIME_ID res   %lds, %ldns\n", time.tv_sec, time.tv_nsec);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
    printf("CLOCK_THREAD_CPUTIME_ID res    %lds, %ldns\n", time.tv_sec, time_res.tv_nsec);

    return 0;
}
