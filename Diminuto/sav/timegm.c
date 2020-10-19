#include <stdio.h>
#include <time.h>
#include <errno.h>

void main(void) {
    struct tm earlier = { 0, };
    time_t seconds = 0;
    struct tm later = { 0, };;
    struct tm * pointer = (struct tm *)0;

    printf("sizeof=%zu\n", sizeof(time_t));

    earlier.tm_year = 1901 - 1900;
    earlier.tm_mon = 12 - 1;
    earlier.tm_mday = 13;
    earlier.tm_hour = 13;
    earlier.tm_min = 45;
    earlier.tm_sec = 52;
    earlier.tm_isdst = 0;
    printf("earlier=%d/%d/%dT%d:%d:%d+%d\n", earlier.tm_year, earlier.tm_mon, earlier.tm_mday, earlier.tm_hour, earlier.tm_min, earlier.tm_sec, earlier.tm_isdst);

    seconds = timegm(&earlier);
    if (seconds == (time_t)-1) { perror("timegm"); }
    printf("seconds=%ld=0x%lx\n", seconds, seconds);

    pointer = localtime_r(&seconds, &later);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("later=%d/%d/%dT%d:%d:%d+%d\n", later.tm_year, later.tm_mon, later.tm_mday, later.tm_hour, later.tm_min, later.tm_sec, later.tm_isdst);
}
