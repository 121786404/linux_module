#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>

// int direction = PCI_DMA_TODEVICE ;
// int direction = PCI_DMA_FROMDEVICE ;
static int direction = PCI_DMA_BIDIRECTIONAL;
//int direction = PCI_DMA_NONE;

static char *kbuf;
static dma_addr_t handle;
static size_t size = (10 * PAGE_SIZE);
static struct dma_pool *mypool;
static size_t pool_size = 1024;
static size_t pool_align = 8;

static void output (char *kbuf, dma_addr_t handle, size_t size, char *string)
{
    unsigned long diff;
    diff = (unsigned long)kbuf - handle;
    printk (KERN_INFO "kbuf=%12p, handle=%12p, size = %d\n", kbuf,(unsigned long *)handle, (int)size);
    printk (KERN_INFO "(kbuf-handle)= %12p, %12lu, PAGE_OFFSET=%12lu, compare=%lu\n",
            (void *)diff, diff, PAGE_OFFSET, diff - PAGE_OFFSET);
    strcpy (kbuf, string);
    printk (KERN_INFO "string written was, %s\n", kbuf);
}

static int __init my_init (void)
{

    /* dma_alloc_coherent method */

    printk (KERN_INFO "Loading DMA allocation test module\n");
    printk (KERN_INFO "\nTesting dma_alloc_coherent()..........\n\n");
    kbuf = dma_alloc_coherent (NULL, size, &handle, GFP_KERNEL);
    output (kbuf, handle, size, "This is the dma_alloc_coherent() string");
    dma_free_coherent (NULL, size, kbuf, handle);

    /* dma_map/unmap_single */
#if 0
    printk (KERN_INFO "\nTesting dma_map_single()................\n\n");
    kbuf = kmalloc (size, GFP_KERNEL);
    handle = dma_map_single (NULL, kbuf, size, direction);
    output (kbuf, handle, size, "This is the dma_map_single() string");
    dma_unmap_single (NULL, handle, size, direction);
    kfree (kbuf);
#endif
    /* dma_pool method */

    printk (KERN_INFO "\nTesting dma_pool_alloc()..........\n\n");
    mypool = dma_pool_create ("mypool", NULL, pool_size, pool_align, 0);
    kbuf = dma_pool_alloc (mypool, GFP_KERNEL, &handle);
    output (kbuf, handle, size, "This is the dma_pool_alloc() string");
    dma_pool_free (mypool, kbuf, handle);
    dma_pool_destroy (mypool);

    return 0;
}
static void __exit my_exit (void)
{
    printk (KERN_INFO "Module Unloading\n");
}

module_init (my_init);
module_exit (my_exit);

MODULE_AUTHOR ("Team Veda");
MODULE_DESCRIPTION ("DMA interface test");
MODULE_LICENSE ("GPL v2");