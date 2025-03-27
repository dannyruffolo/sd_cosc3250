/**
 * COSC 3250 - Project 4
 * This is a function to create new processes in embedded operating system 
 * @author [Sam Mrusek and Danny Ruffolo]
 * Instructor [Dr.Brylow]
 * TA-BOT:MAILTO [samantha.mrusek@marquette.edu danny.ruffolo@marquette.edu]
 */

/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

static pid_typ newpid(void);
void userret(void);
//void *getstk(ulong);

/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param priority priority of process
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, uint priority, char *name, ulong nargs, ...)
{
    ulong *saddr;               /* stack address                */
    ulong pid;                  /* stores new process id        */
    pcb *ppcb;                  /* pointer to proc control blk  */
    ulong i;
    va_list ap;                 /* points to list of var args   */
    ulong pads = 0;             /* padding entries in record.   */ 

    if (ssize < MINSTK)
        ssize = MINSTK;

    ssize = (ulong)((((ulong)(ssize + 3)) >> 2) << 2);
    /* round up to even boundary    */
    //saddr = (ulong *)getstk(ssize);     /* allocate new stack and pid   
    pid = newpid();
    /* a little error checking      */
    if ((((ulong *)SYSERR) == saddr) || (SYSERR == pid))
    {
        return SYSERR;
    }

    numproc++;
    saddr = (ulong *)pgalloc();
    ppcb = &proctab[pid];
	
    // TODO: Setup PCB entry for new process.
    
    ppcb->pagetable = vm_userinit(pid, saddr);
    ppcb->state = PRSUSP;   		// State: suspended
    ppcb->stkbase = saddr;		 // Stack base
    ppcb->stklen = ssize;         	// Stack length: size
    ppcb->tickets = priority;            // Assigning tickets to process for scheduling
    strncpy(ppcb->name, name, PNMLEN);    	// Name: ID
    

    /* Initialize stack with accounting block. */
    *saddr = STACKMAGIC;
    *--saddr = pid;
    *--saddr = ppcb->stklen;
    *--saddr = (ulong)ppcb->stkbase;

    /* Handle variable number of arguments passed to starting function   */
    if (nargs)
    {
        pads = ((nargs - 1) / ARG_REG_MAX) * ARG_REG_MAX;
    }
    /* If more than 8 args, pad record size to multiple of native memory */
    /*  transfer size.  Reserve space for extra args                     */
    for (i = 0; i < pads; i++)
    {
        *--saddr = 0;
    }
    // TODO: Initialize process context.
 
 
    ppcb->ctx[CTX_SP] = (ulong)saddr;
    ppcb->ctx[CTX_PC] = (ulong)funcaddr;
    ppcb->ctx[CTX_RA] = (ulong)INITRET;

    ppcb->swaparea[CTX_KERNSATP] = (ulong)MAKE_SATP(0, _kernpgtbl);
    ppcb->swaparea[CTX_KERNSP] = (ulong)_kernsp;

    
    // TODO:  Place arguments into context and/or activation record.
    //        See K&R 7.3 for example using va_start, va_arg and
    //        va_end macros for variable argument functions.

    va_start(ap, nargs);
    for(i = 0;i < nargs; i++) {
	    if(i < 8) {
		    ppcb->ctx[i] = va_arg(ap, ulong);
	    }
	    else {
		    saddr[i - 8] = va_arg(ap, ulong);
	    } 
    }

    va_end(ap);

    return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
static pid_typ newpid(void)
{
    pid_typ pid;                /* process id to return     */
    static pid_typ nextpid = 0;

    for (pid = 0; pid < NPROC; pid++)
    {                           /* check all NPROC slots    */
        nextpid = (nextpid + 1) % NPROC;
        if (PRFREE == proctab[nextpid].state)
        {
            return nextpid;
        }
    }
    return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
    // ASSIGNMENT 5 TODO: Replace the call to kill(); with user_kill();
    // when you believe your trap handler is working in Assignment 5
    user_kill();
    // kill(currpid); 
}
