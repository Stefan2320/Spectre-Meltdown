#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

/*
========== Module for placing a secret in the kernel ==========
To achieve this goal we create a kernel module. We first place
the secrets address in the kernel message buffer (printk), which we can
access in the user space.This is so we know where the target is.

We need to cache our secret, this is done by creating a proc/entry
which can be accessed as a user and thus interact with the kernel module.

When we read from the entry, we will load the secret data => it will be
cached.
*/

/* for implementation details of structs and so: /usr/include/linux */
static char secret[7] = {'P', 'W', 'N', 't', 'i', 'm', 'e'};
static struct proc_dir_entry *secret_entry;
static char *secret_buffer;

/* inode (defined in fs.h) */
/* Read about seq_open, single_open is a simplified version of it */
static int test_proc_open(struct inode *inode, struct file *file)
{
/* MACRO from linux/version.h */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4, 0, 0)
  return single_open(file, NULL, PDE(inode)->data);
#else
  return single_open(file, NULL, PDE_DATA(inode));
#endif
}

/* the function read_proc will give to the user space the data that we want to export from the kernel space. */
static ssize_t read_proc(struct file *filp, char *buffer, size_t length, loff_t *offset)
{

  memcpy(secret_buffer, &secret, 7);
  return 7;
}

/* llseek - change r/w position/offset in a file;
 release - invoked when file structure is released *optional* */
static const struct file_operations test_proc_fops =
    {
        .owner = THIS_MODULE,
        .open = test_proc_open,
        .read = read_proc,
        .llseek = seq_lseek,
        .release = single_release,
};

/*Mark as initialzed data/initialization functions. Is used only during the initialization phase and free up used memory resources after */
static __init int test_proc_init(void)
{
  /*All printk() messages are printed to the kernel log buffer, which is a ring buffer exported to userspace through /dev/kmsg. The usual way to read it is using dmesg.*/
  printk("Secret data address is: %p\n", &secret);

  /*kmalloc() - guarantees that the pages are physically contiguous (and virtually contiguous)
  vmalloc() - only ensures that the pages are contiguous within the virtual address space.  */
  secret_buffer = (char *)vmalloc(7);

  /* proc_create_data(const char *name, umode_t mode,struct proc_dir_entry *parent,const struct file_operations *proc_fops,void *data);*/
  secret_entry = proc_create_data("secret_melt", 0444, NULL, &test_proc_fops, NULL);

  if (secret_entry)
    return 0;

  /*No memory*/
  return -ENOMEM;
}

static __exit void test_proc_cleanup(void)
{

  remove_proc_entry("secret_melt", NULL);
}

/* Driver initialization entry point */
module_init(test_proc_init);
/* Driver exit entry point */
module_exit(test_proc_cleanup);
