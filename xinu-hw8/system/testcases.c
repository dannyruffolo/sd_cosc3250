/**
 * @file testcases.c
 * @provides testcases
 *
 *
 * Modified by:	
 *
 * TA-BOT:MAILTO 
 *
 */
/* Embedded XINU, Copyright (C) 2023.  All rights reserved. */
#include <xinu.h>

void my_heapinit(void) {
	// Create a cute little toy heap
        proctab[0].heaptop = pgalloc();

        // Initialize all four fields of memhead
        memhead *mh = (memhead *)proctab[0].heaptop;
        mh->head = (struct memblock *)(mh + 1);
        mh->length = PAGE_SIZE - sizeof(struct memhead);
        mh->base = mh->head;
        mh->bound = ((ulong)mh) + PAGE_SIZE;

        // Build first memblock
        mh->head->next = NULL;
        mh->head->length = mh->length;
}

void printList(struct memhead *mh) {
        struct memblock *prev, *curr;
        prev = mh->head;
        curr = mh->head->next;

        kprintf("This is the head next: 0x%x length: 0x%x\r\n", prev->next, prev->length);
        while (curr != NULL) {
		kprintf("in while\r\n");
                kprintf("This is the current next: 0x%x length: 0x%x\r\n", curr->next, curr->length);
                curr = curr->next;
        }
        kprintf('Finished printing the list\r\n');
}

void testcases(void)
{
        uchar c;

        kprintf("===TEST BEGIN===\r\n");

        // TODO: Test your operating system!

        c = kgetc();
        switch (c)
        {
                case '0':
                        my_heapinit();
                        struct memhead *head = (struct memhead *)proctab[0].heaptop;
                        
			void *one = malloc(64);
                        if (one == NULL) {
                            kprintf("NULL\r\n");
                        } else {	
                            kprintf("allocated: %lu\r\n", one);
                        }

                        void *two = malloc(128);
                        if (two == NULL){
                                kprintf("NULL\r\n");
                        } 
                        else {
                                kprintf("allocated: %lu\r\n", two);
                        }

                        free(one);
                        kprintf("freed: %d\r\n", one);
                        free(two);
                        kprintf("freed: %d\r\n", two);
                        break;
                case '1':

                        break;
                case '2':

                        break;
                case '3':

                        break;

                default:
			break;
	}

	kprintf("\r\n===TEST END===\r\n");
	return;
}
