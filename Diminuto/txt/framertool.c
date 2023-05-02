    FILE * stream:
    diminuto_mux_t multiplexor;
    diminuto_ticks_t timeout = 1000000000;
    int fds;
    int fd;
    int datum;

    stream = fopen(path, "r");

    diminuto_mux_init(&multiplexor);

    diminuto_mux_register_read(&multiplexor, fileno(stream));

    fds = diminuto_mux_wait(&multiplexor, timeout);
    while ((fds--) > 0) {
        fd = diminuto_mux_ready_read(&multiplexor);
        if (fd == fileno(stream)) {
            do {
                datum = fgetc(stream);
                if (datum == EOF) {
                    diminuto_mux_unregister_read(&multiplexor, fileno(stream));
                    fclose(stream);
                    break;
                } else {
                    process(datum);
                }
            } while (diminuto_file_ready(stream) > 0);
        }
    }
