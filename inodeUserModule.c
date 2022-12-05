#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <fcntl.h>

#define file_proc "/proc/inode_info"


int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Incorrect amount of arguments \n");
		return 1;
	}

	char *path = argv[1];
	int path_size = strlen(path);

	int fd = open(file_proc, O_RDWR);
	char inbuf[4096];
	sprintf(inbuf, "%s", path);
	write(fd, inbuf, path_size);
	lseek(fd, 0, SEEK_SET);
	char outbuf[4096];
	read(fd, outbuf, 4096);
	puts(outbuf);
	
	return 0;
}
