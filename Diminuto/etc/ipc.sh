#!/bin/bash
unittest-ipc || exit 1
unittest-ipc4 || exit 2
unittest-ipc6 || exit 3
unittest-mux4 || exit 4
unittest-mux6 || exit 5
unittest-mux-eventloop || exit 6
unittest-poll4 || exit 7
unittest-poll6 || exit 8
unittest-poll-eventloop || exit 9
exit 0
