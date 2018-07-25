/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_escape.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include <string.h>

int main(void)
{
	SETLOGMASK();

	{
		char one[1 << (sizeof(char) * 8)];
		char two[(sizeof(one) * (sizeof("\\xff") - 1)) + 1];
		char three[sizeof(two)];
		static const char FOUR[] = "\\x01\\x02\\x03\\x04\\x05\\x06"
				                   "\\a\\b\\t\\n\\v\\f\\r"
				                   "\\x0e\\x0f"
				                   "\\x10\\x11\\x12\\x13\\x14\\x15\\x16\\x17"
				                   "\\x18\\x19\\x1a\\x1b\\x1c\\x1d\\x1e\\x1f"
				                   "\\ !\\\"#$%&'()*+,-./"
				                   "0123456789"
				                   ":;<=>?@"
				                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				                   "[\\\\]^_`"
				                   "abcdefghijklmnopqrstuvwxyz"
		                           "{|}~"
				                   "\\x7f"
				                   "\\x80\\x81\\x82\\x83\\x84\\x85\\x86\\x87"
				                   "\\x88\\x89\\x8a\\x8b\\x8c\\x8d\\x8e\\x8f"
				                   "\\x90\\x91\\x92\\x93\\x94\\x95\\x96\\x97"
				                   "\\x98\\x99\\x9a\\x9b\\x9c\\x9d\\x9e\\x9f"
				                   "\\xa0\\xa1\\xa2\\xa3\\xa4\\xa5\\xa6\\xa7"
				                   "\\xa8\\xa9\\xaa\\xab\\xac\\xad\\xae\\xaf"
				                   "\\xb0\\xb1\\xb2\\xb3\\xb4\\xb5\\xb6\\xb7"
				                   "\\xb8\\xb9\\xba\\xbb\\xbc\\xbd\\xbe\\xbf"
				                   "\\xc0\\xc1\\xc2\\xc3\\xc4\\xc5\\xc6\\xc7"
				                   "\\xc8\\xc9\\xca\\xcb\\xcc\\xcd\\xce\\xcf"
				                   "\\xd0\\xd1\\xd2\\xd3\\xd4\\xd5\\xd6\\xd7"
				                   "\\xd8\\xd9\\xda\\xdb\\xdc\\xdd\\xde\\xdf"
				                   "\\xe0\\xe1\\xe2\\xe3\\xe4\\xe5\\xe6\\xe7"
				                   "\\xe8\\xe9\\xea\\xeb\\xec\\xed\\xee\\xef"
				                   "\\xf0\\xf1\\xf2\\xf3\\xf4\\xf5\\xf6\\xf7"
				                   "\\xf8\\xf9\\xfa\\xfb\\xfc\\xfd\\xfe\\xff"
				                   "\\0";
		static const char FIVE[] = "\\x01\\x02\\x03\\x04\\x05\\x06"
				                   "\\a\\b\\t\\n\\v\\f\\r"
				                   "\\x0e\\x0f"
				                   "\\x10\\x11\\x12\\x13\\x14\\x15\\x16\\x17"
				                   "\\x18\\x19\\x1a\\x1b\\x1c\\x1d\\x1e\\x1f"
				                   " !\"#$%&'()*+,-./"
				                   "0123456789"
				                   ":;<=>?@"
				                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				                   "[\\\\]^_`"
				                   "abcdefghijklmnopqrstuvwxyz"
		                           "{|}~"
				                   "\\x7f"
				                   "\\x80\\x81\\x82\\x83\\x84\\x85\\x86\\x87"
				                   "\\x88\\x89\\x8a\\x8b\\x8c\\x8d\\x8e\\x8f"
				                   "\\x90\\x91\\x92\\x93\\x94\\x95\\x96\\x97"
				                   "\\x98\\x99\\x9a\\x9b\\x9c\\x9d\\x9e\\x9f"
				                   "\\xa0\\xa1\\xa2\\xa3\\xa4\\xa5\\xa6\\xa7"
				                   "\\xa8\\xa9\\xaa\\xab\\xac\\xad\\xae\\xaf"
				                   "\\xb0\\xb1\\xb2\\xb3\\xb4\\xb5\\xb6\\xb7"
				                   "\\xb8\\xb9\\xba\\xbb\\xbc\\xbd\\xbe\\xbf"
				                   "\\xc0\\xc1\\xc2\\xc3\\xc4\\xc5\\xc6\\xc7"
				                   "\\xc8\\xc9\\xca\\xcb\\xcc\\xcd\\xce\\xcf"
				                   "\\xd0\\xd1\\xd2\\xd3\\xd4\\xd5\\xd6\\xd7"
				                   "\\xd8\\xd9\\xda\\xdb\\xdc\\xdd\\xde\\xdf"
				                   "\\xe0\\xe1\\xe2\\xe3\\xe4\\xe5\\xe6\\xe7"
				                   "\\xe8\\xe9\\xea\\xeb\\xec\\xed\\xee\\xef"
				                   "\\xf0\\xf1\\xf2\\xf3\\xf4\\xf5\\xf6\\xf7"
				                   "\\xf8\\xf9\\xfa\\xfb\\xfc\\xfd\\xfe\\xff"
				                   "\\0";
		size_t size;

		for (size = 0; size < sizeof(one); ++size) {
			one[size] = size + 1;
		}
		DIMINUTO_LOG_DEBUG("one [%zu]\n", sizeof(one));
		diminuto_dump(stdout, one, sizeof(one));
		ASSERT(sizeof(one) == 256);

		size = diminuto_escape_expand(two, one, sizeof(two), sizeof(one), "\" ");
		DIMINUTO_LOG_DEBUG("two [%zu] \"%s\"\n", size, two);
		diminuto_dump(stdout, two, size);
		ASSERT(size == sizeof(FOUR));
		ASSERT(strcmp(two, FOUR) == 0);

		size = diminuto_escape_collapse(three, two, sizeof(three));
		DIMINUTO_LOG_DEBUG("three [%zu]\n", size);
		diminuto_dump(stdout, three, size);
		ASSERT(size == (sizeof(one) + 1));
		ASSERT(memcmp(one, three, sizeof(one)) == 0);

		size = diminuto_escape_expand(two, one, sizeof(two), sizeof(one), "");
		DIMINUTO_LOG_DEBUG("two [%zu] \"%s\"\n", size, two);
		diminuto_dump(stdout, two, size);
		ASSERT(size == sizeof(FIVE));
		ASSERT(strcmp(two, FIVE) == 0);

		size = diminuto_escape_collapse(three, two, sizeof(three));
		DIMINUTO_LOG_DEBUG("three [%zu]\n", size);
		diminuto_dump(stdout, three, size);
		ASSERT(size == (sizeof(one) + 1));
		ASSERT(memcmp(one, three, sizeof(one)) == 0);

		size = diminuto_escape_expand(two, one, sizeof(two), sizeof(one), (const char *)0);
		DIMINUTO_LOG_DEBUG("two [%zu] \"%s\"\n", size, two);
		diminuto_dump(stdout, two, size);
		ASSERT(size == sizeof(FIVE));
		ASSERT(strcmp(two, FIVE) == 0);

		size = diminuto_escape_collapse(three, two, sizeof(three));
		DIMINUTO_LOG_DEBUG("three [%zu]\n", size);
		diminuto_dump(stdout, three, size);
		ASSERT(size == (sizeof(one) + 1));
		ASSERT(memcmp(one, three, sizeof(one)) == 0);
	}

	{
		ASSERT(diminuto_escape_expand((char *)0, (const char *)0, 0, 0, (const char *)0) == 0);
		ASSERT(diminuto_escape_collapse((char *)0, (const char *)0, 0) == 0);
	}

	{
		static const char one[1] = "";
		char two[(sizeof(one) * (sizeof("\\xff") - 1)) + 1];
		char three[sizeof(two)];
		static const char FOUR[] = "";
		size_t size;

		ASSERT(sizeof(one) == 1);
		ASSERT(strlen(one) == 0);
		size = diminuto_escape_expand(two, one, sizeof(two), strlen(one), (const char *)0);
		ASSERT(size == 1);
		ASSERT(strcmp(two, FOUR) == 0);
		size = diminuto_escape_collapse(three, two, sizeof(three));
		ASSERT(size == 1);
		ASSERT(memcmp(one, three, sizeof(one)) == 0);
	}

	{
		char out[64];
		ASSERT(diminuto_escape_trim((char *)0, (const char *)0, 0, 0) == 0);
		ASSERT(diminuto_escape_trim(out, (const char *)0, sizeof(out), 0) == 0);
		ASSERT(diminuto_escape_trim(out, "", sizeof(out), 0) == 0);
		ASSERT(strcmp(out, "") == 0);
		ASSERT(diminuto_escape_trim(out, "", sizeof(out), 1) == 0);
		ASSERT(strcmp(out, "") == 0);
		ASSERT(diminuto_escape_trim(out, " ", sizeof(out), 0) == 0);
		ASSERT(strcmp(out, "") == 0);
		ASSERT(diminuto_escape_trim(out, " ", sizeof(out), 1) == 0);
		ASSERT(strcmp(out, "") == 0);
		ASSERT(diminuto_escape_trim(out, "123", sizeof(out), 3) == 3);
		ASSERT(strcmp(out, "123") == 0);
		ASSERT(diminuto_escape_trim(out, "123 ", sizeof(out), 6) == 3);
		ASSERT(strcmp(out, "123") == 0);
		ASSERT(diminuto_escape_trim(out, "123  ", sizeof(out), 6) == 3);
		ASSERT(strcmp(out, "123") == 0);
		ASSERT(diminuto_escape_trim(out, "123\\", sizeof(out), 4) == 4);
		ASSERT(strcmp(out, "123\\") == 0);
		ASSERT(diminuto_escape_trim(out, "123\\  ", sizeof(out), 6) == 5);
		ASSERT(strcmp(out, "123\\ ") == 0);
		ASSERT(diminuto_escape_trim(out, "123\\\\", sizeof(out), 5) == 5);
		ASSERT(strcmp(out, "123\\\\") == 0);
		ASSERT(diminuto_escape_trim(out, "123\\\\ ", sizeof(out), 5) == 5);
		ASSERT(strcmp(out, "123\\\\") == 0);
		ASSERT(diminuto_escape_trim(out, "  123  456  789  ", sizeof(out), 17) == 15);
		ASSERT(strcmp(out, "  123  456  789") == 0);
		ASSERT(diminuto_escape_trim(out, "123 \\   456 \\   789 \\   ", sizeof(out), 24) == 22);
		ASSERT(strcmp(out, "123 \\   456 \\   789 \\ ") == 0);
		ASSERT(diminuto_escape_trim(out, "123 \\   456 \\   789 \\   ", sizeof(out), 8) == 6);
		ASSERT(strcmp(out, "123 \\ ") == 0);
		ASSERT(diminuto_escape_trim(out, "123 \\   456 \\   789 \\   ", 7, 24) == 6);
		ASSERT(strcmp(out, "123 \\ ") == 0);
	}

	{
		unsigned char string[2] = "";
		int ii;
		int printable;
		for (ii = 0; ii < 256; ++ii) {
			string[0] = ii;
			printable = diminuto_escape_printable(string);
			ASSERT((((string[0] == '\0') || ((' ' <= string[0]) && (string[0] <= '~'))) && printable) || ((!((string[0] == '\0') || ((' ' <= string[0]) && (string[0] <= '~')))) && (!printable)));
		}
	}

    {
		static const char DATA[] = "\\0\\x01\\x02\\x03\\x04\\x05\\x06"
				                   "\\a\\b\\t\\n\\v\\f\\r"
				                   "\\x0e\\x0f"
				                   "\\x10\\x11\\x12\\x13\\x14\\x15\\x16\\x17"
				                   "\\x18\\x19\\x1a\\x1b\\x1c\\x1d\\x1e\\x1f"
				                   "\\ !\\\"#$%&'()*+,-./"
				                   "0123456789"
				                   ":;<=>?@"
				                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				                   "[\\\\]^_`"
				                   "abcdefghijklmnopqrstuvwxyz"
		                           "{|}~"
				                   "\\x7f"
				                   "\\x80\\x81\\x82\\x83\\x84\\x85\\x86\\x87"
				                   "\\x88\\x89\\x8a\\x8b\\x8c\\x8d\\x8e\\x8f"
				                   "\\x90\\x91\\x92\\x93\\x94\\x95\\x96\\x97"
				                   "\\x98\\x99\\x9a\\x9b\\x9c\\x9d\\x9e\\x9f"
				                   "\\xa0\\xa1\\xa2\\xa3\\xa4\\xa5\\xa6\\xa7"
				                   "\\xa8\\xa9\\xaa\\xab\\xac\\xad\\xae\\xaf"
				                   "\\xb0\\xb1\\xb2\\xb3\\xb4\\xb5\\xb6\\xb7"
				                   "\\xb8\\xb9\\xba\\xbb\\xbc\\xbd\\xbe\\xbf"
				                   "\\xc0\\xc1\\xc2\\xc3\\xc4\\xc5\\xc6\\xc7"
				                   "\\xc8\\xc9\\xca\\xcb\\xcc\\xcd\\xce\\xcf"
				                   "\\xd0\\xd1\\xd2\\xd3\\xd4\\xd5\\xd6\\xd7"
				                   "\\xd8\\xd9\\xda\\xdb\\xdc\\xdd\\xde\\xdf"
				                   "\\xe0\\xe1\\xe2\\xe3\\xe4\\xe5\\xe6\\xe7"
				                   "\\xe8\\xe9\\xea\\xeb\\xec\\xed\\xee\\xef"
				                   "\\xf0\\xf1\\xf2\\xf3\\xf4\\xf5\\xf6\\xf7"
				                   "\\xf8\\xf9\\xfa\\xfb\\xfc\\xfd\\xfe\\xff";
        unsigned char * buffer;
        size_t size;
        unsigned int ii;
		diminuto_dump(stdout, DATA, sizeof(DATA));
        buffer = (char *)malloc(sizeof(DATA));
        memcpy(buffer, DATA, sizeof(DATA));
		diminuto_dump(stdout, buffer, sizeof(DATA));
		size = diminuto_escape_collapse(buffer, buffer, sizeof(DATA));
		diminuto_dump(stdout, buffer, size);
        for (ii = 0; ii < (size - 1); ++ii) {
            ASSERT(buffer[ii] == ii);
        }
        ASSERT(buffer[size - 1] == '\0');
        free(buffer);
    }

    EXIT();
}
