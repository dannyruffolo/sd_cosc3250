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

/* This fake page table will allow you to test your printPageTable function
 * without having paging completely working.
 */
pgtbl createFakeTable(void){
	pgtbl root = pgalloc();
	pgtbl lvl1 = pgalloc();
	pgtbl lvl0 = pgalloc();

	volatile ulong *pte = &(root[5]);
	*pte = PA2PTE(lvl1) | PTE_V;

	ulong *lvl1pte = &(lvl1[145]);
	*lvl1pte = PA2PTE(lvl0) | PTE_V;

	ulong *lvl0pte = &(lvl0[343]);
	*lvl0pte = PA2PTE(0x1000) | PTE_W | PTE_R | PTE_V;

	ulong *lvl0pte1 = &(lvl0[120]);
	*lvl0pte1 = PA2PTE(0x4000) | PTE_X | PTE_R | PTE_V;

	ulong *lvl0pte2 = &(lvl0[45]);
	*lvl0pte2 = PA2PTE(0x8000) | PTE_X | PTE_R | PTE_V;

	return root;
}

void printPageTable(pgtbl pagetable, int level)
{
	/*
	* TODO: Write a function that prints out the page table.
	* Your function should print out all *valid* page table entries in
	* the page table.  If any of the entries are a link (if the
	* Read/Write/Execute bits aren't set), recursively print that page
	* table.  If it is a leaf, print the page table entry and the
	* physical address is maps to. 
	*/
    
	if(level < 0) {
		return;
	}

	int i;
        for(i = 0; i < PTE_MAX; i++) {
                ulong pte = pagetable[i];
                ulong pa = PTE2PA(pte);
                if(pte & PTE_V) {
			int j;
                        for(j = 0; j < level; j++) {
                                kprintf("\t");
                        }
			kprintf("Entry %d   pa: %x   pte: %x\r\n", i, pa, pte);


                        if((pte & PTE_R) || (pte & PTE_W) || (pte & PTE_X)) {
				if(pte & PTE_R) {
					if(pte & PTE_W) {
						if(pte & PTE_X) {
							kprintf("\trwx\r\n");
						}
						kprintf("\trw-\r\n");
					}
					else {
						if(pte & PTE_X) {
							kprintf("\tr-x\r\n");
						}
						kprintf("\tr--\r\n");
					}
				}
				else {
					if(pte & PTE_W) {
                                                if(pte & PTE_X) {
                                                        kprintf("\t-wx\r\n");
                                                }
                                                kprintf("\t-w-\r\n");
                                        }
                                        else {
                                                if(pte & PTE_X) {
                                                        kprintf("\t--x\r\n");
                                                }
                                                kprintf("\t---\r\n");
                                        }

				}
                        }
                        else {
                                printPageTable(pa, level - 1);
                        }
                }
        }
}


int testCreateProcess() {
	kprintf("Test of one process running\r\n");
	return 0;
}

int testUserMode(void) {
	user_putc(0, 'U');
	user_putc(0, 's');
	user_putc(0, 'e');
	user_putc(0, 'r');
	user_putc(0, ' ');
        user_putc(0, 'T');
        user_putc(0, 'e');
        user_putc(0, 's');
        user_putc(0, 't');

	sc_putc(0, 'O');
	sc_putc(0, 'S');
	sc_putc(0, ' ');
        sc_putc(0, 'T');
        sc_putc(0, 'e');
        sc_putc(0, 's');
        sc_putc(0, 't');

	return 0;
	kprintf("DONE");
}

int testKernel() {
	kprintf("Testing that we can read process ticket value\r\n");
	kprintf("Tickets: %d\r\n", proctab[currpid].tickets);

	kprintf("Testing if we can write to kernel variables\r\n");
	proctab[currpid].tickets = 20;
	kprintf("Tickets: %d\r\n", proctab[currpid].tickets);

	return 0;
}

int testmain(int argc, char **argv) {
	int i = 0;
	for(i = 0;i < 10; i++) {
		kprintf("This is process %d\r\n", currpid);

		user_yield();
	}
	return 0;
}
/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
	uchar c;

	kprintf("===TEST BEGIN===\r\n");

	// TODO: Test your operating system!

	c = kgetc();
	switch (c)
	{
		case 'p':
			// Testing printPageTable function with the fake table
			
			kprintf("This is the Fake Table\r\n");
			
			printPageTable(createFakeTable(), 2);
			break;

		case '0':
			// TODO: Write a testcase that creates a user process
			// and prints out it's page table
			
			kprintf("Create user process and print out its page table\r\n");
			
			pid_typ pid = create(testCreateProcess, INITSTK, PRIORITY_HIGH, "MAIN1", 2, 0, NULL);
			pcb *ppcb = &proctab[pid];
			printPageTable(ppcb->pagetable, 2);

			break;
		case '1':
			// TODO: Write a testcase that demonstrates a user
			// process cannot access certain areas of memory
			
			kprintf("Demonstrate user process cannot access certain memory\r\n");

            		ready(create((void *)testUserMode, INITSTK, PRIORITY_HIGH, "MAIN1", 0), RESCHED_YES);
			
			break;
		case '2':
			// TODO: Write a testcase that demonstrates a user
			// process can read kernel variables but cannot write
			// to them
			
			kprintf("Read from kernel variables, but cannot write to them\r\n");

			ready(create((void *)testKernel, INITSTK, PRIORITY_HIGH, "MAIN1", 2, 0, NULL), RESCHED_YES);
			
			break;
		case '3':
			// TODO: Extra credit! Add handling in xtrap to detect
			// and print out a Null Pointer Exception.  Write a
			// testcase that demonstrates your OS can detect a
			// Null Pointer Exception.
			break;
		default:
			break;
	}

	kprintf("\r\n===TEST END===\r\n");
	return;
}
