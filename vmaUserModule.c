#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <fcntl.h>

#include "common.h"

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
	if (fd == -1) {
		fprintf(stderr, "Proc file doesn't exist \n");
		return 1;
	}
	
	write(fd, &pid, sizeof(int));
	lseek(fd, 0, SEEK_SET);
	struct my_vma vmas[10];
	read(fd, vmas, sizeof(struct my_vma)*10);
	for(size_t i = 0; i<10; i+=1){
		printf("VM_Area number %d\n", vmas[i].number);
		printf("start is %ld\n", vmas[i].start);
		printf("end is %ld\n", vmas[i].end);
		printf("flags are %ld\n", vmas[i].flags);
		printf("\n");
	}
	
	return 0;
}
