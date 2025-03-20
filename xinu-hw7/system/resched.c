/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 Assignment 6
 */
/* Embedded XINU, Copyright (C) 2008,2024.  All rights reserved. */

#include <xinu.h>

extern void ctxsw(void *, void *);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate 
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */
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

    /**
     * We recommend you use a helper function for the following:
     * TODO: Get the total number of tickets from all processes that are in
     * current and ready states.
     * Use random() function to pick a random ticket. 
     * Traverse through the process table to identify which proccess has the
     * random ticket value.  Remove process from queue.
     * Set currpid to the new process.
     */

    void lottery(){
        int totaltickets = 0;
        
        while(i < NPROC){

        }

        // counter: used to track if we’ve found the winner yet
        int counter = 0;

        // winner: call some random number generator to
        int winner = random(0, totaltickets - 1);

        // current: use this to walk through the list of jobs
        node_t *current = head;
        while (current) {
            counter = counter + current->tickets;
            if (counter > winner)
                break; // found the winner
            current = current->next;
        }
        // ’current’ is the winner: schedule it...


    }

    currpid = dequeue(readylist);
    newproc = &proctab[currpid];
    newproc->state = PRCURR;    /* mark it currently running    */

#if PREEMPT
    preempt = QUANTUM;
#endif

    ctxsw(&oldproc->ctx, &newproc->ctx);

    /* The OLD process returns here when resumed. */
    return OK;
}
