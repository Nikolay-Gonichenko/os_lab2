#include <stdatomic.h>


struct my_inode {
	//atomic64_t version;
	long number;
	unsigned long long size;
};

struct file_path {
	size_t size;
	char *path;
};
