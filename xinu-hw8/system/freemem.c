/**
 * @file freemem.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * @ingroup memory_mgmt
 *
 * Frees a block of heap-allocated memory.
 *
 * @param memptr
 *      Pointer to memory block allocated with getmem().
 *
 * @param nbytes
 *      Length of memory block, in bytes.  (Same value passed to getmem().)
 *
 * @return
 *      ::OK on success; ::SYSERR on failure.  This function can only fail
 *      because of memory corruption or specifying an invalid memory block.
 */
syscall freemem(void *memptr, uint nbytes)
{
    register struct memblock *block, *next, *prev;
    struct memhead *head;
    ulong top;

    /* make sure block is in heap */
    if ((0 == nbytes) || ((ulong)memptr < (ulong)proctab[currpid].heaptop))
    {
        return SYSERR;
    }

    head = (struct memhead *)proctab[currpid].heaptop;
    block = (struct memblock *)memptr;
    nbytes = (uint)roundmb(nbytes);

    /* TODO:
     *      - Find where the memory block should
     *        go back onto the freelist (based on address)
     *      - Find top of previous memblock
     *      - Make sure block is not overlapping on prev or next blocks
     *      - Coalesce with previous block if adjacent
     *      - Coalesce with next block if adjacent
     */
    
    struct memblock *curr = head->head;
    block->length = nbytes;
    prev = NULL;

    // Find where the memory block should go
    while((curr != NULL) && (curr < block)) {
            prev = curr;
            curr = curr->next;
    }

    // Find top of previous memblock


    if(curr != NULL && (block + block->length == curr)) {
            block->length += curr->length;
            block->next = curr->next;
    }

    else {
            block->next = curr;
    }

    if(prev != NULL && prev + prev->length == block) {
            prev->length += block->length;
            prev->next = block->next;
    } else {
        if (prev == NULL) {
            head->head = block;
        } else {
            prev->next = block;
        }
    }

    return OK;
}
