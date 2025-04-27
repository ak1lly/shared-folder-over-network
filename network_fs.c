// network_fs.c

#include <linux/module.h>   
#include <linux/kernel.h>   
#include <linux/init.h>     

// load
static int __init network_fs_init(void)
{
    printk(KERN_INFO "test init!\n");
    return 0; // 0 means successful loading
}

// unload
static void __exit network_fs_exit(void)
{
    printk(KERN_INFO "text exit\n");
}

module_init(network_fs_init);
module_exit(network_fs_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("YourName");
MODULE_DESCRIPTION("A minimal network fs test module.");
