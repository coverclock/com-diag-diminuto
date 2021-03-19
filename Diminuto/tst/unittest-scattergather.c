/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the scatter/gather feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * I find the command
 *
 * valgrind --leak-check=full --show-leak-kinds=all unittest-ipc-scattergather
 *
 * useful for insuring I've covered all the memory allocation and freeing
 * correctly.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_scattergather.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_fletcher.h"
#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

static diminuto_scattergather_pool_t pool;

static diminuto_scattergather_segment_t segments[DIMINUTO_SCATTERGATHER_NODES];

/*******************************************************************************
 * FARENDS
 ******************************************************************************/

/*
 * In any real application, we would be passing the binary fields in
 * network byte order, and the field lengths would be consistent. Here,
 * we pass everything in host byte order, and the length field is eight
 * bytes on the x86_64 and four bytes on an ARM with a 32-bit kernel. We
 * would also not assume below that the entire packet had been read or
 * received in a single system call. I didn't make any effort to optimize
 * or minimize the error leg code paths like I would in production code.
 * I try several different methods to receive and parse data streams that
 * have been sent using writev(2) or sendmsg(2), the gather system calls.
 * In all cases, the data packet on the wire has the following format.
 *
 * PACKET: ADDRESS[4], PORT[2], LENGTH[4 or 8], DATA[LENGTH], CHECKSUM[2]
 */

enum Offsets {
    ADDRESS = 0,
    PORT    = ADDRESS   + sizeof(diminuto_ipv4_t),
    LENGTH  = PORT      + sizeof(diminuto_port_t),
    DATA    = LENGTH    + sizeof(size_t),
};

enum Lengths {
    HEADER  = DATA,
    MINIMUM = HEADER   + sizeof(uint16_t), /* Zero length payload. */
    MAXIMUM = MINIMUM   + 256, /* Arbitrary. */
};

/*
 * Stream Socket Server
 */
