
#include <linux/module.h>
#include <linux/slab.h>


MODULE_AUTHOR("David Xie");
MODULE_LICENSE("Dual BSD/GPL");

static int leak_func(void)
{
    char *p;
    char mark[] = "memory leak test";
    p = kmalloc(sizeof(mark) , GFP_KERNEL);

    if (!p) {
        printk ("krnleak can I t allocate rnernory\n ") ;
        return -ENOMEM;
    }
    
    printk("pointer to the allocated memory: %p\n",p);
    memcpy(p , &mark, sizeof(mark));
    if (!p)
        kfree(p) ;
    return 0;
}

static int __init kmleak_init(void)
{
    leak_func() ;
    return 0;
}

static void __exit kmleak_exit(void)
{
    return;
}

module_init(kmleak_init);
module_exit(kmleak_exit);
