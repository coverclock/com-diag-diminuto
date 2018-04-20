/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_escape.h"
#include <string.h>
#include <ctype.h>

size_t diminuto_escape_collapse(char * to, const char * from, size_t tsize)
{
    const char * ff;
    char * tt;
    char byte;
    int ii;

    ff = from;
    tt = to;

    while ((1 < tsize) && ('\0' != *ff)) {
        if ('\\' == *ff) {
            switch (*(ff + 1)) {
            case '\0':                  /* Terminating NUL. */
                ++ff;
                break;
            case '\n':                  /* Escaped newline; skip over. */
                ff += 2;
                break;
            case '\r':                  /* Escaped return; skip over. */
                ff += 2;
                break;
            case '\\':                  /* Escaped backslash. */
                ff += 2;
                *(tt++) = '\\';
                --tsize;
                break;
            case 'a':                   /* Alarm a.k.a. BEL. */
                ff += 2;
                *(tt++) = '\a';
                --tsize;
                break;
            case 'b':                   /* Backspace a.k.a. BS. */
                ff += 2;
                *(tt++) = '\b';
                --tsize;
                break;
            case 'f':                   /* Formfeed a.k.a. FF. */
                ff += 2;
                *(tt++) = '\f';
                --tsize;
                break;
            case 'n':                   /* Newline a.k.a. NL or LF. */
                ff += 2;
                *(tt++) = '\n';
                --tsize;
                break;
            case 'r':                   /* Return a.k.a. CR. */
                ff += 2;
                *(tt++) = '\r';
                --tsize;
                break;
            case 't':                   /* Horizontal tab a.k.a. HT. */
                ff += 2;
                *(tt++) = '\t';
                --tsize;
                break;
            case 'v':                   /* Vertical tab a.k.a. VT. */
                ff += 2;
                *(tt++) = '\v';
                --tsize;
                break;
            case 'x':                   /* Hexadecimal sequence. */
                byte = 0;
                /* ANSI hexadecimal sequences 1 or 2 hexadecimal digits long. */
                for (ii = 2; ; ++ii) {
                    if (('0' <= *(ff + ii)) && (*(ff + ii) <= '9')) {
                        byte <<= 4;
                        byte |= '\x0' + *(ff + ii) - '0';
                    } else if (('a' <= *(ff + ii)) && (*(ff + ii) <= 'f')) {
                        byte <<= 4;
                        byte |= '\xa' + *(ff + ii) - 'a';
                    } else if (('A' <= *(ff + ii)) && (*(ff + ii) <= 'F')) {
                        byte <<= 4;
                        byte |= '\xA' + *(ff + ii) - 'A';
                    } else {
                        break;
                    }
                }
                if (2 < ii) {
                    ff += ii;           /* Hexadecimal sequence. */
                    *(tt++) = byte;
                } else {
                    *(tt++) = *(ff++);  /* Invalid hexadecimal sequence. */
                }
                --tsize;
                break;
            case '0':                   /* Octal sequence. */
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                byte = 0;
                /* ANSI octal sequences may be 1, 2 or 3 octal digits long. */
                for (ii = 1; 4 > ii; ++ii) {
                    if (('0' <= *(ff + ii)) && (*(ff + ii) <= '7')) {
                        byte <<= 3;
                        byte |= '\0' + *(ff + ii) - '0';
                    } else {
                        break;
                    }
                }
                ff += ii;           /* Octal sequence. */
                *(tt++) = byte;
                --tsize;
                break;
            default:
                ff += 2;
                *(tt++) = *(ff - 1);  /* Unknown escape sequence: copy. */
                --tsize;
                break;
            }
        } else {
            *(tt++) = *(ff++);      /* Non-escape sequence. */
            --tsize;
        }
    }

    if (0 < tsize) {
        *(tt++) = '\0';
        --tsize;
    }

    return tt - to;
}

static const char HEX[] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

