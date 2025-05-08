/**
 * COSC 3250 - Project 9
 * This is a function to implement a File System
 * @author [Sam Mrusek and Danny Ruffolo]
 * Instructor [Dr.Brylow]
 * TA-BOT:MAILTO [samantha.mrusek@marquette.edu danny.ruffolo@marquette.edu]
 * */

/* passwdFileRead.c - passwdFileRead */
/* Copyright (C) 2024, Marquette University.  All rights reserved. */

#include <xinu.h>


#define FILE_BUFFER_SIZE 1024
#define FILE_PASSWD "passwd"

/*------------------------------------------------------------------------
 * passwdFileRead - Read in a password file from filesystem.
 *------------------------------------------------------------------------
 */
devcall passwdFileRead(void)
{
/**
 * TODO:
 * This function opens a file called "passwd" for reading, reads in its
 * contents, and stores them into the global usertab array.
 * Steps:
 * 1) Open file "passwd", getting the file descriptor.
 * 2) Use the descriptor to seek to offset zero in the file, the beginning.
 * 3) Read in the bytes of the file using fileGetChar(), storing them
 *    into a suitable temporary location.
 * 4) Close the file.
 * 5) After checking the file contents look OK, copy over to usertab using
 *    memcpy(), and return OK.
 *
 * Errors to watch for:
 * 1) Trouble opening the passwd file.  (It may not exist.)
 *    Error text = "No passwd file found.\n"
 * 2) Trouble reading bytes from the file.  (It might be too short.)
 *    Return SYSERR.
 * 3) The contents of the file could be blank or corrupted.  Check that
 *    the first field of the first user entry is state USERUSED, and that
 *    the salt field matched the SALT constant for this version of the O/S
 *    before overwriting the contents of the existing user table.
 *    Error text = "Passwd file contents corrupted!\n".
 */

    int fd; // File descriptor
    char buffer[FILE_BUFFER_SIZE]; // Temporary buffer for file contents
    int bytesRead = 0;
    int userCount = 0;

    // Step 1: Open the "passwd" file
    fd = open(FILE_PASSWD, "r");
    if (fd == SYSERR)
    {
        printf("No passwd file found.\n");
        return SYSERR;
    }

    // Step 2: Read the file contents into the buffer
    bytesRead = read(fd, buffer, FILE_BUFFER_SIZE - 1);
    if (bytesRead <= 0)
    {
        printf("Error reading passwd file.\n");
        close(fd);
        return SYSERR;
    }
    buffer[bytesRead] = '\0'; // Null-terminate the buffer

    // Step 3: Parse the file contents line by line
    char *line = strtok(buffer, "\n");
    while (line != NULL && userCount < MAXUSERS)
    {
        // Split the line into username, salt, and hash
        char *username = strtok(line, ",");
        char *salt = strtok(NULL, ",");
        char *hash = strtok(NULL, ",");

        if (username == NULL || salt == NULL || hash == NULL)
        {
            printf("Passwd file contents corrupted!\n");
            close(fd);
            return SYSERR;
        }

        // Populate the usertab entry
        strncpy(usertab[userCount].username, username, MAXUSERLEN);
        usertab[userCount].salt = strtoul(salt, NULL, 10); // Convert salt to ulong
        usertab[userCount].passhash = strtoul(hash, NULL, 10); // Convert hash to ulong
        usertab[userCount].state = USERUSED;

        userCount++;
        line = strtok(NULL, "\n"); // Move to the next line
    }

    // Step 4: Close the file
    close(fd);

    // Step 5: Verify the first user entry and global consistency
    if (userCount == 0 || usertab[0].state != USERUSED || usertab[0].salt != SALT)
    {
        printf("Passwd file contents corrupted!\n");
        return SYSERR;
    }

    // Update the global user count
    extern int nusers; // Declare nusers as extern
    nusers = userCount;

    return OK;
}