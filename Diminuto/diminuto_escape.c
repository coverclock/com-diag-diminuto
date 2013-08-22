/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_escape.h"
#include <string.h>

size_t diminuto_escape_collapse(char * to, const char * from, size_t tsize)
{
    const char * ff;
    char * tt;
    char byte;
    int ii;

    ff = from;
    tt = to;

    while (('\0' != *ff) && (sizeof("") < tsize)) {
        if ('\\' == *ff) {
            switch (*(ff + 1)) {
            case '\0':                  /* Terminating NUL. */
                ++ff;
                break;
            case '\n':                  /* Escaped newline; skip over. */
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
            case 'x':                   /* Possible hexadecimal sequence. */
                byte = 0;
                /* ANSI hexadecimal sequences must have at least 1 digit. */
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
                *(tt++) = *(ff++);  /* Unknown escape sequence. */
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
    char buffer[sizeof("\\xff")];
    char * bb;
    size_t bsize;
    size_t ii;

    ff = from;
    tt = to;

    while ((0 < fsize) && (1 < tsize)) {
        bb = buffer;
        if ('\0' == *ff) {
            *(bb++) = '\\';             /* Escaped zero a.k.a. NUL. */
            *(bb++) = '0';
        } else if (((const char *)0 != special) && (0 != strchr(special, *ff))) {
            *(bb++) = '\\';             /* Escaped character. */
            *(bb++) = *ff;
        } else if ((' ' <= *ff) && (*ff <= '~')) {
            *(bb++) = *ff;              /* Non-escaped character. */
        } else if ('\a' == *ff) {
            *(bb++) = '\\';             /* Alarm a.k.a. BEL. */
            *(bb++) = 'a';
        } else if ('\b' == *ff) {
            *(bb++) = '\\';             /* Backspace a.k.a. BS. */
            *(bb++) = 'b';
        } else if ('\f' == *ff) {
            *(bb++) = '\\';             /* Formfeed a.k.a. FF. */
            *(bb++) = 'f';
        } else if ('\n' == *ff) {
            *(bb++) = '\\';             /* Newline a.k.a. NL or LF. */
            *(bb++) = 'n';
        } else if ('\r' == *ff) {
            *(bb++) = '\\';             /* Return a.k.a. CR. */
            *(bb++) = 'r';
        } else if ('\t' == *ff) {
            *(bb++) = '\\';             /* Horizontal tab a.k.a. HT. */
            *(bb++) = 't';
        } else if ('\v' == *ff) {
            *(bb++) = '\\';             /* Vertical tab a.k.a. VT. */
            *(bb++) = 'v';
        } else {
            *(bb++) = '\\';             /* Hexadecimal sequence. */
            *(bb++) = 'x';
            *(bb++) = HEX[(*ff >> 4) & 0x0f];
            *(bb++) = HEX[(*ff >> 0) & 0x0f];
        }
        ++ff;
        --fsize;
        bsize = bb - buffer;
        if (bsize >= tsize) {
            break;
        }
        for (ii = 0; ii < bsize; ++ii) {
            *(tt++) = *(buffer + ii);
        }
        tsize -= bsize;
    }

    if (0 < tsize) {
        *(tt++) = '\0';
        --tsize;
    }

    return tt - to;
}