size_t diminuto_escape_expand(char * to, const char * from, size_t tsize, size_t fsize, const char * special)
{
    const char * ff;
    char * tt;
    size_t ss;
    const char * ee;

    ff = from;
    tt = to;

    while (0 < fsize) {
        if (tsize <= (ss = 1)) {
            break;
        } else if ((((const char *)0 == special) || ((const char *)0 == (ee = strchr(special, *ff)))) && (((' ' <= *ff) && (*ff <= '[')) || ((']' <= *ff) && (*ff <= '~')))) {
            *(tt++) = *ff;              /* Unescaped character. */
        } else if (tsize <= (ss = 2)) {
            break;
        } else if ('\\' == *ff) {
            *(tt++) = '\\';             /* Backslash. */
            *(tt++) = '\\';
        } else if ('\0' == *ff) {
           *(tt++) = '\\';              /* Zero a.k.a. NUL. */
            *(tt++) = '0';
        } else if ('\a' == *ff) {
            *(tt++) = '\\';             /* Alarm a.k.a. BEL. */
            *(tt++) = 'a';
        } else if ('\b' == *ff) {
            *(tt++) = '\\';             /* Backspace a.k.a. BS. */
            *(tt++) = 'b';
        } else if ('\f' == *ff) {
            *(tt++) = '\\';             /* Form feed a.k.a. FF. */
            *(tt++) = 'f';
        } else if ('\n' == *ff) {
            *(tt++) = '\\';             /* New line a.k.a. NL or LF. */
            *(tt++) = 'n';
        } else if ('\r' == *ff) {
            *(tt++) = '\\';             /* Return a.k.a. CR. */
            *(tt++) = 'r';
        } else if ('\t' == *ff) {
            *(tt++) = '\\';             /* Horizontal tab a.k.a. HT. */
            *(tt++) = 't';
        } else if ('\v' == *ff) {
            *(tt++) = '\\';             /* Vertical tab a.k.a. VT. */
            *(tt++) = 'v';
        } else if (((const char *)0 != special) && ((const char *)0 != ee)) {
            *(tt++) = '\\';             /* Special character. */
            *(tt++) = *ff;
        } else if (tsize <= (ss = 4)) {
            break;
        } else {
            *(tt++) = '\\';             /* Hexadecimal sequence. */
            *(tt++) = 'x';
            *(tt++) = HEX[(*ff >> 4) & 0x0f];
            *(tt++) = HEX[(*ff >> 0) & 0x0f];
        }
        ++ff;
        --fsize;
        tsize -= ss;
    }

    if (0 < tsize) {
        *(tt++) = '\0';
        --tsize;
    }

    return tt - to;
}

size_t diminuto_escape_trim(char * to, const char * from, size_t tsize, size_t fsize)
{
    const char * ff;
    size_t size;

    fsize = strnlen(from, fsize);
    size = fsize;
    ff = from + fsize - 1;

    while ((0 < size) && (' ' == *ff)) {
        --ff;
        --size;
    }

    if (0 == size) {
        /* Do nothing: zero length. */
    } else if (fsize == size) {
        /* Do nothing: no blanks. */
    } else if ((*ff != '\\') || ((size > 1) && (*(ff - 1) == '\\'))) {
        /* Do nothing: last blank not escaped. */
    } else {
        ++ff;
        ++size;
    }

    if (tsize < size) {
        size = tsize;
    }

    strncpy(to, from, size);

    if (tsize == 0) {
        /* Do nothing. */
    } else if (tsize > size) {
        to[size] = '\0';
    } else if (tsize == size) {
        to[--size] = '\0';
    } else {
        /* Do nothing. */
    }

    return size;
}

int diminuto_escape_printable(const char * from)
{
    int printable = !0;

    while (*from != '\0') {
        if (!isprint(*(from++))) {
            printable = 0;
            break;
        }
    }

    return printable;
}
