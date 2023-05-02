#include <stdio.h>
#include <stddef.h>

/**
 * Return the number of octets a FILE object has ready to read in its buffer.
 * The result before the first read of the FILE object or after the FILE
 * object has been closed is unpredictable.
 * @param fp points to the FILE object.
 * @return the number of octets ready or <0 if an error occurred.
 */
static inline ssize_t diminuto_file_ready(const FILE * stream)
{
    return (stream->_IO_read_end - stream->_IO_read_ptr);
}
