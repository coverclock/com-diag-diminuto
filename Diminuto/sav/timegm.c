#include <stdio.h>
#include <time.h>
#include <errno.h>

void main(void) {
    time_t seconds;
    struct tm datetime;

    datetime.tm_year = 1901 - 1900;
    datetime.tm_mon = 12 - 1;
    datetime.tm_mday = 13;
    datetime.tm_hour = 13;
    datetime.tm_min = 45;
    datetime.tm_sec = 52;
    datetime.tm_isdst = 0;

    seconds = timegm(&datetime);
    printf("timegm=%ld=0x%lx\n", seconds, seconds);
    if (seconds == (time_t)-1) { perror("timegm"); }
}
