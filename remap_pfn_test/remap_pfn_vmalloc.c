#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>

#define BUF_SIZE (32*PAGE_SIZE)

static void *kbuff;

static int remap_pfn_open(struct inode *inode, struct file *file)
{
	struct mm_struct *mm = current->mm;
/*
    client: remap_pfn_test_ (710)
    code  section: [0x10000   0x6ad28]
    data  section: [0x7b000   0x7bf68]
    brk   section: s: 0x7d000, c: 0x9f000
    mmap  section: s: 0xb6fa2000
    stack section: s: 0xbecece90
    arg   section: [0xbececf6f   0xbececf82]
    env   section: [0xbececf82   0xbececfe9]
*/
	printk("client: %s (%d)\n", current->comm, current->pid);
	printk("code  section: [0x%lx   0x%lx]\n", mm->start_code, mm->end_code);
	printk("data  section: [0x%lx   0x%lx]\n", mm->start_data, mm->end_data);
	printk("brk   section: s: 0x%lx, c: 0x%lx\n", mm->start_brk, mm->brk);
	printk("mmap  section: s: 0x%lx\n", mm->mmap_base);
	printk("stack section: s: 0x%lx\n", mm->start_stack);
	printk("arg   section: [0x%lx   0x%lx]\n", mm->arg_start, mm->arg_end);
	printk("env   section: [0x%lx   0x%lx]\n", mm->env_start, mm->env_end);

	return 0;
}

static int remap_pfn_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long virt_start = (unsigned long)kbuff + (unsigned long)(vma->vm_pgoff << PAGE_SHIFT);
	unsigned long pfn_start = (unsigned long)vmalloc_to_pfn((void *)virt_start);
	unsigned long size = vma->vm_end - vma->vm_start;
	int ret = 0;
	unsigned long vmstart = vma->vm_start;
	int i = 0;

/*
    phy: 0x8dc40000, offset: 0x0, size: 0x10000   
    
    比如用 vmalloc 分配128KB的内存，vmalloc计算发现需要分配32个page，然后会调用32次alloc_page()，
    每次从伙伴系统分配1个page，每分配一个page就将该page映射到准备好的连续的虚拟地址上，
    当然也就无法保证这些page之间对应的物理页帧的连续性
*/
	printk("phy: 0x%lx, offset: 0x%lx, size: 0x%lx\n", pfn_start << PAGE_SHIFT, offset, size);

	while (size > 0) {
		ret = remap_pfn_range(vma, vmstart, pfn_start, PAGE_SIZE, vma->vm_page_prot);
		if (ret) {
			printk("%s: remap_pfn_range failed at [0x%lx  0x%lx]\n",
				__func__, vmstart, vmstart + PAGE_SIZE);
			ret = -ENOMEM;
			goto err;
		} else
			printk("%s: map 0x%lx (0x%lx) to 0x%lx , size: 0x%lx, number: %d\n", __func__, virt_start,
				pfn_start << PAGE_SHIFT, vmstart, PAGE_SIZE, ++i);

		if (size <= PAGE_SIZE)
			size = 0;
		else {
			size -= PAGE_SIZE;
			vmstart += PAGE_SIZE;
			virt_start += PAGE_SIZE;
			pfn_start = vmalloc_to_pfn((void *)virt_start);
		}
	}

	return 0;
err:
	return ret;
}

static const struct file_operations remap_pfn_fops = {
	.owner = THIS_MODULE,
	.open = remap_pfn_open,
	.mmap = remap_pfn_mmap,
};

static struct miscdevice remap_pfn_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "remap_pfn",
	.fops = &remap_pfn_fops,
};

static int __init remap_pfn_init(void)
{
	int ret = 0;

	kbuff = vmalloc(BUF_SIZE);
	if (!kbuff) {
		ret = -ENOMEM;
		goto err;
	}

	ret = misc_register(&remap_pfn_misc);
	if (unlikely(ret)) {
		pr_err("failed to register misc device!\n");
		goto err;
	}

	return 0;

err:
	return ret;
}

static void __exit remap_pfn_exit(void)
{
	misc_deregister(&remap_pfn_misc);
	vfree(kbuff);
}

module_init(remap_pfn_init);
module_exit(remap_pfn_exit);
MODULE_LICENSE("GPL");
