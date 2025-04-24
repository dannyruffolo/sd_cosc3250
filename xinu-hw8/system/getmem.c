/**
 * @file getmem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Allocate heap memory.
 *
 *
 * @param nbytes
 *      Number of bytes requested.
 *
 * @return
 *      ::SYSERR if @p nbytes was 0 or there is no memory to satisfy the
 *      request; otherwise returns a pointer to the allocated memory region.
 *      The returned pointer is guaranteed to be 8-byte aligned.  Free the block
 *      with memfree() when done with it.
 */
void *getmem(uint nbytes)
{
    register memblk *prev, *curr, *leftover;

    if (0 == nbytes)
    {
        return (void *)SYSERR;
    }

    /* round to multiple of memblock size   */
    nbytes = (uint)roundmb(nbytes);
    struct memhead *head = (memhead *)proctab[currpid].heaptop;

    /* TODO:
     *      - Traverse through the freelist
     *        to find a block that's suitable 
     *        (Use First Fit with remainder splitting)
     *      - if there is no suitable block, call user_incheap
     *        with the request to add more pages to our process heap
     *      - return memory address if successful
     */

    prev = NULL;
    curr = head->head;

    if(curr == NULL) {
            struct memblock *newmem = (struct memblock *)user_incheap(nbytes);

    }

    while(curr != NULL) {
            if(curr->length == nbytes) {
                    head->head = curr->next;
                    head->length -= curr->length;

                    return curr;
            }

            else if(curr->length > nbytes) {
                    head->head = leftover;
                    leftover->length = curr->length - nbytes;
                    head->length -= nbytes;

                    return curr;
            }
            else {
                    prev = curr;
                    head->head = curr->next;
            }
    }

    return (void *)SYSERR;
}
