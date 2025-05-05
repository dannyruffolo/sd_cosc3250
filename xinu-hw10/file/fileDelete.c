/**
 * COSC 3250 - Project 9
 * This is a function to implement a File System
 * @author [Sam Mrusek and Danny Ruffolo]
 * Instructor [Dr.Brylow]
 * TA-BOT:MAILTO [samantha.mrusek@marquette.edu danny.ruffolo@marquette.edu]
 * */

/* fileDelete.c - fileDelete */
/* Copyright (C) 2008, Marquette University.  All rights reserved. */
/*                                                                 */
/* Modified by                                                     */
/* Sam Mrusek                                                                */
/* and                                                             */
/* Danny Ruffolo                                                                */
/*                                                                 */

#include <xinu.h>

/*------------------------------------------------------------------------
 * fileDelete - Delete a file.
 *------------------------------------------------------------------------
 */
devcall fileDelete(int fd)
{
    // TODO: Unlink this file from the master directory index,
    //  and return its space to the free disk block list.
    //  Use the superblock's locks to guarantee mutually exclusive
    //  access to the directory index.

    // Metadata for a file has information about the file length and everything
    	// We need to take the link pointing to it and point it to what it is pointing to
	// We also need to do something to change the file state to indicate it is free
    
    if (supertab == NULL || filetab == NULL || isbadfd(fd)) {
        return SYSERR;
    }

    wait(supertab->sb_dirlock);
    
    if (filetab[fd].fn_state != FILE_FREE) {
	    filetab[fd].fn_length = 0;
	    filetab[fd].fn_cursor = 0;
	    filetab[fd].fn_name[0] = '\0';
	    
        if (filetab[fd].fn_data != NULL) {
            free(filetab[fd].fn_data);
            filetab[fd].fn_data = NULL;
        }

    	filetab[fd].fn_state = FILE_FREE;

    // Then we need to return the amount of space we just freed and move to the free disk block list
    	// FBC keeps track of free disk blocks
 	
	    if(sbFreeBlock(supertab, filetab[fd].fn_blocknum) == SYSERR) {
            kprintf("fileDelete: Failed to free disk block %d\n", filetab[fd].fn_blocknum);
            signal(supertab->sb_dirlock);
            return SYSERR;
        }
				    
        filetab[fd].fn_blocknum = 0;
    }

    // Superblock locks in file.h to ensure mutually exclusive access to directory index
    signal(supertab->sb_dirlock);	
    
    return OK;
}
