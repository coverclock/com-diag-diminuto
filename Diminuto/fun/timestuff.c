/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * This program helps me reverse engineer the underlying
 * time handling stuff.
 */
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void main(void) {
    struct tm zulu = { 0, };
    struct tm juliet = { 0, };
    struct tm data = { 0, };
    struct tm * pointer = (struct tm *)0;
    time_t seconds = (time_t)0;
    time_t result = (time_t)0;
    int64_t datum = 0;
    static const int64_t MINIMUM    = 0xffffffff80000000LL;
    static const int64_t MAXIMUM    = 0x000000007fffffffLL;
    static const int64_t NEGATIVE   = 0xffffffffffffffffLL;
    static const int64_t ZERO       = 0x0000000000000000LL;
    extern char *tzname[2];
    extern long timezone;
    extern int daylight;

    tzset();

    printf("sizeof(time_t)=%zu\n", sizeof(time_t));

    putchar('\n');

    printf("tzname[0]=\"%s\"\n", tzname[0]);
    printf("tzname[1]=\"%s\"\n", tzname[1]);
    printf("timezone=%ld\n", timezone);
    printf("daylight=%02d\n", daylight);

    putchar('\n');

    seconds = (time_t)MINIMUM;
    printf("seconds=%lld=0x%8.8llx\n", (long long)seconds, (long long)seconds);

    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", zulu.tm_year + 1900, zulu.tm_mon + 1, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    pointer = localtime_r(&seconds, &juliet);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", juliet.tm_year + 1900, juliet.tm_mon + 1, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);

    putchar('\n');

    seconds = (time_t)NEGATIVE;
    printf("seconds=%lld=0x%8.8llx\n", (long long)seconds, (long long)seconds);

    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", zulu.tm_year + 1900, zulu.tm_mon + 1, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    pointer = localtime_r(&seconds, &juliet);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", juliet.tm_year + 1900, juliet.tm_mon + 1, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);

    putchar('\n');

    seconds = (time_t)ZERO;
    printf("seconds=%lld=0x%8.8llx\n", (long long)seconds, (long long)seconds);

    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", zulu.tm_year + 1900, zulu.tm_mon + 1, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    pointer = localtime_r(&seconds, &juliet);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", juliet.tm_year + 1900, juliet.tm_mon + 1, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);

    putchar('\n');

    seconds = (time_t)MAXIMUM;
    printf("seconds=%lld=0x%8.8llx\n", (long long)seconds, (long long)seconds);

    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", zulu.tm_year + 1900, zulu.tm_mon + 1, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    pointer = localtime_r(&seconds, &juliet);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", juliet.tm_year + 1900, juliet.tm_mon + 1, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);

    putchar('\n');

    data.tm_year = 1901 - 1900;
    data.tm_mon = 12 - 1;
    data.tm_mday = 13;
    data.tm_hour = 13;
    data.tm_min = 45;
    data.tm_sec = 0;
    data.tm_isdst = 0;
    printf("data=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);
    result = timegm(&data);
    if (result == (time_t)-1) { perror("timegm"); }
    printf("result=%lld=0x%8.8llx\n", (long long)result, (long long)result);
    pointer = gmtime_r(&seconds, &zulu);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", zulu.tm_year + 1900, zulu.tm_mon + 1, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);

    putchar('\n');

    for (datum = MINIMUM; datum <= MAXIMUM; datum++) {

        seconds = datum;

        pointer = gmtime_r(&seconds, &zulu);
        if (pointer == (struct tm *)0) { continue; }

        result = timegm(&zulu);
        if (result != seconds) { continue; }

        printf("seconds=%lld=0x%8.8llx\n", (long long)seconds, (long long)seconds);
        printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", zulu.tm_year + 1900, zulu.tm_mon + 1, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);
        printf("result=%lld=0x%8.8llx\n", (long long)result, (long long)result);
        break;

    }

    putchar('\n');

    for (datum = MAXIMUM; datum >= MINIMUM; datum--) {

        seconds = datum;

        pointer = gmtime_r(&seconds, &zulu);
        if (pointer == (struct tm *)0) { continue; }

        result = timegm(&zulu);
        if (result != seconds) { continue; }

        printf("seconds=%lld=0x%8.8llx\n", (long long)seconds, (long long)seconds);
        printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", zulu.tm_year + 1900, zulu.tm_mon + 1, zulu.tm_mday, zulu.tm_hour, zulu.tm_min, zulu.tm_sec, zulu.tm_isdst);
        printf("result=%lld=0x%8.8llx\n", (long long)result, (long long)result);
        break;

    }

    putchar('\n');

    for (datum = MINIMUM; datum <= MAXIMUM; datum++) {

        seconds = datum;

        pointer = localtime_r(&seconds, &juliet);
        if (pointer == (struct tm *)0) { continue; }

        result = timelocal(&juliet);
        if (result != seconds) { continue; }

        printf("seconds=%lld=0x%8.8llx\n", (long long)seconds, (long long)seconds);
        printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", juliet.tm_year + 1900, juliet.tm_mon + 1, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);
        printf("result=%lld=0x%8.8llx\n", (long long)result, (long long)result);
        break;

    }

    putchar('\n');

    for (datum = MAXIMUM; datum >= MINIMUM; datum--) {

        seconds = datum;

        pointer = localtime_r(&seconds, &juliet);
        if (pointer == (struct tm *)0) { continue; }

        result = timelocal(&juliet);
        if (result != seconds) { continue; }

        printf("seconds=%lld=0x%8.8llx\n", (long long)seconds, (long long)seconds);
        printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", juliet.tm_year + 1900, juliet.tm_mon + 1, juliet.tm_mday, juliet.tm_hour, juliet.tm_min, juliet.tm_sec, juliet.tm_isdst);
        printf("result=%lld=0x%8.8llx\n", (long long)result, (long long)result);
        break;

    }

}
