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
 * time handling stuff. I've seen some minor differences
 * between platforms - all distros based on Debian - mostly
 * because of the difference in sizeof(time_t): 32-bits on
 * some platforms, 64-bits on others. Another complication is
 * the fact that timegm(3) and timelocal(3) are documented as
 * returning (time_t)-1 as an error value; -1 is a valid epoch
 * value to return, since time_t is signed, and timegm(3),
 * timelocal(3), gmtime_r(3), and localtime_r(3) all use
 * negative time_t values to encode times before the epoch.
 * Also, as far as I can tell, sometimes the functions return
 * (time_t)-1 meaning error, but leave errno unset.
 */
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void main(void) {
    struct tm data = { 0, };
    struct tm * pointer = (struct tm *)0;
    time_t seconds = (time_t)0;
    time_t result = (time_t)0;
    int64_t datum = 0;
    static const int64_t MINIMUM    = 0xffffffff80000000LL;
    static const int64_t MAXIMUM    = 0x000000007fffffffLL;
    static const int64_t NEGATIVE   = 0xffffffffffffffffLL;
    static const int64_t ZERO       = 0x0000000000000000LL;
    static const int64_t SUPERMIN   = 0x8000000000000000LL;
    static const int64_t SUPERMAX   = 0x7FFFFFFFFFffffffLL;
    extern char *tzname[2];
    extern long timezone;
    extern int daylight;

    tzset();

    printf("%s[%d]:\n\n", __FILE__, __LINE__);

    printf("sizeof(time_t)=%zu\n", sizeof(time_t));

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    printf("tzname[0]=\"%s\"\n", tzname[0]);
    printf("tzname[1]=\"%s\"\n", tzname[1]);
    printf("timezone=%ld\n", timezone);
    printf("daylight=%02d\n", daylight);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    seconds = (time_t)MINIMUM;
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = localtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    seconds = (time_t)NEGATIVE;
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = localtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    seconds = (time_t)ZERO;
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = localtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    seconds = (time_t)MAXIMUM;
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = localtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    seconds = (time_t)SUPERMIN;
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = localtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    seconds = (time_t)SUPERMAX;
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    pointer = localtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("localtime_r"); }
    printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    data.tm_year = 1901 - 1900;
    data.tm_mon = 12 - 1;
    data.tm_mday = 13;
    data.tm_hour = 20;
    data.tm_min = 45;
    data.tm_sec = 51;
    data.tm_isdst = 0;
    printf("data=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    errno = 0;
    seconds = timegm(&data);
    if (seconds == (time_t)-1) { perror("timegm"); }
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    data.tm_year = 1901 - 1900;
    data.tm_mon = 12 - 1;
    data.tm_mday = 13;
    data.tm_hour = 20;
    data.tm_min = 45;
    data.tm_sec = 52;
    data.tm_isdst = 0;
    printf("data=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    errno = 0;
    seconds = timegm(&data);
    if (seconds == (time_t)-1) { perror("timegm"); }
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    data.tm_year = 2038 - 1900;
    data.tm_mon = 1 - 1;
    data.tm_mday = 19;
    data.tm_hour = 3;
    data.tm_min = 14;
    data.tm_sec = 7;
    data.tm_isdst = 0;
    printf("data=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    errno = 0;
    seconds = timegm(&data);
    if (seconds == (time_t)-1) { perror("timegm"); }
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    data.tm_year = 2038 - 1900;
    data.tm_mon = 1 - 1;
    data.tm_mday = 19;
    data.tm_hour = 3;
    data.tm_min = 14;
    data.tm_sec = 8;
    data.tm_isdst = 0;
    printf("data=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    errno = 0;
    seconds = timegm(&data);
    if (seconds == (time_t)-1) { perror("timegm"); }
    printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);

    errno = 0;
    memset(&data, ~0, sizeof(data));
    pointer = gmtime_r(&seconds, &data);
    if (pointer == (struct tm *)0) { perror("gmtime_r"); }
    printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    for (datum = MINIMUM; datum <= MAXIMUM; datum++) {

        seconds = datum;

        memset(&data, ~0, sizeof(data));
        pointer = gmtime_r(&seconds, &data);
        if (pointer == (struct tm *)0) { continue; }

        result = timegm(&data);
        if (result != seconds) { continue; }

        printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);
        printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);
        printf("result=%lld=0x%llx\n", (long long)result, (long long)result);
        break;

    }

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    for (datum = MAXIMUM; datum >= MINIMUM; datum--) {

        seconds = datum;

        memset(&data, ~0, sizeof(data));
        pointer = gmtime_r(&seconds, &data);
        if (pointer == (struct tm *)0) { continue; }

        result = timegm(&data);
        if (result != seconds) { continue; }

        printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);
        printf("zulu=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);
        printf("result=%lld=0x%llx\n", (long long)result, (long long)result);
        break;

    }

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    for (datum = MINIMUM; datum <= MAXIMUM; datum++) {

        seconds = datum;

        memset(&data, ~0, sizeof(data));
        pointer = localtime_r(&seconds, &data);
        if (pointer == (struct tm *)0) { continue; }

        result = timelocal(&data);
        if (result != seconds) { continue; }

        printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);
        printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);
        printf("result=%lld=0x%llx\n", (long long)result, (long long)result);
        break;

    }

    printf("\n%s[%d]:\n\n", __FILE__, __LINE__);

    for (datum = MAXIMUM; datum >= MINIMUM; datum--) {

        seconds = datum;

        memset(&data, ~0, sizeof(data));
        pointer = localtime_r(&seconds, &data);
        if (pointer == (struct tm *)0) { continue; }

        result = timelocal(&data);
        if (result != seconds) { continue; }

        printf("seconds=%lld=0x%llx\n", (long long)seconds, (long long)seconds);
        printf("juliet=%04d/%02d/%02dT%02d:%02d:%02d~%d\n", data.tm_year + 1900, data.tm_mon + 1, data.tm_mday, data.tm_hour, data.tm_min, data.tm_sec, data.tm_isdst);
        printf("result=%lld=0x%llx\n", (long long)result, (long long)result);
        break;

    }

}
