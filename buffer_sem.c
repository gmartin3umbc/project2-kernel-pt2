#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/string.h>

#include "buffer_sem.h"

bb_buffer_421_t *buffer_sem = NULL;
static struct semaphore mutex;
static struct semaphore fill_count;
static struct semaphore empty_count;

SYSCALL_DEFINE0(init_buffer_sem_421)
{
	// Write your code to initialize buffer
	// only initialize if buffer does not exist (i.e. equals to null)
	if (!buffer_sem) {
		// allocating memory for buffer
		buffer_sem = kmalloc(sizeof(bb_buffer_421_t), GFP_KERNEL);

		// initializing buffer with standard values
		buffer_sem->length = 0;

		int i;

		for (i = 0; i < SIZE_OF_BUFFER; i++) {
			// allocating memory for node
			bb_node_421_t *newNode =
				kmalloc(sizeof(bb_node_421_t), GFP_KERNEL);

			// initializing newNode with standard values
			newNode->next = NULL;

			if (i == 0) {
				// make new node the head
				buffer_sem->read = newNode;

				// make new node the tail
				buffer_sem->write = newNode;
			}

			else {
				// insert new node at the end of the queue
				buffer_sem->write->next = newNode;

				// make new node the tail of the buffer
				buffer_sem->write = newNode;
			}

			// making buffer circular by making the tail point to the head
			buffer_sem->write->next = buffer_sem->read;
		}

		// making write and read point to first node
		buffer_sem->write = buffer_sem->read;

		// Initialize your semaphores here.
		sema_init(&fill_count, 0);
		sema_init(&empty_count, 20);
		sema_init(&mutex, 1);

		return 0;
	}

	return -1;
}

SYSCALL_DEFINE1(enqueue_buffer_sem_421, char *__user, data)
{
	down(&empty_count);
	down(&mutex);

	// check if buffer has been initialized and if it is not full
	if (buffer_sem && buffer_sem->length < SIZE_OF_BUFFER) {
		// copy data into write node
		copy_from_user(buffer_sem->write->data, data, DATA_LENGTH);

		// move write node
		buffer_sem->write = buffer_sem->write->next;

		// increase length of buffer
		++buffer_sem->length;

		up(&fill_count);
		up(&mutex);

		printk("Enqueue: %c\n", data[0]);
		printk("sema mutex = %d\n", mutex.count);
		printk("sema fill_count = %d\n", fill_count.count);
		printk("sema empty_count = %d\n", empty_count.count);

		return 0;
	}

	up(&fill_count);
	up(&mutex);

	return -1;
}

SYSCALL_DEFINE1(dequeue_buffer_sem_421, char *__user, data)
{
	down(&fill_count);
	down(&mutex);

	// check if buffer has been initialized and if it is not empty
	if (buffer_sem && buffer_sem->length > 0) {
		// copy data into write node
		copy_to_user(data, buffer_sem->read->data, DATA_LENGTH);

		// move read node
		buffer_sem->read = buffer_sem->read->next;

		// decrease length of buffer
		--buffer_sem->length;

		up(&empty_count);
		up(&mutex);

		printk("Dequeue: %c\n", data[0]);
		printk("sema mutex = %d\n", mutex.count);
		printk("sema fill_count = %d\n", fill_count.count);
		printk("sema empty_count = %d\n", empty_count.count);

		return 0;
	}

	up(&empty_count);
	up(&mutex);

	return -1;
}

SYSCALL_DEFINE0(delete_buffer_sem_421)
{
	// Tip: Don't call this while any process is waiting to enqueue or dequeue.
	// write your code to delete buffer and other unwanted components

	down(&mutex);

	if (buffer_sem) {
		int i;
		for (i = 0; i < SIZE_OF_BUFFER; i++) {
			bb_node_421_t *currentNode = buffer_sem->read;

			buffer_sem->read = buffer_sem->read->next;

			currentNode->next = NULL;
			kfree(current);
		}

		kfree(buffer_sem);

		buffer_sem = NULL;

		up(&mutex);

		//sem_destroy(&fill_count);
		//sem_destroy(&empty_count);
		//sem_destroy(&mutex);

		return 0;
	}

	up(&mutex);

	return -1;
}
