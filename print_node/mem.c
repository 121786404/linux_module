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

#ifndef CONFIG_NEED_MULTIPLE_NODES
#include <linux/mmzone.h>
#else
#include <asm/mmzone.h>
#endif

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Guanjun He, gjhe@suse.com");


static struct proc_dir_entry *proc_entry;

struct mem_ret_data
{
	unsigned long free;
	unsigned long inactive;
	unsigned long inactive_avl;
	unsigned long active;
	unsigned long active_avl;
	unsigned long noaction;
	unsigned long reserved;
	unsigned long suspend;
	unsigned long slab;
	unsigned long swap;
};

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
#ifdef CONFIG_PAGE_STATES
	if (mapping->mlocked)
		return 0;
#endif
	//3.
	//we will do more work about this section, page_count() is not safe to call,we didn't lock it.
	//if (page_mapcount(page) + 1 != page_count(page))
	//        return 0;

	return 1;
}

struct mem_ret_data avail_memory(void)
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
        //struct list_head list_iter;
        struct page *tmp1=NULL ;
        //struct page *tmp2=NULL ;
        //struct page *sh=NULL;
        struct mem_ret_data ret;
	
#ifndef CONFIG_NEED_MULTIPLE_NODES
	for(i=0;i<contig_page_data.nr_zones;i++)
	{
		zone_iter = contig_page_data.node_zones[i];
#else
	for(k=0;k<MAX_NUMNODES;k++)
	{
		struct pglist_data *node_iter = node_data[k];
		if(!node_iter)
			continue;
		for(l=0;l<node_iter->nr_zones;l++)
		{
			zone_iter = node_iter->node_zones[l];
#endif
		for(j=0;j<zone_iter.present_pages;j++)
		//for(j=0;j<zone_iter.spanned_pages;j++)
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
				swap++;
			else if(PageReserved(tmp1))
			{
				reserved++;
			}
			/*
			version 2.6.16 - 2.6.21 work ok
			version 2.6.22 - 2.6.26 do not have the 2 macros
			*/
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22))
			else if(PageNosave(tmp1) || PageNosaveFree(tmp1))
			{
				suspend++;
			}
#endif
			else if(PageSlab(tmp1))
			{
				slab++;
			}
			else
			{
				free_page_num++;
			}

		}
#ifndef CONFIG_NEED_MULTIPLE_NODES
	}
#else
		}
	}
#endif
	ret.free = free_page_num;
	ret.active = total_page_num_active;
	ret.active_avl = avail_page_num_active;
	ret.inactive = total_page_num_inactive;
	ret.inactive_avl = avail_page_num_inactive;
	ret.noaction = avail_page_num_noneed_actions;
	ret.reserved = reserved;
	ret.suspend = suspend;
	ret.slab =  slab;
	ret.swap =  swap;

	return ret;
}


ssize_t mem_write( struct file *filp, const char __user *buff,unsigned long len, void *data )
{
	return 1;
}

int mem_read( char *page, char **start, off_t off,
		 int count, int *eof, void *data )
{
	int len;
	struct mem_ret_data tmp_ret;

	if (off > 0)
	{
		*eof = 1;
		return 0;
	}
	tmp_ret = avail_memory();
	len = sprintf(page, "%8s %8s %8s %8s %8s %6s %8s %8s %6s %8s %6s\n\
%8lu %8lu %8lu %8lu %8lu %6lu %8lu %8lu %6lu %8lu %6lu\n",\
		"free","inac", "inac_re", "ac", "ac_re", "noac", "reserved", "suspend", "slab", "t_avail", "swap",\
		tmp_ret.free, tmp_ret.inactive, tmp_ret.inactive_avl,\
		tmp_ret.active, tmp_ret.active_avl, tmp_ret.noaction, \
		tmp_ret.reserved, tmp_ret.suspend, tmp_ret.slab, tmp_ret.free+tmp_ret.inactive_avl+tmp_ret.active_avl, tmp_ret.swap);

	return len;
}

static int mc_init(void)
{
	int ret = 0;
	proc_entry = create_proc_entry("mem", 0444, NULL);
	if (proc_entry == NULL) 
	{
		ret = -ENOMEM;
		printk(KERN_INFO "mem: Couldn't create proc entry\n");
	} 
	else 
	{
		proc_entry->read_proc = mem_read;
		proc_entry->write_proc = mem_write;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
		proc_entry->owner = THIS_MODULE;
#endif
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,29))
		//no need we do this manually,kernel keep and supply it at registration time.
		//proc_entry->proc_fops->owner
#endif
		printk(KERN_INFO "mem: Module loaded.\n");
	}
	return ret;
}

void mc_exit(void)
{
//2.6.26/2.6.27 do not have global "proc_root" defined
//so,it's better to use proc_entry->parent,that is defined also in 2.6.16
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25))
	remove_proc_entry("mem", proc_entry->parent);
#else
	remove_proc_entry("mem", &proc_root);
#endif
	//remove_proc_entry("mem", proc_entry->parent);
	printk(KERN_INFO "mem: Module unloaded.\n");
	return;
}

module_init( mc_init );
module_exit( mc_exit );

