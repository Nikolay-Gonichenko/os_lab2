#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/proc_fs.h> 
#include <linux/uaccess.h> 
#include <linux/version.h> 
#include <linux/pid.h>
#include <linux/fs.h>
#include <linux/mm_types.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0) 
#define HAVE_PROC_OPS 
#endif 
 
#define procfs_name "vma_info" 
#define PROCFS_MAX_SIZE 8192
 
static struct proc_dir_entry *our_proc_file;

static int pid = 1; 
 
static ssize_t procfile_read(struct file *filePointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) 
{ 
	int len = 0;
	char buf[PROCFS_MAX_SIZE];
	
	struct task_struct *task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
	if (task == NULL) {
		len += sprintf(buf, "Error. No proccess with this pid \n");
	} else {
		struct vm_area_struct *vmlist;
		int count = 0;
		for(vmlist = task->mm->mmap; vmlist!=NULL && count<10; vmlist=vmlist->vm_next) {
			len += sprintf(buf+len, "VM_Area number %d\n", count);
			len += sprintf(buf+len, "start is %ld\n", vmlist->vm_start);
			len += sprintf(buf+len, "end is %ld\n", vmlist->vm_end);
			len += sprintf(buf+len, "flags are %ld\n", vmlist->vm_flags);
			len += sprintf(buf+len, "\n");
			count++;
		}
	}

	if(copy_to_user(buffer, buf, len)) {
		pr_info("error \n");
		return -EFAULT;
	}

	*offset = len;
	return len;
};

static ssize_t procfile_write(struct file *file, const char __user *buff, 
                             size_t len, loff_t *off) 
{ 
	char buffer[PROCFS_MAX_SIZE];
	if (copy_from_user(buffer, buff, len)) {
		return -EFAULT;
	}

	int num_of_digit;
	int read_digit;
	
	num_of_digit = sscanf(buffer, "%d", &read_digit);
	if (num_of_digit != 1) {
		return -EFAULT;
	}

	pid = read_digit;
	*off = strlen(buffer);
	
	return strlen(buffer);
	
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
