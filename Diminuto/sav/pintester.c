#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {

	{
		int fd;
		ssize_t bb;
		fd = open("/sys/class/gpio/gpio16/direction", O_WRONLY, 0);
		if (fd < 0) { perror("open"); exit(1); }
		bb = write(fd, "in", sizeof("in"));
		if (bb != sizeof("in")) { perror("write"); exit(1); }
		fd = close(fd);
		if (fd < 0) { perror("close"); exit(1); }
	}

	{
		int rc;
		FILE * fp;
		fp = fopen("/sys/class/gpio/gpio16/direction", "w");
		if (fp == (FILE *)0) { perror("fopen"); exit(1); }
		rc = fputs("in", fp);
		if (rc == EOF) { perror("fputs"); exit(1); }
		rc = fclose(fp);
		if (rc == EOF) { perror("fclose"); exit(1); }
	}

	{
		int fd;
		ssize_t bb;
		fd = open("/sys/class/gpio/gpio16/direction", O_WRONLY, 0);
		if (fd < 0) { perror("open"); exit(1); }
		bb = write(fd, "out", sizeof("out"));
		if (bb != sizeof("out")) { perror("write"); exit(1); }
		fd = close(fd);
		if (fd < 0) { perror("close"); exit(1); }
	}

	{
		int rc;
		FILE * fp;
		fp = fopen("/sys/class/gpio/gpio16/direction", "w");
		if (fp == (FILE *)0) { perror("fopen"); exit(1); }
		rc = fputs("out", fp);
		if (rc == EOF) { perror("fputs"); exit(1); }
		rc = fclose(fp);
		if (rc == EOF) { perror("fclose"); exit(1); }
	}

	return 0;
}
