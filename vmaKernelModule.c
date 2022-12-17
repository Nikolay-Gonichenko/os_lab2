#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/proc_fs.h> 
#include <linux/uaccess.h> 
#include <linux/version.h> 
#include <linux/pid.h>
#include <linux/fs.h>
#include <linux/mm_types.h>

#include "common.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0) 
#define HAVE_PROC_OPS 
#endif 
 
#define procfs_name "vma_info" 

static int pid = 1; 
static struct proc_dir_entry *our_proc_file;
 
static ssize_t procfile_read(struct file *filePointer, char *buffer, 
                             size_t buffer_length, loff_t *offset) 
{ 
	
	struct task_struct *task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
	if (task == NULL) {
	} else {
		struct vm_area_struct *vmlist;
		int count = 0;
		struct my_vma vmas[10];
		for(vmlist = task->mm->mmap; vmlist!=NULL && count<10; vmlist=vmlist->vm_next) {
			vmas[count].number = count;
			vmas[count].start = vmlist->vm_start;
			vmas[count].end = vmlist->vm_end;
			vmas[count].flags = vmlist->vm_flags;
			count++;
		}
		if(copy_to_user(buffer, vmas, sizeof(struct my_vma)*10)) {
			pr_info("error \n");
			return -EFAULT;
		}
	}

	

	*offset = sizeof(struct my_vma)*10;
	return sizeof(struct my_vma)*10;
};

static ssize_t procfile_write(struct file *file, const char *buff, 
                             size_t len, loff_t *off) 
{ 
	if (copy_from_user(&pid, buff, len)) {
		return -EFAULT;
	}

	*off = sizeof(int);
	
	return sizeof(int);
	
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
    struct proc_dir_entry *our_proc_file = proc_create(procfs_name, 0666, NULL, &proc_file_fops); 
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
