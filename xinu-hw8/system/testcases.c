/**
 * @file testcases.c
 * @provides testcasese.
 *
 *
 * Modified by:	
 *
 * TA-BOT:MAILTO 
 *
 */
/* Embedded XINU, Copyright (C) 2023.  All rights reserved. */

#include <xinu.h>

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
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
	
	uchar c;

	kprintf("===TEST BEGIN===\r\n");

	// TODO: Test your operating system!

	c = kgetc();
	switch (c)
	{
		case '0':
			int mem = (void *)malloc(sizeof(ulong));
			if (mem == NULL) {
				kprintf("Error\r\n");
			}

			else {
				mem = 2;
				kprintf("Memory allocated = %d\r\n", mem);
			}

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
