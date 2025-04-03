/**
 * @file map.c
 * @provides mapPage, mapAddressRange
 *
 * Modified by:
 *
 * TA-BOT:MAILTO
 */
/* Embedded Xinu, Copyright (C) 2013, 2025.  All rights reserved. */

#include <xinu.h>

syscall mapPage(pgtbl pagetable, ulong vaddr, ulong paddr, int attr);

/**
 * Maps a given virtual address range to a corresponding physical address range.
 * @param pagetable    base pagetable
 * @param vaddr        start of the virtual address range.
 *                     This should be truncated to the nearest page boundry.
 * @param paddr        start of the physical address range
 * @param length       length of the range to map
 * @param attr         any attributes to set on the page table entry
 * @return             OK if successful, otherwise a SYSERR
 */
syscall mapAddressRange(pgtbl pagetable, ulong vaddr, ulong paddr,
                        ulong length, int attr)
{
    ulong end;

    // DEBUGGING LINE:
    //    kprintf("mapAddressRange(0x%lX, 0x%lX, 0x%lX, 0x%lX, 0x%lX)\r\n",
    //      pagetable, vaddr, paddr, length,  attr);

    if (length == 0)
    {
        return SYSERR;
    }

    // Round the length, vaddr and paddr to the nearest page size
    length = roundpage(length);
    vaddr = (ulong)truncpage(vaddr);
    paddr = (ulong)truncpage(paddr);
    end = vaddr + length;

    // Loop over the entire range
    for (; vaddr < end; vaddr += PAGE_SIZE, paddr += PAGE_SIZE)
    {
        // Map the individual virtual pages into the table.
        if (mapPage(pagetable, vaddr, paddr, attr) == SYSERR)
        {
            return SYSERR;
        }
    }

    return OK;
}

/**
 * Maps a page to a specific virtual address
 * @param pagetable  base pagetable
 * @param vaddr      virtual address at which the page will reside
 * @param paddr      physical frame that will be mapped at the virtual address
 * @param attr       any attributes to set on the page table entry
 * @return           OK if successful, otherwise a SYSERR
 */
syscall mapPage(pgtbl pagetable, ulong vaddr, ulong paddr, int attr)
{
/**
 * Starting at the base pagetable, traverse the hierarchical page table
 *  structure for the virtual address.  Create pages along the way if they
 *  don't exist.
 */

    /**
    * TODO:
    * For each level in the page table, get the page table entry by masking
    * and shifting the bits in the virtualaddr depending on the level.
    * If the valid bit is set, use that pagetable for the next level
    * Otherwise create the page by calling pgalloc().  Make sure to setup the 
    * page table entry accordingly. Call sfence_vma once finished to flush TLB
    * Once you've traversed all three levels, set the attributes (attr) for
    * the leaf page (don't forget to set the valid bit!)
    */
    // This code is from the board and to me makes sense for masking and shifting bits in vaddr
    
	ulong VPN2 = PX(2, vaddr);
    	ulong VPN1 = PX(1, vaddr);
    	ulong VPN0 = PX(0, vaddr);

		ulong *level0 = NULL;
		ulong *level1 = NULL;
		
		if (!(pagetable[VPN2] & PTE_V)) {
        		// Allocate a new page for this level and set the valid bit
        		pagetable[VPN2] = PA2PTE(pgalloc()) | PTE_V;  // Set page entry with valid bit
			// Get the physical address for the next level (level 1)
			level1 = (ulong *)PTE2PA(pagetable[VPN2]);

    			// Level 1: Check if the entry is valid
    			if (!(level1[VPN1] & PTE_V)) {
        		// Allocate a new page for this level and set the valid bit
        			level1[VPN1] = PA2PTE(pgalloc()) | PTE_V;  // Set page entry with valid bit
				// Get the physical address for the next level (level 0)
				level0 = (ulong *)PTE2PA(level1[VPN1]);


				if(!(level0[VPN0] & PTE_V)) {
					level0[VPN0] = PA2PTE(pgalloc()) | PTE_V;

				        // Level 0: Set the final mapping for the leaf page
                			level0[VPN0] = PA2PTE(paddr) | attr | PTE_V;
				}
			}
		}
		
		sfence_vma(); // Flush TLB

    //  DEBUGGING LINE:
    	kprintf("mapPage(pt:0x%X, v:0x%X, p:0x%0X, a:0x%03X)\r\n", pagetable, vaddr, paddr, attr);
    return OK;
}
