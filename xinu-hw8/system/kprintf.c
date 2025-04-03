/**
 * COSC 3250 - Project 3
 * This program is creating a simple synchronous serial driver for the embedded operating system
 * @author [Sam Mrusek and Danny Ruffolo]
 * Instructor [Dr. Brylow]
 * TA-BOT:MAILTO [samantha.mrusek@marquette.edu, danny.ruffolo@marquette.edu]
 */

#include <xinu.h>

#define UNGETMAX 10             /* Can un-get at most 10 characters. */
static unsigned char ungetArray[UNGETMAX];
unsigned int  bufp;

syscall kgetc()
{
    volatile struct ns16550_uart_csreg *regptr;
    regptr = (struct ns16550_uart_csreg *)UART_BASE;

    // TODO: First, check the unget buffer for a character.
    //       Otherwise, check UART line status register, and
    //       once there is data ready, get character c.
    
    uchar c = 0;
    
    if(bufp > 0) { /* If the ungetArray is not empty */ 
	    c =  ungetArray[--bufp]; /* Decrease pointer and get value at that index in ungetArray */
	    return c;
    }
    while(!(regptr->lsr  & UART_LSR_DR)); /* Infinite loop to keep polling for input */
    c = regptr->buffer; /* Assign value from buffer to character */
    return c;

    return SYSERR; /* If anything else, return an error */
}

/**
 * kcheckc - check to see if a character is available.
 * @return true if a character is available, false otherwise.
 */
syscall kcheckc(void)
{
    volatile struct ns16550_uart_csreg *regptr;
    regptr = (struct ns16550_uart_csreg *)UART_BASE;

    if((bufp > 0) || (regptr->lsr & UART_LSR_DR)) { /* Check to see if buffer is not empty or serial port is not 
						       waiting for data */

            return 1; /* If either has data, return true */
    }
    else {
	    return 0;
    }
	
    // TODO: Check the unget buffer and the UART for characters.

	return SYSERR;
}

/**
 * kungetc - put a serial character "back" into a local buffer.
 * @param c character to unget.
 * @return c on success, SYSERR on failure.
 */
syscall kungetc(unsigned char c)
{
	if(bufp >= UNGETMAX) { /* If the ungetArray is full, return an error */
		kprintf("ungetch: too many characters\n");
	}
	else { /* Otherwise assign character input to next empty spot in buffer */
		ungetArray[bufp++] = c;
	}

    // TODO: Check for room in unget buffer, put the character in or discard.

    return SYSERR;
}

syscall kputc(uchar c)
{
    volatile struct ns16550_uart_csreg *regptr;
    regptr = (struct ns16550_uart_csreg *)UART_BASE;

    // TODO: Check UART line status register.
    //       Once the Transmitter FIFO is empty, send character c.
    	
	while(!(regptr->lsr & UART_LSR_TEMT));	 /* This is checking to see if the status register is empty 
				      			and ready to accept new data (c) */
	regptr->buffer = c; /* Write character onto transmit buffer */
	return c;
}

syscall kprintf(const char *format, ...)
{
    int retval;
    va_list ap;

    va_start(ap, format);
    retval = _doprnt(format, ap, (int (*)(long, long))kputc, 0);
    va_end(ap);
    return retval;
}
