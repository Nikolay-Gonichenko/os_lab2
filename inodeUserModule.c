#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <fcntl.h>

#include "common_inode.h"

#define file_proc "/proc/inode_info"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Incorrect amount of arguments \n");
		return 1;
	}

	char *path = argv[1];
	int path_size = strlen(path);

	int fd = open(file_proc, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Proc file doesn't exist \n");
		return 1;
	}
	struct file_path fp;
	fp.size = path_size;
	fp.path = path;
	write(fd, &fp, sizeof(struct file_path));
	lseek(fd, 0, SEEK_SET);
	struct my_inode m_inode;
	read(fd, &m_inode, sizeof(struct my_inode));
	printf("\nInode info:\n");
	printf("size is %llu\n", m_inode.size);
	printf("number is %llu\n", m_inode.number);
	return 0;
}
