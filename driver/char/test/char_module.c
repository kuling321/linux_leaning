/*
* drivers/char/timer_test/sunxi_timer_test.c
*
* Copyright(c) 2013-2015 Allwinnertech Co., Ltd.
*      http://www.allwinnertech.com
*
* Author: liugang <liugang@allwinnertech.com>
*
* sunxi timer test driver
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/


#define DEV_NAME "test_module"
static dev_t g_devid;
static struct cdev *g_ptest_cdev; /* in /proc/devices */
static struct class *g_test_class;


static int test_open(struct inode *inode, struct file *file)
{
	printk("open file\n");
	return 0;
}


static ssize_t 
test_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos)
{
	char data[] = "this is read test!"

	if(nbytes <= sizeof(data)) {
		memcpy(buf, data, nbytes);
		return nbytes;
	}

	return 0;
}


static ssize_t test_write(struct file *file, const char __user *buffer,
			    size_t count, loff_t *ppos)
{
	char data[12] = "";

	if(count <= sizeof(data)) {
		memcpy(data,buffer , count);
		printk("write:%s\n",data);
		return count;
	}
	return 0;
}

static long test_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	return 0;
}


static const struct file_operations test_fops = {
	.owner			= THIS_MODULE,
	.open			= test_open,
	.read			= test_read,
	.write			= test_write,
	.unlocked_ioctl		= test_ioctl,
};

static int module_init(void)
{
	int ret;

	/* alloc device_id from system */
	/* register_chrdev_region() functin is used for devie_id
	 * you had konw
	 * */
	ret = alloc_chrdev_region(&g_devid, 0, 1, DEV_NAME);
	if (ret) {
		pr_info("%s err, line %d\n", __func__, __LINE__);
		return -1;
	}

	/* for malloc cdev type memory */
	g_ptest_cdev = cdev_alloc();
	if (g_ptest_cdev == NULL) {
		pr_info("%s err, line %d\n", __func__, __LINE__);
		goto err2;
	}

	/* init cdev struct data,such device_id and [struct file_operations]*/
	cdev_init(g_ptest_cdev, &test_fops);


	/* add cdev device to system */
	ret = cdev_add(g_ptest_cdev, g_devid, 1);
	if (ret < 0) {
		pr_info("%s err, line %d\n", __func__, __LINE__);
		goto err1;
	}

	/* to creat a class and will creat in sysfs */
	g_test_class = class_create(THIS_MODULE, DEV_NAME);
	if (IS_ERR(g_test_class)) {
		pr_info("%s err, line %d\n", __func__, __LINE__);
		goto err1;
	}

	/* creat node in /dev dir */
	device_create(g_test_class, NULL, g_devid, 0, DEV_NAME);

	pr_info("%s success\n", __func__);
	return 0;

err1:
	cdev_del(g_ptest_cdev);
	g_ptest_cdev = NULL;
err2:
	/* free device_id */
	unregister_chrdev_region(g_devid, 1);
	return -1;

}

static void timer_test_cdev_deinit(void)
{
	/*destroy node of /dev/xxx */
	device_destroy(g_test_class, g_devid);
	/*destory node of /sys/class/xx */ 
	class_destroy(g_test_class);

	/* delete cdev in system */
	cdev_del(g_ptest_cdev);

	/*free device_id  */
	unregister_chrdev_region(g_devid, 1);
}

static int __init test_init(void)
{
	int ret;
	pr_info("%s enter\n", __func__);
	ret = module_init();
	if (ret < 0) {
		pr_err("%s err, line %d\n", __func__, __LINE__);
	}
	pr_info("%s success\n", __func__);
	return 0;
}

static void __exit test_exit(void)
{
	pr_info("%s enter\n", __func__);
	test_cdev_deinit();

}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("author");
MODULE_DESCRIPTION("sunxi timer test driver code");
