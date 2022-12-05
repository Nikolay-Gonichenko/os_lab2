#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <fcntl.h>

#define file_proc "/proc/vma_info"


int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Incorrect amount of arguments \n");
		return 1;
	}

	char *c;
	int pid = strtol(argv[1], &c, 10);
	if (*c != '\0' || pid < 0) {
		fprintf(stderr, "pid must be integer and more than 0 \n");
		return 1;
	}

	int fd = open(file_proc, O_RDWR);
	char inbuf[4096];
	sprintf(inbuf, "%s", argv[1]);
	write(fd, inbuf, 17);
	lseek(fd, 0, SEEK_SET);
	char outbuf[8192];
	read(fd, outbuf, 8192);
	puts(outbuf);
	
	return 0;
}
