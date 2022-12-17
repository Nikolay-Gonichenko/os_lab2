#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/proc_fs.h> 
#include <linux/uaccess.h> 
#include <linux/version.h> 
#include <linux/fs.h>
#include <linux/namei.h>
#include <linux/time.h>
#include <linux/path.h>
#include <linux/slab.h>

#include "common_inode.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0) 
#define HAVE_PROC_OPS 
#endif 
 
#define procfs_name "inode_info" 

static struct inode* my_inode;
static struct proc_dir_entry *our_proc_file; 
 
static ssize_t procfile_read(struct file *filePointer, char *buffer, 
                             size_t buffer_length, loff_t *offset) 
{ 
	int len = 0;
	struct my_inode m_inode;
	if (my_inode == NULL) {
	} else {
		len = sizeof(struct my_inode);
		m_inode.number =  my_inode->i_ino;
		m_inode.size =  my_inode->i_size;
	}

	if(copy_to_user(buffer, &m_inode, sizeof(struct my_inode))) {
		return -EFAULT;
	}
	*offset = len;
	return len;
}

static ssize_t procfile_write(struct file *file, const char *buff, 
                             size_t len, loff_t *off) 
{ 
	struct file_path fp;
	if (copy_from_user(&fp, buff, len)) {
		return -EFAULT;
	}
	pr_info("path %s \n", fp.path);
	struct path path;
	kern_path(fp.path, LOOKUP_FOLLOW, &path);
	
	my_inode = path.dentry->d_inode;

	*off = sizeof(struct file_path);
	
	return sizeof(struct file_path);
	
}
 
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