int streamserver(int listensocket)
{
    int result = 1;
    int streamsocket;
    uint8_t * bp;
    ssize_t total;
    diminuto_ipv4_t address;
    diminuto_port_t port;
    size_t length;
    uint16_t checksum;
    diminuto_ipv4_buffer_t printable;
    uint8_t a;
    uint8_t b;
    uint16_t expected;
    struct iovec vector[5];
    size_t minimum;
    diminuto_scattergather_record_t * rp;
    diminuto_scattergather_segment_t * sp;
    int ii;

    do {

        if ((streamsocket = diminuto_ipc4_stream_accept(listensocket)) < 0) {
            break;
        }

        /*
         * This approach reads the packet a field at a time. Reading multiple
         * fields at a time, such as the front matter that preceeds the payload,
         * into a structure isn't as easy as it sounds. Structs can have holes
         * due to the memory alignment requirements of their individial fields,
         * while bits on the wire have no such gaps.
         */

        fprintf(stderr, "streamserver: ONE\n");

        fprintf(stderr, "READ:\n");

        if ((total = diminuto_ipc4_stream_read_generic(streamsocket, &address, sizeof(address), sizeof(address))) != sizeof(address)) {
            errno = EINVAL;
            diminuto_perror("short");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        if ((total = diminuto_ipc4_stream_read_generic(streamsocket, &port, sizeof(port), sizeof(port))) != sizeof(port)) {
            errno = EINVAL;
            diminuto_perror("short");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        fprintf(stderr, "  PORT: %d\n", port);

        if ((total = diminuto_ipc4_stream_read_generic(streamsocket, &length, sizeof(length), sizeof(length))) != sizeof(length)) {
            errno = EINVAL;
            diminuto_perror("short");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        fprintf(stderr, "  LENGTH: %zu\n", length);

        if (length > (MAXIMUM - MINIMUM)) {
            errno = EINVAL;
            diminuto_perror("length");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        if (length > 0) {

            bp = (uint8_t *)diminuto_buffer_malloc(length);
            if (bp == (uint8_t *)0) {
                (void)diminuto_ipc_close(streamsocket);
                break;
            }

            if ((total = diminuto_ipc4_stream_read_generic(streamsocket, bp, length, length)) != length) {
                errno = EINVAL;
                diminuto_perror("short");
                (void)diminuto_ipc_close(streamsocket);
                break;
            }
            fprintf(stderr, "  DATA:\n");
            diminuto_dump_general(stderr, bp, total, 0, '.', 0, 0, 4);

        }

        if ((total = diminuto_ipc4_stream_read_generic(streamsocket, &checksum, sizeof(checksum), sizeof(checksum))) != sizeof(checksum)) {
            errno = EINVAL;
            diminuto_perror("short");
            if (length > 0) {
                diminuto_buffer_free(bp);
            }
            break;
        }
        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (checksum != expected) {
            errno = EINVAL;
            diminuto_perror("checksum");
            if (length > 0) {
                diminuto_buffer_free(bp);
            }
            break;
        }

        if (length > 0) {
            diminuto_buffer_free(bp);
        }

        /*
         * This approach uses readv(2) to read the first three fields and
         * scatter them into their variables. Then it uses readv(2) again to
         * read the payload (now that we know how long it is) and the
         * checksum. I don't think this will work with datagrams, which are
         * treated as one record.
         */

        minimum = 0;

        vector[0].iov_base = &address;
        vector[0].iov_len = sizeof(address);
        minimum += vector[0].iov_len;

        vector[1].iov_base = &port;
        vector[1].iov_len = sizeof(port);
        minimum += vector[1].iov_len;

        vector[2].iov_base = &length;
        vector[2].iov_len = sizeof(length);
        minimum += vector[2].iov_len;

        if ((total = readv(streamsocket, &vector[0], 3)) < minimum) {
            errno = EINVAL;
            diminuto_perror("short");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        fprintf(stderr, "streamserver: TWO\n");

        fprintf(stderr, "READ [%zd]:\n", total);

        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        fprintf(stderr, "  PORT: %d\n", port);

        fprintf(stderr, "  LENGTH: %zu\n", length);

        minimum = 0;
        ii = 0;

        if (length > 0) {

            bp = (uint8_t *)diminuto_buffer_malloc(length);
            if (bp == (uint8_t *)0) {
                (void)diminuto_ipc_close(streamsocket);
                break;
            }
            vector[ii].iov_base = bp;
            vector[ii].iov_len = length;
            minimum += vector[ii].iov_len;
            ii += 1;
 
        }

        vector[ii].iov_base = &checksum;
        vector[ii].iov_len = sizeof(checksum);
        minimum += vector[ii].iov_len;
        ii += 1;

        if ((total = readv(streamsocket, &vector[0], ii)) < minimum) {
            errno = EINVAL;
            diminuto_perror("short");
            if (length > 0) {
                diminuto_buffer_free(bp);
            }
            break;
        }

        fprintf(stderr, "READ [%zd]:\n", total);

        fprintf(stderr, "  DATA:\n");
        diminuto_dump_general(stderr, bp, length, 0, '.', 0, 0, 4);

        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            if (length > 0) {
                diminuto_buffer_free(bp);
            }
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        if (length > 0) {
            diminuto_buffer_free(bp);
        }

        /*
         * This approach uses the record and segment facility just like
         * the sender, with the complication that we have to deal with
         * zero-length payloads. (This test program doesn't actually
         * send zero length payloads, so that path has not actually been
         * tested). This would only work for stream sockets, since we
         * read the incoming packet in two parts: the header and the
         * payload plus checksum.
         */

        if ((rp = diminuto_scattergather_record_allocate(&pool)) == (diminuto_scattergather_record_t *)0) {
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        if ((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(diminuto_ipv4_t))) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        diminuto_scattergather_record_segment_append(rp, sp);

        if ((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(diminuto_port_t))) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        diminuto_scattergather_record_segment_append(rp, sp);

        if ((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(size_t))) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        diminuto_scattergather_record_segment_append(rp, sp);

        if ((total = diminuto_scattergather_record_read(streamsocket, rp)) != HEADER) {
            errno = EINVAL;
            diminuto_perror("short");
            diminuto_scattergather_record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        fprintf(stderr, "streamserver: THREE\n");

        diminuto_scattergather_record_dump(stderr, rp);
        fprintf(stderr, "READ [%zd]:\n", total);

        address = *(diminuto_ipv4_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));
        diminuto_scattergather_record_segment_free(&pool, rp, sp);

        port = *(diminuto_port_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
        fprintf(stderr, "  PORT: %d\n", port);
        diminuto_scattergather_record_segment_free(&pool, rp, sp);

        length = *(size_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
        fprintf(stderr, "  LENGTH: %zu\n", length);
        diminuto_scattergather_record_segment_free(&pool, rp, sp);

        if (length <= 0) {
            /* Do nothing. */
        } else if ((sp = diminuto_scattergather_segment_allocate(&pool, length)) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        } else {
            diminuto_scattergather_record_segment_append(rp, sp);
        }

        if ((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(uint16_t))) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        diminuto_scattergather_record_segment_append(rp, sp);

        if ((total = diminuto_scattergather_record_read(streamsocket, rp)) != (length + sizeof(uint16_t))) {
            errno = EINVAL;
            diminuto_perror("short");
            diminuto_scattergather_record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        diminuto_scattergather_record_dump(stderr, rp);
        fprintf(stderr, "READ [%zd]:\n", total);

        if (length > 0) {
            fprintf(stderr, "  DATA:\n");
            bp = (uint8_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
            diminuto_dump_general(stderr, bp, length, 0, '.', 0, 0, 4);
            diminuto_scattergather_record_segment_free(&pool, rp, sp);
        } else {
            bp = (uint8_t *)0;
        }

        checksum = *(uint16_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);
        diminuto_scattergather_record_segment_free(&pool, rp, sp);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            diminuto_scattergather_record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        diminuto_scattergather_record_free(&pool, rp);

        /*
         * This approach just reads the entire packet into a single
         * contiguous buffer. Since the payload is of variable length,
         * if we were receiving multiple messages using read(2) we
         * would have to effectively parse the input stream since
         * read(2) might return fewer or more bytes than were written
         * in a single write(2) call from the far end. That's why it's
         * called a stream versus a datagram. And why this approach has
         * to be the last one; otherwise we'd would consume part or all
         * of the following packet.
         */

        bp = (uint8_t *)diminuto_buffer_malloc(MAXIMUM);
        if (bp == (uint8_t *)0) {
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        if ((total = diminuto_ipc4_stream_read(streamsocket, bp, MAXIMUM)) < MINIMUM) {
            errno = EINVAL;
            diminuto_perror("short");
            diminuto_buffer_free(bp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        fprintf(stderr, "streamserver: FOUR\n");

        fprintf(stderr, "READ [%zd]:\n", total);
        diminuto_dump_general(stderr, bp, total, 0, '.', 0, 0, 2);

        memcpy(&address, &bp[ADDRESS], sizeof(address));
        fprintf(stderr, "    ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        memcpy(&port, &bp[PORT], sizeof(port));
        fprintf(stderr, "    PORT: %d\n", port);

        memcpy(&length, &bp[LENGTH], sizeof(length));
        fprintf(stderr, "    LENGTH: %zu\n", length);
        if ((MINIMUM + length) != total) {
            errno = EINVAL;
            diminuto_perror("length");
            diminuto_buffer_free(bp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        memcpy(&checksum, &bp[DATA + length], sizeof(checksum));
        fprintf(stderr, "    CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(&bp[DATA], length, &a, &b);
        fprintf(stderr, "    EXPECTED: 0x%x\n", expected);
        if (checksum != expected) {
            errno = EINVAL;
            diminuto_perror("checksum");
            diminuto_buffer_free(bp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        diminuto_buffer_free(bp);

        /*
         * Done.
         */

        if (diminuto_ipc_close(streamsocket) < 0) {
            break;
        }

        result = 0;

    } while (0);

    return result; 
}

/*
 * Datagram Peer
 */
int datagrampeer(int datagramsocket)
{
    int result = 1;
    ssize_t total;
    diminuto_ipv4_t address;
    diminuto_port_t port;
    size_t length;
    uint8_t * bp;
    uint16_t checksum;
    diminuto_ipv4_buffer_t printable;
    uint8_t a;
    uint8_t b;
    uint16_t expected;
    struct msghdr message;
    struct iovec vector[4];
    size_t maximum;
    diminuto_scattergather_record_t * rp;
    diminuto_scattergather_segment_t * sp;
    int ii;

    do {

        /*
         * This approach receives the first datagram all in one piece and
         * parses it apart.
         */

        bp = (uint8_t *)diminuto_buffer_malloc(MAXIMUM);
        if (bp == (uint8_t *)0) {
            break;
        }

        if ((total = diminuto_ipc4_datagram_receive(datagramsocket, bp, MAXIMUM)) < MINIMUM) {
            errno = EINVAL;
            diminuto_perror("short");
            diminuto_buffer_free(bp);
            break;
        }

        fprintf(stderr, "datagrampeer: ONE\n");

        fprintf(stderr, "RECEIVE [%zd]:\n", total);
        diminuto_dump_general(stderr, bp, total, 0, '.', 0, 0, 2);

        memcpy(&address, &bp[ADDRESS], sizeof(address));
        fprintf(stderr, "    ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        memcpy(&port, &bp[PORT], sizeof(port));
        fprintf(stderr, "    PORT: %d\n", port);

        memcpy(&length, &bp[LENGTH], sizeof(length));
        fprintf(stderr, "    LENGTH: %zu\n", length);
        if ((length + MINIMUM) != total) {
            errno = EINVAL;
            diminuto_perror("length");
            diminuto_buffer_free(bp);
            break;
        }
    
        memcpy(&checksum, &bp[DATA +  length], sizeof(checksum));
        fprintf(stderr, "    CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(&bp[DATA], length, &a, &b);
        fprintf(stderr, "    EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            diminuto_buffer_free(bp);
            break;
        }

        diminuto_buffer_free(bp);

        /*
         * This approach receives the second datagram and use recvmsg(2) to
         * scatter the fields. Because we don't know the length of the variable
         * length payload, we have to receive the payload and checksum
         * into a single buffer and extract the checksum afterwards.
         */

        message.msg_name = (void *)0;
        message.msg_namelen = 0;
        message.msg_iov = vector;
        message.msg_iovlen = diminuto_countof(vector);
        message.msg_control = (void *)0;
        message.msg_controllen = 0;
        message.msg_flags = 0;

        vector[0].iov_base = &address;
        vector[0].iov_len = sizeof(address);

        vector[1].iov_base = &port;
        vector[1].iov_len = sizeof(port);

        vector[2].iov_base = &length;
        vector[2].iov_len = sizeof(length);

        maximum = MAXIMUM - MINIMUM + sizeof(uint16_t);
        bp = (uint8_t *)diminuto_buffer_malloc(maximum);
        if (bp == (uint8_t *)0) {
            break;
        }
        vector[3].iov_base = bp;
        vector[3].iov_len = maximum;

        if ((total = recvmsg(datagramsocket, &message, 0)) < MINIMUM) {
            errno = EINVAL;
            diminuto_perror("short");
            diminuto_buffer_free(bp);
            break;
        }

        fprintf(stderr, "datagrampeer: TWO\n");

        fprintf(stderr, "RECEIVE [%zd]:\n", total);

        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        fprintf(stderr, "  PORT: %d\n", port);

        fprintf(stderr, "  LENGTH: %zu\n", length);
        if ((length + MINIMUM) != total) {
            errno = EINVAL;
            diminuto_perror("length");
            diminuto_buffer_free(bp);
            break;
        }

        fprintf(stderr, "  DATA:\n");
        diminuto_dump_general(stderr, bp, length, 0, '.', 0, 0, 4);

        memcpy(&checksum, &bp[length], sizeof(checksum));
        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            diminuto_buffer_free(bp);
            break;
        }

        diminuto_buffer_free(bp);

        /*
         * This approach is similar to the one above except it uses the
         * record and segment paradigm which leaves the checksum as the
         * only field we have to handle as a special case. Because we
         * receive the entire datagram all at once (that's how datagrams
         * work) we don't know the length of the last segment that
         * contains both the payload and the checksum until we interrogate
         * the length segment.
         */

        if ((rp = diminuto_scattergather_record_allocate(&pool)) == (diminuto_scattergather_record_t *)0) {
            break;
        }

        if ((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(diminuto_ipv4_t))) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            break;
        }
        diminuto_scattergather_record_segment_append(rp, sp);

        if ((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(diminuto_port_t))) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            break;
        }
        diminuto_scattergather_record_segment_append(rp, sp);

        if ((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(size_t))) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            break;
        }
        diminuto_scattergather_record_segment_append(rp, sp);

        maximum = MAXIMUM - MINIMUM + sizeof(uint16_t);
        if ((sp = diminuto_scattergather_segment_allocate(&pool, maximum)) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            break;
        }
        diminuto_scattergather_record_segment_append(rp, sp);

        if ((total = diminuto_scattergather_record_receive(datagramsocket, rp)) < MINIMUM) {
            errno = EINVAL;
            diminuto_perror("short");
            diminuto_scattergather_record_free(&pool, rp);
            break;
        }

        fprintf(stderr, "datagrampeer: THREE\n");

        diminuto_scattergather_record_dump(stderr, rp);
        fprintf(stderr, "RECEIVE [%zd]:\n", total);

        address = *(diminuto_ipv4_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));
        diminuto_scattergather_record_segment_free(&pool, rp, sp);

        port = *(diminuto_port_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
        fprintf(stderr, "  PORT: %d\n", port);
        diminuto_scattergather_record_segment_free(&pool, rp, sp);

        length = *(size_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
        fprintf(stderr, "  LENGTH: %zu\n", length);
        diminuto_scattergather_record_segment_free(&pool, rp, sp);
        if ((HEADER + length + sizeof(uint16_t)) != total) {
            errno = EINVAL;
            diminuto_perror("length");
            diminuto_scattergather_record_free(&pool, rp);
            break;
        }

        /* Kinda overkill to use a segment here but stick with the paradigm. */

        diminuto_scattergather_segment_length_set(diminuto_scattergather_record_segment_head(rp), length);

        if ((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(uint16_t))) == (diminuto_scattergather_segment_t *)0) {
            diminuto_scattergather_record_free(&pool, rp);
            break;
        }
        bp = (uint8_t *)diminuto_scattergather_segment_payload_get(diminuto_scattergather_record_segment_head(rp));
        memcpy(diminuto_scattergather_segment_payload_get(sp), &bp[length], diminuto_scattergather_segment_length_get(sp));
        diminuto_scattergather_record_segment_append(rp, sp);

        /* There's a checksum segment now and the data length is correct. */

        if (length > 0) {
            fprintf(stderr, "  DATA:\n");
            bp = (uint8_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
            diminuto_dump_general(stderr, bp, length, 0, '.', 0, 0, 4);
            diminuto_scattergather_record_segment_free(&pool, rp, sp);
        } else {
            bp = (uint8_t *)0;
        }

        checksum = *(uint16_t *)diminuto_scattergather_segment_payload_get(sp = diminuto_scattergather_record_segment_head(rp));
        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            diminuto_scattergather_record_free(&pool, rp);
            break;
        }

        diminuto_scattergather_record_free(&pool, rp);

        /*
         * Done.
         */

        result = 0;

    } while (0);

    return result;
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/

int main(void)
{
    diminuto_scattergather_record_t * rp;
    diminuto_ipv4_t address;
    diminuto_port_t streamport;
    diminuto_port_t datagramport;
    int listensocket;
    int datagramsocket;
    pid_t streampid;
    pid_t datagrampid;
    static const char DATA[] = "Now is the time for all good men to come to the aid of their country.";

    SETLOGMASK();

    {
        diminuto_scattergather_segment_t segment = DIMINUTO_SCATTERGATHER_SEGMENT_INIT(&segment);

        TEST();

        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&segment) == 0);
        ASSERT(diminuto_scattergather_record_measure(&segment) == 0);

        STATUS();
    }

    {
        diminuto_scattergather_segment_t segment;

        TEST();

        ASSERT(diminuto_scattergather_segment_init(&segment) == &segment);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&segment) == 0);
        ASSERT(diminuto_scattergather_record_measure(&segment) == 0);
        diminuto_scattergather_segment_fini(&segment);

        STATUS();
    }

    {
        diminuto_scattergather_record_t record = DIMINUTO_SCATTERGATHER_RECORD_INIT(&record);

        TEST();

        ASSERT(diminuto_scattergather_record_enumerate(&record) == 0);
        ASSERT(diminuto_scattergather_record_measure(&record) == 0);
        ASSERT(diminuto_scattergather_record_dump(stderr, &record) == &record);

        STATUS();
    }

    {
        diminuto_scattergather_record_t record;

        TEST();

        ASSERT(diminuto_scattergather_record_init(&record) == &record);
        ASSERT(diminuto_scattergather_record_enumerate(&record) == 0);
        ASSERT(diminuto_scattergather_record_measure(&record) == 0);
        ASSERT(diminuto_scattergather_record_dump(stderr, &record) == &record);
        diminuto_scattergather_record_fini(&record);

        STATUS();
    }

    {
        diminuto_scattergather_pool_t pool = DIMINUTO_SCATTERGATHER_POOL_INIT(&pool); /* Private. */
        diminuto_scattergather_segment_t segments[7]; /* Private. */

        TEST();

        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == 0);
        ASSERT(diminuto_scattergather_record_measure(&pool) == 0);
        ASSERT(diminuto_scattergather_pool_populate(&pool, segments, countof(segments)) == &pool);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == countof(segments));
        ASSERT(diminuto_scattergather_record_measure(&pool) == 0);
        diminuto_scattergather_pool_fini(&pool);
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == 0);
        ASSERT(diminuto_scattergather_record_measure(&pool) == 0);

        STATUS();
    }

    {
        diminuto_scattergather_pool_t pool; /* Private. */
        diminuto_scattergather_segment_t segments[11]; /* Private. */

        TEST();

        ASSERT(diminuto_scattergather_pool_init(&pool) == &pool);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == 0);
        ASSERT(diminuto_scattergather_record_measure(&pool) == 0);
        ASSERT(diminuto_scattergather_pool_populate(&pool, segments, countof(segments)) == &pool);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == countof(segments));
        ASSERT(diminuto_scattergather_record_measure(&pool) == 0);
        diminuto_scattergather_pool_fini(&pool);
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == 0);
        ASSERT(diminuto_scattergather_record_measure(&pool) == 0);

        STATUS();
    }

    {
        void * dp;
        diminuto_scattergather_buffer_t buffer;
        diminuto_scattergather_buffer_t * bp;
        diminuto_scattergather_pool_t pool; /* Private. */
        diminuto_scattergather_segment_t segments[1]; /* Private. */
        diminuto_scattergather_segment_t * sp;

        TEST();

        dp = (void *)&(buffer.payload[0]);
        ASSERT(diminuto_scattergather_bufferof(dp) == &buffer);
        bp = malloc(sizeof(buffer));
        dp = (void *)&(bp->payload[0]);
        ASSERT(diminuto_scattergather_bufferof(dp) == bp);
        free(bp);
        ASSERT(diminuto_scattergather_pool_init(&pool) == &pool);
        ASSERT(diminuto_scattergather_pool_populate(&pool, segments, countof(segments)) == &pool);
        ASSERT((sp = diminuto_scattergather_segment_allocate(&pool, 13)) != (diminuto_scattergather_segment_t *)0);
        ASSERT(sp->data != (void *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == 13);
        ASSERT(diminuto_scattergather_segment_length_set(sp, 17) == 17);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == 17);
        ASSERT((dp = diminuto_scattergather_segment_payload_get(sp)) != (void *)0);
        ASSERT(diminuto_scattergather_bufferof(dp) == (diminuto_scattergather_buffer_t *)(sp->data));
        diminuto_scattergather_segment_free(&pool, sp);
        diminuto_scattergather_pool_fini(&pool);

        STATUS();
    }

    {
        TEST();

        ASSERT(diminuto_scattergather_pool_init(&pool) == &pool);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == 0);
        ASSERT(diminuto_scattergather_pool_populate(&pool, segments, countof(segments)) == &pool);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == countof(segments));
        ASSERT(diminuto_scattergather_record_measure(&pool) == 0);

        STATUS();
    }

    {
        diminuto_scattergather_segment_t * sp[3];
        void * dp;

        TEST();

        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == (DIMINUTO_SCATTERGATHER_NODES - 0));
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT((sp[0] = diminuto_scattergather_segment_allocate(&pool, sizeof(size_t))) != (diminuto_scattergather_segment_t *)0);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == (DIMINUTO_SCATTERGATHER_NODES - 1));
        ASSERT((dp = diminuto_scattergather_segment_payload_get(sp[0])) != (void *)0);
        ASSERT((((uintptr_t)dp) & 0x7) == 0);
        ASSERT(diminuto_scattergather_segment_length_get(sp[0]) == sizeof(size_t));
        ASSERT(diminuto_scattergather_segment_length_set(sp[0], 0) == 0);
        ASSERT(diminuto_scattergather_segment_length_get(sp[0]) == 0);
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT((sp[1] = diminuto_scattergather_segment_allocate(&pool, sizeof(uint64_t))) != (diminuto_scattergather_segment_t *)0);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == (DIMINUTO_SCATTERGATHER_NODES - 2));
        ASSERT((dp = diminuto_scattergather_segment_payload_get(sp[1])) != (void *)0);
        ASSERT((((uintptr_t)dp) & 0x7) == 0);
        ASSERT(diminuto_scattergather_segment_length_get(sp[1]) == sizeof(uint64_t));
        ASSERT(diminuto_scattergather_segment_length_set(sp[1], 0) == 0);
        ASSERT(diminuto_scattergather_segment_length_get(sp[1]) == 0);
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT((sp[2] = diminuto_scattergather_segment_allocate(&pool, 64)) != (diminuto_scattergather_segment_t *)0);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == (DIMINUTO_SCATTERGATHER_NODES - 3));
        ASSERT((dp = diminuto_scattergather_segment_payload_get(sp[2])) != (void *)0);
        ASSERT((((uintptr_t)dp) & 0x7) == 0);
        ASSERT(diminuto_scattergather_segment_length_get(sp[2]) == 64);
        ASSERT(diminuto_scattergather_segment_length_set(sp[2], 0) == 0);
        ASSERT(diminuto_scattergather_segment_length_get(sp[2]) == 0);
        ASSERT(diminuto_buffer_log() >= 0);

        /* Order is deliberate. */

        diminuto_scattergather_segment_free(&pool, sp[0]);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == (DIMINUTO_SCATTERGATHER_NODES - 2));
        ASSERT(diminuto_buffer_log() >= 0);

        diminuto_scattergather_segment_free(&pool, sp[2]);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == (DIMINUTO_SCATTERGATHER_NODES - 1));
        ASSERT(diminuto_buffer_log() >= 0);

        diminuto_scattergather_segment_free(&pool, sp[1]);
        /* Everything is a List node. */
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == (DIMINUTO_SCATTERGATHER_NODES - 0));
        ASSERT(diminuto_buffer_log() >= 0);

        STATUS();
    }

    {
        TEST();

        ASSERT((rp = diminuto_scattergather_record_allocate(&pool)) != (diminuto_scattergather_record_t *)0);
        ASSERT(diminuto_scattergather_record_enumerate(rp) == 0);
        ASSERT(diminuto_scattergather_record_measure(rp) == 0);
        ASSERT(diminuto_scattergather_record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        diminuto_scattergather_segment_t * sp;
        uint8_t * bp;
        size_t ll;

        /* Payload */

        TEST();

        ASSERT((sp = diminuto_scattergather_segment_allocate(&pool, MAXIMUM)) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == MAXIMUM);
        ASSERT((bp = (uint8_t *)diminuto_scattergather_segment_payload_get(sp)) != (uint8_t *)0);
        ASSERT((ll = (strlen(DATA) + 1 /* Including NUL. */)) > 0);
        strncpy(bp, DATA, ll);
        ASSERT(diminuto_scattergather_segment_length_set(sp, ll) == ll);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == ll);
        ASSERT(diminuto_scattergather_record_segment_append(rp, sp) == sp);

        ASSERT(diminuto_scattergather_record_enumerate(rp) == 1);
        ASSERT(diminuto_scattergather_record_measure(rp) > 0);
        ASSERT(diminuto_scattergather_record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        diminuto_scattergather_segment_t * sp;
        uint8_t * bp;
        size_t length;
        uint8_t a;
        uint8_t b;
        uint16_t checksum;

        /* Length Payload Checksum */

        TEST();

        ASSERT((sp = diminuto_scattergather_record_segment_head(rp)) != (diminuto_scattergather_segment_t *)0);
        ASSERT((bp = (uint8_t *)diminuto_scattergather_segment_payload_get(sp)) != (uint8_t *)0);
        ASSERT((length = diminuto_scattergather_segment_length_get(sp)) > 0);
        a = b = 0;
        checksum = diminuto_fletcher_16(bp, length, &a, &b);

        ASSERT((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(length))) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == sizeof(length));
        ASSERT((bp = (uint8_t *)diminuto_scattergather_segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &length, sizeof(length));
        ASSERT(diminuto_scattergather_record_segment_prepend(rp, sp) == sp);
 
        ASSERT((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(checksum))) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == sizeof(checksum));
        ASSERT((bp = (uint8_t *)diminuto_scattergather_segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &checksum, sizeof(checksum));
        ASSERT(diminuto_scattergather_record_segment_append(rp, sp) == sp);
    
        ASSERT(diminuto_scattergather_record_enumerate(rp) == 3);
        ASSERT(diminuto_scattergather_record_measure(rp) > 0);
        ASSERT(diminuto_scattergather_record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        fflush(stderr);
    }

    {
        TEST();

        address = diminuto_ipc4_address("localhost");
        ASSERT(!diminuto_ipc4_is_unspecified(&address));

        ASSERT((listensocket = diminuto_ipc4_stream_provider(0)) >= 0);
        ASSERT(diminuto_ipc4_nearend(listensocket, (diminuto_ipv4_t *)0, &streamport) >= 0);

        ASSERT((datagramsocket = diminuto_ipc4_datagram_peer(0)) >= 0);
        ASSERT(diminuto_ipc4_nearend(datagramsocket, (diminuto_ipv4_t *)0, &datagramport) >= 0);

        if ((streampid = fork()) == 0) {
            diminuto_scattergather_segment_t * sp;
            int xc;
            ASSERT(diminuto_scattergather_record_segments_free(&pool, rp) == rp);
            ASSERT(diminuto_scattergather_record_enumerate(rp) == 0);
            ASSERT(diminuto_scattergather_record_measure(rp) == 0);
            diminuto_scattergather_record_free(&pool, rp);
            xc = streamserver(listensocket);
            ASSERT(diminuto_ipc_close(listensocket) >= 0);
            /* To make valgrind(1) happy. */
            while ((sp = diminuto_list_head(&pool)) != (diminuto_scattergather_segment_t *)0) {
                ASSERT(sp->data == (void *)0);
                diminuto_list_remove(sp);
            }
            diminuto_buffer_log();
            diminuto_buffer_fini();
            diminuto_buffer_log();
            exit(xc);
        }
        ASSERT(streampid > 0);
        ASSERT(diminuto_ipc_close(listensocket) >= 0);

        if ((datagrampid = fork()) == 0) {
            diminuto_scattergather_segment_t * sp;
            int xc;
            ASSERT(diminuto_scattergather_record_segments_free(&pool, rp) == rp);
            ASSERT(diminuto_scattergather_record_enumerate(rp) == 0);
            ASSERT(diminuto_scattergather_record_measure(rp) == 0);
            diminuto_scattergather_record_free(&pool, rp);
            xc = datagrampeer(datagramsocket);
            ASSERT(diminuto_ipc_close(datagramsocket) >= 0);
            /* TO make valgrind(1) happy. */
            while ((sp = diminuto_list_head(&pool)) != (diminuto_scattergather_segment_t *)0) {
                ASSERT(sp->data == (void *)0);
                diminuto_list_remove(sp);
            }
            diminuto_buffer_log();
            diminuto_buffer_fini();
            diminuto_buffer_log();
            exit(xc);
        }
        ASSERT(datagrampid  > 0);
        ASSERT(diminuto_ipc_close(datagramsocket) >= 0);

        STATUS();
    }

    {
        uint8_t * bp;
        diminuto_scattergather_segment_t * sp;
        diminuto_scattergather_segment_t * tp;
        ssize_t length;

        /* Address StreamPort Length Payload Checksum */

        TEST();

        ASSERT((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(address))) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == sizeof(address));
        ASSERT((bp = (uint8_t *)diminuto_scattergather_segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &address, sizeof(address));
        ASSERT(diminuto_scattergather_record_segment_prepend(rp, sp) == sp);
        tp = sp;
 
        ASSERT((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(streamport))) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == sizeof(streamport));
        ASSERT((bp = (uint8_t *)diminuto_scattergather_segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &streamport, sizeof(streamport));
        ASSERT(diminuto_scattergather_record_segment_insert(tp, sp) == sp);
    
        ASSERT(diminuto_scattergather_record_enumerate(rp) == 5);
        ASSERT(diminuto_scattergather_record_measure(rp) > 0);
        ASSERT(diminuto_scattergather_record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        fflush(stderr);
    }

    {
        diminuto_scattergather_segment_t * sp;
        diminuto_ipv4_t * addressp;
        diminuto_port_t * portp;
        size_t total;
        int socket;
        ssize_t length;
        int status;

        /* Write Stream (more than once) */

        TEST();

        ASSERT((sp = diminuto_scattergather_record_segment_head(rp)) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == sizeof(*addressp));
        ASSERT((addressp = (diminuto_ipv4_t *)diminuto_scattergather_segment_payload_get(sp)) != (diminuto_ipv4_t *)0);

        ASSERT((sp = diminuto_scattergather_record_segment_next(rp, sp)) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == sizeof(*portp));
        ASSERT((portp = (diminuto_port_t *)diminuto_scattergather_segment_payload_get(sp)) != (diminuto_port_t *)0);

        ASSERT((total = diminuto_scattergather_record_measure(rp)) > 0);
        ASSERT((socket = diminuto_ipc4_stream_consumer(*addressp, *portp)) >= 0);
        ASSERT((length = diminuto_scattergather_record_write(socket, rp)) == total);
        ASSERT((length = diminuto_scattergather_record_write(socket, rp)) == total);
        ASSERT((length = diminuto_scattergather_record_write(socket, rp)) == total);
        ASSERT((length = diminuto_scattergather_record_write(socket, rp)) == total);
        ASSERT(diminuto_ipc_close(socket) >= 0);

        status = 2;
        ASSERT(diminuto_reaper_reap_generic(streampid, &status, 0) == streampid);
        ASSERT(WIFEXITED(status));
        ASSERT(WEXITSTATUS(status) == 0);

        STATUS();
    }

    {
        uint8_t * bp;
        diminuto_scattergather_segment_t * sp;
        diminuto_scattergather_segment_t * tp;
        diminuto_ipv4_t address;
        diminuto_port_t port;
        ssize_t length;

        /* Address DatagramPort Length Payload Checksum */

        TEST();

        ASSERT((sp = diminuto_scattergather_segment_allocate(&pool, sizeof(datagramport))) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == sizeof(datagramport));
        ASSERT((bp = (uint8_t *)diminuto_scattergather_segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &datagramport, sizeof(streamport));
        ASSERT((tp = diminuto_scattergather_record_segment_head(rp)) != (diminuto_scattergather_segment_t *)0);
        ASSERT((tp = diminuto_scattergather_record_segment_next(rp, tp)) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_record_segment_replace(tp, sp) == tp);
        diminuto_scattergather_segment_free(&pool, tp);
    
        ASSERT(diminuto_scattergather_record_enumerate(rp) == 5);
        ASSERT(diminuto_scattergather_record_measure(rp) > 0);
        ASSERT(diminuto_scattergather_record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        fflush(stderr);
    }

    {
        diminuto_scattergather_segment_t * sp;
        diminuto_ipv4_t * addressp;
        diminuto_port_t * portp;
        size_t total;
        int socket;
        ssize_t length;
        int status;

        /* Send Datagram (more than once) */

        TEST();

        ASSERT((sp = diminuto_scattergather_record_segment_head(rp)) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == sizeof(*addressp));
        ASSERT((addressp = (diminuto_ipv4_t *)diminuto_scattergather_segment_payload_get(sp)) != (diminuto_ipv4_t *)0);

        ASSERT((sp = diminuto_scattergather_record_segment_next(rp, sp)) != (diminuto_scattergather_segment_t *)0);
        ASSERT(diminuto_scattergather_segment_length_get(sp) == sizeof(*portp));
        ASSERT((portp = (diminuto_port_t *)diminuto_scattergather_segment_payload_get(sp)) != (diminuto_port_t *)0);

        ASSERT((total = diminuto_scattergather_record_measure(rp)) > 0);
        ASSERT((socket = diminuto_ipc4_datagram_peer(0)) >= 0);
        ASSERT((length = diminuto_scattergather_record_send_generic(socket, rp, *addressp, *portp)) == total);
        ASSERT((length = diminuto_scattergather_record_send_generic(socket, rp, *addressp, *portp)) == total);
        ASSERT((length = diminuto_scattergather_record_send_generic(socket, rp, *addressp, *portp)) == total);
        ASSERT(diminuto_ipc_close(socket) >= 0);

        status = 3;
        ASSERT(diminuto_reaper_reap_generic(datagrampid, &status, 0) == datagrampid);
        ASSERT(WIFEXITED(status));
        ASSERT(WEXITSTATUS(status) == 0);

        STATUS();
    }

    {
        diminuto_scattergather_segment_t * sp;

        TEST();

        diminuto_scattergather_record_free(&pool, rp);
        ASSERT(diminuto_scattergather_record_enumerate(&pool) > 0);
        ASSERT(diminuto_scattergather_record_measure(&pool) == 0);
        diminuto_scattergather_pool_fini(&pool);
        ASSERT(diminuto_scattergather_record_enumerate(&pool) == 0);
        ASSERT(diminuto_scattergather_record_measure(&pool) == 0);
        ASSERT(diminuto_buffer_log() > 0);
        diminuto_buffer_fini();
        ASSERT(diminuto_buffer_log() == 0);

        STATUS();
    }

    EXIT();
}
