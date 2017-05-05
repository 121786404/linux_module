/*************************************************************************
    > File Name: drivers/input/kobj_test.c
    > Author: chengdong
    > Mail: zchengdongdong@gmail.com 
    > Created Time: Tue 04 Dec 2012 10:50:27 AM CST
 ************************************************************************/

#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/stat.h>
#include <linux/delay.h>


/*
# echo xxx  > /sys/kobject_test/kobj_config 
# cat /sys/kobject_test/kobj_config
*/

MODULE_AUTHOR("David Xie");
MODULE_LICENSE("Dual BSD/GPL");

void obj_test_release(struct kobject *kobject);
ssize_t kobj_test_show(struct kobject *kobject, struct attribute *attr, char *buf);
struct kobject kobj;

struct attribute test_attr = {
		.name = "kobj_config",
		.mode = S_IRWXUGO,
};

static struct attribute *def_attrs[] = {
		&test_attr,
		NULL,
};

ssize_t kobj_test_store(struct kobject *kobject, struct attribute *attr,const char *buf, size_t count)
{
		printk("havestore\n");
		printk("write:%s\n", buf);
		return (ssize_t)count;
}

struct sysfs_ops obj_test_sysops = {
		.show = kobj_test_show,
		.store = kobj_test_store,
};

struct kobj_type ktype = {
		.release = obj_test_release,
		.sysfs_ops = &obj_test_sysops,
		.default_attrs = def_attrs,
};

void obj_test_release(struct kobject *kobject)
{
	printk("eric_test:release.\n");
}

ssize_t kobj_test_show(struct kobject *kobject, struct attribute *attr, char *buf)
{
		printk("have show.\n");
		printk("attrname:%s.\n", attr->name);
		sprintf(buf, "%s sheldon\n", attr->name);
		return strlen(attr->name) + 2;
}

static int kobj_test_init(void)
{
    int ret;
	printk("kobject test init.\n");
	/*kobject_init_and_add(&kobj, &ktype, NULL, "kobject_test");*/
	kobject_init(&kobj, &ktype);
	ret = kobject_add(&kobj, NULL, "kobject_test");
	if (ret)
		return ret;
		
	return 0;
}

static void kobj_test_exit(void)
{
		printk("kobject test exit.\n");
		kobject_del(&kobj);
}
module_init(kobj_test_init);
module_exit(kobj_test_exit);
