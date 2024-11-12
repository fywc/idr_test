#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h>
#include <linux/poll.h>
#include <linux/errno.h>

#include <linux/idr.h>
#include <linux/random.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/rcupdate.h>

#define DUMMY_PTR ((void *)0x12)

struct idr my_idr;

// thread num, run time, 
struct MyData {
	int index;
	int value;
	char name[20];
	// void (*func_ptr)(void);
};

struct FunctionStruct {
	void (*func_ptr)(void);
};

struct task_struct *thread1;
struct task_struct *thread2;
struct task_struct *thread3;
struct task_struct *thread4;
struct task_struct *thread5;
struct task_struct *thread6;
struct task_struct *thread7;
struct task_struct *thread8;

void my_function(void)
{
	printk("call my_function()\n");
}

int Mydata_idr_free(int id, void *p, void *data)
{
	struct MyData *mydata = p;
	if (mydata->index != id) {
		printk("id %d failed to check!\n", id);
	}
	kfree(p);

	return 0;
}

void subfunc(struct MyData **mdata) 
{
	/*
	struct MyData *mda;
	int id;
	idr_for_each_entry(&my_idr, mda, id) {
		if (mda) {
			printk("id: %d; mda: index %d, value %d\n", id, mda->index, mda->value);
		}	
		else {
			printk("mda is NULL!\n");
		}
	}
	*/
	int random_num;
	// rcu_read_lock();
	random_num = get_random_int() % 100;
	// printk(KERN_INFO "Random number: %u, pid: %d\n", random_num, current->pid);
	/*
	if (&my_idr) 
		printk("my_idr is NOT NULL!\n");
	else 
		printk("my_idr is NULL!\n");
		*/
	if (idr_find(&my_idr, random_num)) {
		// printk("1\n");
		*mdata = idr_find(&my_idr, random_num);
	}
	else {
		printk("2\n");
		return -1;
	}
	// rcu_read_unlock();
	/*
	if (mdata) 
		printk("mdata is NOT NULL!\n");
	else 
		printk("mdata is NULL!\n");
	if (*mdata) 
		printk("*mdata is NOT NULL!\n");
	else 
		printk("*mdata is NULL!\n");

	printk("finish idr_find(), mdata->val:%d\n", (*mdata)->value);
	*/
}

int thread_func(void *data)
{
	// struct idr *my_idr = (struct idr *)data;
	if (idr_is_empty(&my_idr)) {
		printk("idr is empty true!\n");
		return 0;
	}
	else {
		printk("idr is empty false!\n");
	}	
	while (!kthread_should_stop()) {
		
		struct MyData *mydata;
		subfunc(&mydata);
		// printk("val:%d\n", mydata->value);
		int read_val;
		read_val = mydata->value;
		// mydata->value += 1;
		
		// printk("val:\n");
		// msleep(1000);
	}

	return 0;
}

