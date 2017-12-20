#include <linux/module.h>

int init_module(void)
{
    printk("%s Enter\n",__FUNCTION__);
    return 0;
}

void cleanup_module(void)
{
    printk("%s Enter\n",__FUNCTION__);
}