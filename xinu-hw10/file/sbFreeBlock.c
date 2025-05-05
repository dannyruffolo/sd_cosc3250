/* sbFreeBlock.c - sbFreeBlock */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/*                                                                 */
/* and                                                             */
/*                                                                 */
/*                                                                 */

#include <xinu.h>

/*------------------------------------------------------------------------
 * sbFreeBlock - Add a block back into the free list of disk blocks.
 *------------------------------------------------------------------------
 */
devcall sbFreeBlock(struct superblock *psuper, int block)
{
    // TODO: Add the block back into the filesystem's list of
    //  free blocks.  Use the superblock's locks to guarantee
    //  mutually exclusive access to the free list, and write
    //  the changed free list segment(s) back to disk.
    
    struct fbcnode *fbc, *fbc2;
    struct dirblock *swizzle;
    struct dentry *phw;
    int result, i;
    int diskfd;

    if(NULL == psuper) {
	    return SYSERR;
    }    
    
    phw = psuper->sb_disk;
    if(NULL == phw) {
	    return SYSERR;
    }

    diskfd = phw - devtab;
    fbc = psuper->sb_freelst; 
    if(NULL == fbc) {
	    return SYSERR;
    }

    // Mutually exclusive
    wait(psuper->sb_freelock);

    while(fbc->fbc_next != NULL) {
	    fbc = fbc->fbc_next;
    }

    if(fbc->fbc_count < FREEBLOCKMAX) {
	    fbc->fbc_free[fbc->fbc_count] = block;
	    fbc->fbc_count++;

	// Update this free block record on disk.
        fbc2 = fbc->fbc_next;
        if (NULL == fbc->fbc_next)
        {
            fbc->fbc_next = 0;
        }
        else
        {
            fbc->fbc_next =
                (struct fbcnode *)fbc->fbc_next->fbc_blocknum;
        }
        seek(diskfd, fbc->fbc_blocknum);
        if (SYSERR == write(diskfd, fbc, sizeof(struct fbcnode)))
        {
            return SYSERR;
        }
        fbc->fbc_next = fbc2;
        if (!result)
            result = SYSERR;

    }
    else {
    	if (fbc->fbc_next == NULL) {
        	// Last block in freelist — clear freelist and update superblock.
        	psuper->sb_freelst = NULL;

        	// Swizzle psuper->sb_dirlst before writing to disk.
        	struct dirblock *swizzle = psuper->sb_dirlst;
        	psuper->sb_dirlst = (struct dirblock *)swizzle->db_blocknum;

        	seek(diskfd, psuper->sb_blocknum);
        	if (SYSERR == write(diskfd, psuper, sizeof(struct superblock))) {
            		return SYSERR;
		}

        	// Unswizzle
        	psuper->sb_dirlst = swizzle;

        	result = fbc->fbc_blocknum;


        	signal(psuper->sb_freelock);
        	return result;
	}

    	result = fbc2->fbc_blocknum;

    	for (int i = 0; i < fbc2->fbc_count; i++) {
        fbc->fbc_free[i] = fbc2->fbc_free[i];
	}

    	fbc->fbc_count = fbc2->fbc_count;
    	fbc->fbc_next = fbc2->fbc_next;
    	free(fbc2);

    	// Prepare for disk write — swizzle fbc_next.
    	struct fbcnode *temp_next = fbc->fbc_next;
    	if (fbc->fbc_next == NULL) {
        	fbc->fbc_next = 0;
    	} 
	else {
        	fbc->fbc_next = (struct fbcnode *)temp_next->fbc_blocknum;
	}

    	seek(diskfd, fbc->fbc_blocknum);
    	if (SYSERR == write(diskfd, fbc, sizeof(struct fbcnode))) {
        	return SYSERR;
   	}

   	// Unswizzle
    	fbc->fbc_next = temp_next;
    }

    signal(psuper->sb_freelock);
    return result;

}
