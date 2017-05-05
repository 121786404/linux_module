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
#include <linux/kobject.h>
#include <linux/delay.h>


/*
[root@vexpress share]# insmod kset_test.ko
kset test init.
Filter:kobj kset_b.
Name kobj kset_b.
uevent:kobj kset_b.
ACTION=add.
DEVPATH=/kset_a/kset_b.
SUBSYSTEM=kset_name.
*/
/*
#ls /sys/kset_a/kset_b
kobj_config   kobject_test  kset_c
*/

MODULE_AUTHOR("David Xie");
MODULE_LICENSE("Dual BSD/GPL");

struct kset *kset_p;
struct kset kset_p2;
struct kset kset_c;
struct kset kset_a;

void obj_test_release(struct kobject *kobject);
ssize_t kobj_test_show(struct kobject *kobject, struct attribute *attr, char *buf);
ssize_t kobj_test_store(struct kobject *kobject, struct attribute *attr,const char *buf, size_t count);

struct attribute test_attr = {
		.name = "kobj_config",
		.mode = S_IRWXUGO,
};

static struct attribute *def_attrs[] = {
		&test_attr,
		NULL,
};

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
	printk("[kobj_test:release.]\n");
}

ssize_t kobj_test_show(struct kobject *kobject, struct attribute *attr, char *buf)
{
		printk("have show.\n");
		printk("attrname:%s.\n", attr->name);
		sprintf(buf, "%sc\n", attr->name);
		return strlen(attr->name) + 2;
}

ssize_t kobj_test_store(struct kobject *kobject, struct attribute *attr,const char *buf, size_t count)
{
		printk("%s havestore %s\n",kobject->name,buf);
		return (ssize_t)count;
}

static int kset_filter(struct kset *kset, struct kobject *kobj)
{
	printk("Filter:kobj %s.\n", kobj->name);
	if(strcmp("kobj->name",kset_p2.kobj.name))
	{
		//printk("Filter:kset_p2\n");
		//return 0;
	}
	return 1;
}

static const char *kset_name(struct kset *kset, struct kobject *kobj)
{
	static char buf[20];
	printk("Name kobj %s.\n", kobj->name);
	sprintf(buf, "%s", "kset_name");
	return buf;
}

static int kset_uevent(struct kset *kset, struct kobject *kobj, struct kobj_uevent_env *env)
{
	int i = 0;
	printk("uevent:kobj %s.\n", kobj->name);
	while(i < env->envp_idx)
	{
		printk("%s.\n", env->envp[i]);
		i++;
	}
	return 0;
}

static struct kset_uevent_ops uevent_ops = {
	.filter = kset_filter,
	.name 	= kset_name,
	.uevent	= kset_uevent,
};

struct kobject kobj;
static int kset_test_init(void)
{
	int ret = 0;
	printk("kset test init.\n");
	/*创建并注册kset_p*/
	kset_p = kset_create_and_add("kset_a", &uevent_ops, NULL);

	/*在kset_a里面添加kset_b*/
	kobject_set_name(&kset_p2.kobj,"kset_b");
	kset_p2.kobj.kset = kset_p;
	kset_p2.kobj.parent = &kset_p->kobj;
	kset_p2.kobj.ktype = &ktype;
	ret = kset_register(&kset_p2);

	/*向kset_b中添加一个kobject*/
	kobject_init(&kobj, &ktype);
	ret = kobject_add(&kobj, &kset_p2.kobj, "kobject_test");
	if (ret)
		return ret;

	/*添加kset_c到kset_b*/
	kobject_set_name(&kset_c.kobj, "kset_c");
	kset_c.kobj.kset = &kset_p2;

	/*对于较新版本的内核,在注册kset之前,需要填充kset.kobj的ktype成员,否则注册不会成功*/
	kset_c.kobj.ktype = &ktype;
	ret = kset_register(&kset_c);

	if(ret)
		kset_unregister(kset_p);

	return 0;
}

static void kset_test_exit(void)
{
		printk("kset test exit.\n");
		kset_unregister(kset_p);
		kset_unregister(&kset_c);
}

module_init(kset_test_init);
module_exit(kset_test_exit);
