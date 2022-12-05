#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/proc_fs.h> 
#include <linux/uaccess.h> 
#include <linux/version.h> 
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/time.h>
#include <linux/path.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0) 
#define HAVE_PROC_OPS 
#endif 
 
#define procfs_name "inode_info" 
#define PROCFS_MAX_SIZE 4096
 
static struct proc_dir_entry *our_proc_file;

static struct inode* my_inode; 
 
static ssize_t procfile_read(struct file *filePointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) 
{ 
	int len = 0;
	char buf[PROCFS_MAX_SIZE];
	if (my_inode == NULL) {
		len += sprintf(buf, "Error. No inode with this path \n");
	} else {
		len += sprintf(buf+len, "size is %llu\n", my_inode->i_size);
		len += sprintf(buf+len, "version is %ld\n", my_inode->i_version);
		len += sprintf(buf+len, "id of owner is %ld\n", my_inode->i_uid);
		//struct timespec ts = my_inode->i_mtime;
		//len += sprintf(buf+len, "last changing time is %li.%09li \n", ts.tv_sec , ts.tv_nsec); //+
	}

	if(copy_to_user(buffer, buf, len)) {
		return -EFAULT;
	}

	*offset = len;
	return len;
};

static ssize_t procfile_write(struct file *file, const char __user *buff, 
                             size_t len, loff_t *off) 
{ 
	char file_path[PROCFS_MAX_SIZE];
	if (copy_from_user(file_path, buff, len)) {
		return -EFAULT;
	}
	pr_info("path %s \n", file_path);
	struct path path;
	kern_path(file_path, LOOKUP_FOLLOW, &path);
	
	my_inode = path.dentry->d_inode;

	*off = strlen(file_path);
	
	return strlen(file_path);
	
};
 
#ifdef HAVE_PROC_OPS 
static const struct proc_ops proc_file_fops = { 
	.proc_read = procfile_read,
	.proc_write = procfile_write,
}; 
#else 
static const struct file_operations proc_file_fops = { 
	.read = procfile_read,
	.write = procfile_write,
};  
#endif 
 
static int __init proc_init(void) 
{ 
    our_proc_file = proc_create(procfs_name, 0666, NULL, &proc_file_fops); 
    if (NULL == our_proc_file) { 
        proc_remove(our_proc_file); 
        pr_alert("Error:Could not initialize /proc/%s\n", procfs_name); 
        return -ENOMEM; 
    }

    pr_info("/proc/%s created \n", procfs_name);
  
    return 0; 
}; 
 
static void __exit proc_exit(void) 
{ 
    proc_remove(our_proc_file); 
    pr_info("/proc/%s removed\n", procfs_name); 
}; 
 
module_init(proc_init); 
module_exit(proc_exit); 
 
MODULE_LICENSE("GPL");
