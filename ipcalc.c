/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * ipcalc [ ipaddress [ netmask [ hostmask ] ] ]<BR>
 *
 * EXAMPLES
 *
 * ipcalc 192.168.1.222<BR>
 * ipcalc 192.168.1.222 255.255.255.0<BR>
 * ipcalc 192.168.1.222 255.255.255.0 255.255.255.255<BR>
 * ipcalc 192.168.1.222 255.255.255.0 0.0.0.0<BR>
 *
 * ABSTRACT
 *
 * ipcalc is a clean room reimplementation of a simple little utility
 * that can generate, depending on the number of parameters, a default
 * network mask, a default broadcast address, or a default subnet
 * number or host IP address. It is very useful in embedded systems to
 * generate the parameters needed in init scripts to configure network
 * devices.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <arpa/inet.h>

void usage(const char * pgm, FILE * fp)
{
    fprintf(fp, "Usage: %s 192.168.1.222\n", pgm);
    fprintf(fp, "       %s 192.168.1.222 255.255.255.0\n", pgm);
    fprintf(fp, "       %s 192.168.1.222 255.255.255.0 255.255.255.255\n", pgm);
}

int main(int argc, char ** argv)
{
    uint32_t addr[3];
	int ii;
    struct in_addr ip;

    if ((argc == 2) && (strcmp(argv[1], "-?") == 0)) {
        usage(argv[0], stdout);
        return 0;
    }

    if (!((2 <= argc) && (argc <= 4))) {
        usage(argv[0], stderr);
        return 1;
    }

    for (ii = 1; ii < argc; ++ii) {
        if (!inet_aton(argv[ii], &ip)) {
            errno = EINVAL;
            perror(argv[ii]);
            return 1;
        }
        addr[ii - 1] = ip.s_addr;
    }

    if (argc == 4) {
        
        ip.s_addr = (addr[0] & addr[1]) | (addr[2] & (~addr[1]));

    } else if (argc == 3) {

        ip.s_addr = (addr[0] & addr[1]) | ((~0) ^ addr[1]);

    } else {

        addr[0] = ntohl(addr[0]);

        if ((addr[0] & 0x80000000UL) == 0x00000000UL) {
            ip.s_addr = htonl(0xff000000UL);
        } else if ((addr[0] & 0xc0000000UL) == 0x80000000UL) {
            ip.s_addr = htonl(0xffff0000UL);
        } else {
            ip.s_addr = htonl(0xffffff00UL);
        }

    }        

    printf("%s\n", inet_ntoa(ip));

    return 0;
}
