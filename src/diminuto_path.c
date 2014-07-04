/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_path.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

char * diminuto_path_find(const char * keyword, const char * file)
{
	char * path = (char *)0;
	const char * value;
	char * list = (char *)0;
	char * prefix;
	char * here;
	char * context;
	char * candidate = (char *)0;
	struct stat status;

	do {

		if (keyword == (const char *)0) {
			break;
		}

		if (file == (const char *)0) {
			break;
		}

		if ((value = getenv(keyword)) == (const char *)0) {
			break;
		}

		if ((list = strdup(value)) == (char *)0) {
			break;
		}

		here = list;

		while ((prefix = strtok_r(here, ":", &context)) != (char *)0) {

			if ((candidate = (char *)malloc(strlen(prefix) + strlen("/") + strlen(file) + sizeof(char))) == (char *)0) {
				break;
			}

			strcpy(candidate, prefix);
			strcat(candidate, "/");
			strcat(candidate, file);

			if (stat(candidate, &status) == 0) {
				path = candidate;
				break;
			}

			free(candidate);

			here = (char *)0;
		}

	} while (0);

	free(list);

	return path;
}
