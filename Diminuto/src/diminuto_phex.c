/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Arvada CO USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_phex.h"

/*                                        BEL  BS   TAB  LF   VT   FF   CR  */
/*                                        \a   \b   \t   \n   \v   \f   \r  */
static const unsigned char SPECIAL[] = { 'a', 'b', 't', 'n', 'v', 'f', 'r' };

void diminuto_phex_limit(FILE * fp, const size_t length, ssize_t increment, size_t * currentp, int * endp)
{
    if (length == 0) {
        /* Do nothing. */
    } else if ((increment < 0) && (!(*endp))) {
        fputc('\n', fp);
        *endp = !0;
    } else if ((increment == 0) && ((*currentp) > 0) && (!(*endp))) {
        fputc('\n', fp);
        *endp = !0;
    } else if (((*currentp) += increment) > length) {
        fputc('\n', fp);
        *endp = !0;
        *currentp = increment;
    } else {
        *endp = 0;
    }
}

void diminuto_phex_emit(FILE * fp, unsigned char ch, size_t length, int nl, int esc, int hex, size_t * currentp, int * endp, int flush)
{
    if ((ch == '\0') && hex) {
        diminuto_phex_limit(fp, length, 4, currentp, endp);
        fprintf(fp, "\\x%2.2x", ch);
    } else if (ch == '\0') {
        diminuto_phex_limit(fp, length, 2, currentp, endp);
        fputc('\\', fp);
        fputc('0', fp);
    } else if ((ch == '\n') && nl) {
        diminuto_phex_limit(fp, length, -1, currentp, endp);
    } else if ((('\a' <= ch) && (ch <= '\r')) && hex) {
        diminuto_phex_limit(fp, length, 4, currentp, endp);
        fprintf(fp, "\\x%2.2x", ch);
    } else if (('\a' <= ch) && (ch <= '\r')) {
        diminuto_phex_limit(fp, length, 2, currentp, endp);
        fputc('\\', fp);
        fputc(SPECIAL[ch - '\a'], fp);
    } else if ((ch == '\\') && hex) {
        diminuto_phex_limit(fp, length, 4, currentp, endp);
        fprintf(fp, "\\x%2.2x", ch);
    } else if (ch == '\\') {
        diminuto_phex_limit(fp, length, 2, currentp, endp);
        fputc('\\', fp);
        fputc(ch, fp);
    } else if (((ch == '"') || (ch == '\'') || (ch == '?')) && esc && hex) {
        diminuto_phex_limit(fp, length, 4, currentp, endp);
        fprintf(fp, "\\x%2.2x", ch);
    } else if (((ch == '"') || (ch == '\'') || (ch == '?')) && esc) {
        diminuto_phex_limit(fp, length, 2, currentp, endp);
        fputc('\\', fp);
        fputc(ch, fp);
    } else if ((' ' <= ch) && (ch <= '~')) {
        diminuto_phex_limit(fp, length, 1, currentp, endp);
        fputc(ch, fp);
    } else {
        diminuto_phex_limit(fp, length, 4, currentp, endp);
        fprintf(fp, "\\x%2.2x", ch);
    }
    if (flush) {
        (void)fflush(fp);
    }
}
