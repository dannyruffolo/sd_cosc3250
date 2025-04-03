/**
 * COSC 3250 - Project 6
 * This is a program to implement the lottery scheduling algorithm 
 * @author [Sam Mrusek and Danny Ruffolo]
 * Instructor [Dr.Brylow]
 * TA-BOT:MAILTO [samantha.mrusek@marquette.edu danny.ruffolo@marquette.edu]
 */

/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 Assignment 6
 */
/* Embedded XINU, Copyright (C) 2008,2024.  All rights reserved. */

#include <xinu.h>

extern void ctxsw(void *, void *, ulong);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate 
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */

pid_typ lottery();

syscall resched(void)
{
    pcb *oldproc;               /* pointer to old process entry */
    pcb *newproc;               /* pointer to new process entry */

    oldproc = &proctab[currpid];

    /* place current process at end of ready queue */
    if (PRCURR == oldproc->state)
    {
        oldproc->state = PRREADY;
        enqueue(currpid, readylist);
    }

    currpid = lottery();

    /**
     * We recommend you use a helper function for the following:
     * TODO: Get the total number of tickets from all processes that are in
     * current and ready states.
     * Use random() function to pick a random ticket. 
     * Traverse through the process table to identify which proccess has the
     * random ticket value.  Remove process from queue.
     * Set currpid to the new process.
     */
    // currpid = dequeue(readylist);
    newproc = &proctab[currpid];
    newproc->state = PRCURR;    /* mark it currently running    */

#if PREEMPT
    preempt = QUANTUM;
#endif

   // kprintf("Switching to %d", currpid);
    ctxsw(&oldproc->ctx, &newproc->ctx, MAKE_SATP(currpid, newproc->pagetable));

    /* The OLD process returns here when resumed. */
    return OK;
}  

pid_typ lottery() {
        int totaltickets = 0;
        // number of processes = 50
        int i = 0;
        while(i < NPROC) {
                if(proctab[i].state == PRREADY || proctab[i].state == PRCURR){
                        totaltickets += proctab[i].tickets;
                }
                i++;
        }
	if (totaltickets == 0) {
		return NULLPROC;
	}

        int counter = 0;
        int winner = random(totaltickets);

        // Want to run through the ready list to check which process has winning ticket
	i = 0;
	if(i >= NPROC - 1) {
	       return NULLPROC;
       	}
        else {
		for(i = 0; i < NPROC; i++) {
         		if(proctab[i].state == PRREADY || proctab[i].state == PRCURR) {
                		counter = counter + proctab[i].tickets;
				// kprintf("Counter is %d", counter);
                		if(counter > winner) {
                        		break; // found the winner
				}
			}
		}
		return i;// This is the winning process pid (which is the pid the loop above stops on)
	}
}
