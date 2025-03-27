/**
 * @file testcases.c
 * @provides testcases
 *
 * TA-BOT:MAILTO
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <xinu.h>

extern void main(int, char *);

int testmain(int argc, char **argv)
{
    int i = 0;
    kprintf("Hello XINU World!\r\n");

    for (i = 0; i < 10; i++)
    {
        kprintf("This is process %d\r\n", currpid);

        user_yield();
    }
    return 0;
}

int process1(void) {
	kprintf("Process 1 is running\r\n");
	return 0;
}

int process2(void) {
        kprintf("Process 2 is running\r\n");
	return 0;
}
/*
int process3(void) {
        kprintf("Process 3 is running\r\n");
	return 0;
}
*/

/**
 * testcases - called after initialization completes to test things.
 */
void testcases(void)
{
    int c;

    kprintf("0) Test that 1 process is running\r\n");
    kprintf("1) Test that 2 processes run for equal amounts of time\r\n");
    kprintf("2) Test 2 processes, one runs forever and another never runs\r\n");
    kprintf("3) Test 2 processes with different priorities\r\n");
    kprintf("4) Test 3 processes with different priorities\r\n");

    kprintf("===TEST BEGIN===\r\n");

    // TODO: Test your operating system!

    c = kgetc();
    switch (c)
    {
    case '0':
        ready(create((void *)testmain, INITSTK, HIGH, "MAIN1", 0),
              RESCHED_YES);
		break;

    case '1':
        ready(create((void *)process1, INITSTK, MED, "process1", 0),
              RESCHED_NO);
	ready(create((void *)process2, INITSTK, MED, "process2", 0),
              RESCHED_YES);
	while (numproc > 1) {
		resched();
	}
        break;

    case '2':
        ready(create((void *)testmain, INITSTK, HIGH, "MAIN1", 0),
              RESCHED_NO);
        ready(create((void *)testmain, INITSTK, LOW, "MAIN2", 0),
              RESCHED_YES);
	while (numproc > 1) {
                resched();
        }
        break;

    case '3':
        ready(create((void *)testmain, INITSTK, HIGH, "MAIN1", 0),
              RESCHED_NO);
        ready(create((void *)testmain, INITSTK, MED, "MAIN2", 0),
              RESCHED_YES);
        while (numproc > 1) {
                resched();
        }
        break;

    case '4': 
        ready(create((void *)testmain, INITSTK, LOW, "MAIN1", 2, 0, NULL),
              RESCHED_NO);
        ready(create((void *)testmain, INITSTK, MED, "MAIN2", 2, 0, NULL),
              RESCHED_NO);
	ready(create((void *)testmain, INITSTK, HIGH, "MAIN3", 2, 0, NULL),
              RESCHED_YES);
        while (numproc > 1) {
                resched();
        }
	kprintf("Finished case 4");
        break;

    default:
        break;
    }

    kprintf("\r\n===TEST END===\r\n");
    return;
}
