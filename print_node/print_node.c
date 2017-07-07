#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>

#include<linux/init.h>
#include<linux/module.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/bitops.h>
#include <linux/cache.h>
#include <linux/threads.h>
#include <linux/numa.h>
#include <linux/init.h>
#include <linux/seqlock.h>
#include <linux/nodemask.h>
#include <asm/atomic.h>
#include <asm/page.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/buffer_head.h>
#include <linux/genhd.h>
#include <linux/version.h>
#include <linux/mmzone.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Guanjun He, gjhe@suse.com");

int mem_page_reclaimable(struct page* page)
{
	struct address_space *mapping=NULL;
	//1.directly flags check
	if (PageDirty(page) 
		|| PageReserved(page) 
		|| PageWriteback(page) 
		|| PageLocked(page) 
		|| PagePrivate(page)/*has buffers*/ 
		|| !PageUptodate(page) /*reading data from disk*/
		|| !PageLRU(page) /*kernel use or ...*/
		|| (PageAnon(page) && !PageSwapCache(page)))
		return 0;
	//2.
	mapping = page_mapping(page);
	if (mapping == NULL)/*no file backend*/
		return 1;

	//3.
	//we will do more work about this section, page_count() is not safe to call,we didn't lock it.
	//if (page_mapcount(page) + 1 != page_count(page))
	//        return 0;

	return 1;
}

static int __init print_node_setup(void)
{
    int i=0;
    int j=0;
    int k=0;
    int l=0;
    //unsigned long flags;
    unsigned long free_page_num=0;
    unsigned long avail_page_num_inactive=0;
    unsigned long avail_page_num_active=0;
    unsigned long total_page_num_inactive=0;
    unsigned long total_page_num_active=0;
    unsigned long avail_page_num_noneed_actions=0;
    unsigned long reserved=0;
    unsigned long suspend=0;
    unsigned long slab=0;
    unsigned long swap=0;
    struct zone zone_iter;
    struct page *tmp1=NULL ;

	for(i = 0;i < contig_page_data.nr_zones;i ++)
	{
		zone_iter = contig_page_data.node_zones[i];
		for(j = 0;j < zone_iter.present_pages;j ++)
		{
			tmp1 = pfn_to_page(zone_iter.zone_start_pfn+j);
			if(!tmp1)
				continue;

			if(PageLRU(tmp1))
			{
				if(PageSwapCache(tmp1))
					swap++;

				if(PageActive(tmp1))
				{
					total_page_num_active++;
					if(mem_page_reclaimable(tmp1))
					{
						avail_page_num_active++;
					}
				}
				else
				{
					total_page_num_inactive++;
					if(mem_page_reclaimable(tmp1))
					{
						avail_page_num_inactive++;
					}
					if(!(PageLocked(tmp1) || PageWriteback(tmp1) || PageReserved(tmp1) || PageDirty(tmp1) ||
						page_mapped(tmp1) /*|| (page_count(tmp1) !=2)*/ ))
					{
						avail_page_num_noneed_actions++;
					}
				}
			}
			else if(PageSwapCache(tmp1))
			{
				swap++;
			}
			else if(PageReserved(tmp1))
			{
				reserved++;
			}
			else if(PageSlab(tmp1))
			{
				slab++;
			}
			else
			{
				free_page_num++;
			}
		}
	}

#if 0
   [  477.122512] free_page_num                :   10608
   [  477.122715] total_page_num_active        :     511
   [  477.122828] avail_page_num_active        :     381
   [  477.123549] total_page_num_inactive      :     219
   [  477.123706] avail_page_num_inactive      :     218
   [  477.124077] avail_page_num_noneed_actions:     218
   [  477.124251] reserved                     :    3038
   [  477.124380] suspend                      :       0
   [  477.124559] slab                         :    2008
   [  477.124760] swap                         :       0
#endif

   printk(KERN_INFO"free_page_num                :%8lu\n",free_page_num);
   printk(KERN_INFO"total_page_num_active        :%8lu\n",total_page_num_active);
   printk(KERN_INFO"avail_page_num_active        :%8lu\n",avail_page_num_active);
   printk(KERN_INFO"total_page_num_inactive      :%8lu\n",total_page_num_inactive);
   printk(KERN_INFO"avail_page_num_inactive      :%8lu\n",avail_page_num_inactive);
   printk(KERN_INFO"avail_page_num_noneed_actions:%8lu\n",avail_page_num_noneed_actions);
   printk(KERN_INFO"reserved                     :%8lu\n",reserved);
   printk(KERN_INFO"suspend                      :%8lu\n",suspend);
   printk(KERN_INFO"slab                         :%8lu\n",slab);
   printk(KERN_INFO"swap                         :%8lu\n",swap);
}

static void __exit print_node_cleanup(void)
{
    printk(KERN_INFO"Goodbye!\n");
}

module_init(print_node_setup);
module_exit(print_node_cleanup);

