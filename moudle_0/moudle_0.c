#include <linux/module.h>

static int __init hello_init(void)
{
    printk("%s Enter\n",__FUNCTION__);
    return 0;
}

static void __exit hello_exit(void)
{
    printk("%s Enter\n",__FUNCTION__);
}

module_init(hello_init);
module_exit(hello_exit);
