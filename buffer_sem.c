#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/semaphore.h>
#include <linux/slab.h>

#include "buffer_sem.h"

static bb_node_421_t *nodes_sem;
static bb_buffer_421_t buffer_sem;

static struct semaphore mutex;
static struct semaphore fill_count;
static struct semaphore empty_count;

SYSCALL_DEFINE0(init_buffer_sem_421)
{
	int i;

	//check if buffer has already been initialized
	if (!nodes_sem) {
		//allocate memory for node
		nodes_sem = kmalloc(20 * sizeof(bb_node_421_t), GFP_KERNEL);

		for (i = 0; i < SIZE_OF_BUFFER; ++i) {
			nodes_sem[i].next =
				nodes_sem + ((i + 1) % SIZE_OF_BUFFER);
		}

		//initialize buffer
		buffer_sem = (bb_buffer_421_t){ .length = 0,
						.read = nodes_sem,
						.write = nodes_sem };

		// Initialize your semaphores here
		sema_init(&mutex, i);
		sema_init(&fill_count, 0);
		sema_init(&empty_count, 20);

		return 0;
	}

	return -1;
}

SYSCALL_DEFINE1(enqueue_buffer_sem_421, char *, data)
{
	down(&empty_count);
	down(&mutex);

	// check if buffer has been initialized and if it is not full
	if (buffer_sem.length < SIZE_OF_BUFFER) {
		// copy data into write node
		if (copy_from_user(buffer_sem.write->data, data, DATA_LENGTH) !=0) {
			return -1;
		}

		// move write node
		buffer_sem.write = buffer_sem.write->next;

		// increase length of buffer
		++buffer_sem.length;

		printk("Enqueue: %c\n", data[0]);
		printk("sema mutex = %d\n", mutex.count);
		printk("sema fill_count = %d\n", fill_count.count);
		printk("sema empty_count = %d\n", empty_count.count);

		up(&mutex);
		up(&fill_count);

		return 0;
	}

	up(&mutex);
	up(&fill_count);

	return -1;
}

SYSCALL_DEFINE1(dequeue_buffer_sem_421, char *, data)
{
	down(&fill_count);
	down(&mutex);

	// check if buffer has been initialized and if it is not empty
	if (buffer_sem.length > 0) {
		// copy data into write node
		if(copy_to_user(data, buffer_sem.read->data, DATA_LENGTH) != 0){
			return -1;
		}

		// move read node
		buffer_sem.read = buffer_sem.read->next;

		// decrease length of buffer
		--buffer_sem.length;

		printk("Dequeue: %c\n", data[0]);
		printk("sema mutex = %d\n", mutex.count);
		printk("sema fill_count = %d\n", fill_count.count);
		printk("sema empty_count = %d\n", empty_count.count);

		up(&mutex);
		up(&empty_count);

		return 0;
	}

	up(&mutex);
	up(&empty_count);

	return -1;
}

SYSCALL_DEFINE0(delete_buffer_sem_421)
{
	if (nodes_sem) {
		down(&mutex);

		kfree(nodes_sem);
		nodes_sem = NULL;

		buffer_sem.read = NULL;
		buffer_sem.write = NULL;
		buffer_sem.length = 0;

		return 0;
	}

	return -1;
}