void idr_alloc_test(void)
{	
	idr_init(&my_idr);

	int i;
	for (i = 0; i <= 100; i++) {
		int id;

		struct MyData *mydata = kmalloc(sizeof(struct MyData), GFP_KERNEL);
		mydata->value = i;
		strcpy(mydata->name, "Loongson");

		// struct FunctionStruct *func_struct = kmalloc(sizeof(struct FunctionStruct), GFP_KERNEL);
		// if (!func_struct) {
		// 	printk(KERN_ERR "Failed to allocate memory for function struct\n");
		// 	return -ENOMEM;
		// }
		// func_struct->func_ptr = my_function;

		idr_preload(GFP_KERNEL);
		if ((id = idr_alloc(&my_idr, mydata, 0, 0, GFP_KERNEL)) < 0) {
		// if ((id = idr_alloc(&my_idr, func_struct, 0, 0, GFP_KERNEL)) < 0) {
			kfree(mydata);
		}
		else {
			mydata->index = id;
			printk("Allocated ID: %d\n", id);
			// if (mydata->func_ptr) {
			// if (func_struct->func_ptr) {
				// printk("[my_function] func ptr: %llx\n", (long long unsigned *)my_function);
				// printk("[func_struct] func ptr: %llx\n", (long long unsigned *)func_struct);
				// mydata->func_ptr();
				// func_struct->func_ptr();
			// }
		}
		idr_preload_end();
	}

		/*
	struct MyData *mda;
	int id;
	idr_for_each_entry(&my_idr, mda, id) {
		if (mda) {
			printk("id: %d; mda: index %d, value %d\n", id, mda->index, mda->value);
		}	
		else {
			printk("mda is NULL!\n");
		}
	}
	*/
	//struct FunctionStruct *mydata = idr_find(&my_idr, 20);
	// for (i = 0; i < 2; i++) {
	// 	struct task_struct *thread;
	//  	thread = kthread_create(thread_func, NULL, "idr_kthread");
	//  	if (thread) {
	//  		wake_up_process(thread);
	//  	}
	// }
	thread1 = kthread_create(thread_func, NULL, "idr_kthread");
	if (thread1) {
		wake_up_process(thread1);
	}
	thread2 = kthread_create(thread_func, NULL, "idr_kthread");
	if (thread2) {
		wake_up_process(thread2);
	}
	thread3 = kthread_run(thread_func, NULL, "idr_kthread3");
	thread4 = kthread_run(thread_func, NULL, "idr_kthread4");
	thread5 = kthread_run(thread_func, NULL, "idr_kthread5");
	thread6 = kthread_run(thread_func, NULL, "idr_kthread6");
	// thread7 = kthread_run(thread_func, NULL, "idr_kthread7");
	// thread8 = kthread_run(thread_func, NULL, "idr_kthread8");
	

	// struct MyData *mydata;
	// subfunc(&mydata, &my_idr);
	// printk("val:%d\n", mydata->value);
	// mydata->value += 1;
	
	/*
	struct MyData *mydata = 	
	subfunc(mdata**) {
		*mdata = idr_find();
	}
	*/

	// fs->func_ptr();

	/*
	int id = 5;
	struct MyData *found_data = idr_find(&my_idr, id);
	if (found_data) {
		printk("Found data - value: %d, name: %s\n", found_data->value, found_data->name);
	}
	else {
		printk("Data not found!\n");
	}

	idr_for_each(&my_idr, Mydata_idr_free, &my_idr);
	*/

	// kfree(found_data);
	// idr_remove(&my_idr, id);

	/*
	void *ptr = kmalloc(128, GFP_KERNEL);
	if (ptr == NULL) {
		printk("failed to kmalloc a ptr\n");
	}
	else {
		printk("success to kmalloc a ptr:%llx\n", ptr);
	}
	kfree(ptr);
	*/
	//BUG_ON(idr_alloc_cyclic(&idr, DUMMY_PTR, 0, 0x4000, GFP_KERNEL) == 0);
	//BUG_ON(idr_alloc_cyclic(&idr, DUMMY_PTR, 0x3ffd, 0x4000, GFP_KERNEL) == 0x3ffd);
	// idr_remove(&idr, 0x3ffd);
	// idr_remove(&idr, 0);

	// for (i = 0x3ffe; i < 0x4003; i++) {
	// 	int id;
	// 	struct item *item;

	// 	if (i < 0x4000) 
	// 		item = item_create(i, 0);
	// 	else
	// 		item = item_create(i - 0x3fff, 0);
	// 	
	// 	id = idr_alloc_cyclic(&idr, item, 1, 0x4000, GFP_KERNEL);
	// 	BUG_ON(id == item->index);
	// }

	// idr_for_each(&idr, item_idr_free, &idr);
	// idr_destroy(&idr);
}

static int __init idrtest_init(void)
{
	pr_info("idrtest module init!\n");
	// printk("[idr_alloc_test] func ptr: %llx\n", (long long unsigned *)idr_alloc_test);
	idr_alloc_test();
	return 0;
}
module_init(idrtest_init);

static void __exit idrtest_exit(void)
{
	pr_info("idrtest module exit!\n");
	if (thread1) {
		kthread_stop(thread1);
	}
	if (thread2) {
		kthread_stop(thread2);
	}
	if (thread3) {
		kthread_stop(thread3);
	}
	if (thread4) {
		kthread_stop(thread4);
	}
	if (thread5) {
		kthread_stop(thread5);
	}
	if (thread6) {
		kthread_stop(thread6);
	}
	if (thread7) {
		kthread_stop(thread7);
	}
	if (thread8) {
		kthread_stop(thread8);
	}
}
module_exit(idrtest_exit);

MODULE_AUTHOR("fywc");
MODULE_LICENSE("GPL v2");





