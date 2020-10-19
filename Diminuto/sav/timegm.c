#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>

void main(void) {
    struct tm zulu = { 0, };
    struct tm juliet = { 0, };
    struct tm data = { 0, };
    struct tm * pointer = (struct tm *)0;
    time_t seconds = (time_t)0;

    printf("sizeof=%zu\n", sizeof(time_t));

    seconds = (time_t)0xffffffff80000000LL;
    printf("seconds=%ld=0x%08lx\n", (long)seconds, (long)seconds);

    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%d/%d/%dT%d:%d:%d+%d\n", zulu.tm_year + 1900, zulu.tm_mon, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    pointer = localtime_r(&seconds, &juliet);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%d/%d/%dT%d:%d:%d+%d\n", juliet.tm_year + 1900, juliet.tm_mon, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);

    seconds = (time_t)0xffffffffffffffffLL;
    printf("seconds=%ld=0x%08lx\n", (long)seconds, (long)seconds);

    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%d/%d/%dT%d:%d:%d+%d\n", zulu.tm_year + 1900, zulu.tm_mon, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    pointer = localtime_r(&seconds, &juliet);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%d/%d/%dT%d:%d:%d+%d\n", juliet.tm_year + 1900, juliet.tm_mon, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);

    seconds = (time_t)0x000000000000000LL;
    printf("seconds=%ld=0x%08lx\n", (long)seconds, (long)seconds);

    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%d/%d/%dT%d:%d:%d+%d\n", zulu.tm_year + 1900, zulu.tm_mon, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    pointer = localtime_r(&seconds, &juliet);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%d/%d/%dT%d:%d:%d+%d\n", juliet.tm_year + 1900, juliet.tm_mon, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);

    seconds = (time_t)0x000000007fffffffLL;
    printf("seconds=%ld=0x%08lx\n", (long)seconds, (long)seconds);

    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%d/%d/%dT%d:%d:%d+%d\n", zulu.tm_year + 1900, zulu.tm_mon, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    pointer = localtime_r(&seconds, &juliet);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%d/%d/%dT%d:%d:%d+%d\n", juliet.tm_year + 1900, juliet.tm_mon, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);

    data.tm_year = 1901 - 1900;
    data.tm_mon = 12 - 1;
    data.tm_mday = 13;
    data.tm_hour = 13;
    data.tm_min = 45;
    data.tm_sec = 52;
    data.tm_isdst = 0;
    printf("data=%d/%d/%dT%d:%d:%d+%d\n", data.tm_year + 1900, data.tm_mon, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    seconds = timegm(&data);
    if (seconds == (time_t)-1) { perror("timegm"); }
    printf("seconds=%ld=0x%08lx\n", seconds, seconds);

    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("zulu=%d/%d/%dT%d:%d:%d+%d\n", zulu.tm_year + 1900, zulu.tm_mon, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    seconds = timelocal(&data);
    if (seconds == (time_t)-1) { perror("timelocal"); }
    printf("seconds=%ld=0x%08lx\n", seconds, seconds);

    pointer = localtime_r(&seconds, &juliet);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%d/%d/%dT%d:%d:%d+%d\n", juliet.tm_year + 1900, juliet.tm_mon, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);
}
