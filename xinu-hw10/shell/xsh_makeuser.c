/**
 * COSC 3250 - Project 9
 * This is a function to implement a File System
 * @author [Sam Mrusek and Danny Ruffolo]
 * Instructor [Dr.Brylow]
 * TA-BOT:MAILTO [samantha.mrusek@marquette.edu danny.ruffolo@marquette.edu]
 * */

/**
 * @file     xsh_makeuser.c
 * @provides xsh_makeuser
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (makeuser) makes a new user account.
 * @param args array of arguments
 * @return OK for success, SYSERR for errors.
 */
command xsh_makeuser(int nargs, char *args[])
{
/**
 * TODO:
 * This function creates a new entry in the global table of users,
 * with a valid password hash and salt, and updates the passwd file
 * on disk.  You may break this task down into any number of helper
 * functions within this file, and also may rely on helper functions
 * that already exist, such as getusername(), hasspassword(), and
 * passwdFileWrite().
 *
 * Steps:
 * 1) Find a free usertab entry for a new user, and set it USERUSED.
 * 2) If the shell did not provide a user name, prompt for one.
 * 3) Prompt for a new password, and calculate the hash.
 * 4) Initialize the fields of the new user entry.
 * 5) Commit the changes to the passwd file on disk.
 * 6) Printf "Successfully created user ID %d\n" with the new user ID.
 *
 * Errors to watch for:
 * 1) There is not already a user logged in.
 *    Error text = "Must login first\n".
 * 2) The logged in userid is not already SUPERUID.
 *    Error text = "ERROR: Only superusr can make new users!\n".
 * 3) There are no more unused slots in usertab.
 *    Error text = "ERROR: No more users available in usertab!\n".
 */

    // Step 1: Verify the user is logged in and is the superuser
    if (userid < SUPERUID)
    {
        printf("Must login first\n");
        return SYSERR;
    }
    if (userid != SUPERUID)
    {
        printf("ERROR: Only superusr can make new users!\n");
        return SYSERR;
    }

    // Step 2: Check argument count
    if (nargs != 2)
    {
        printf("Usage: makeuser <username>\n");
        return SYSERR;
    }

    char *username = args[1];

    // Step 3: Search if the user already exists in the global usertab
    for (int i = 0; i < MAXUSERS; i++)
    {
        if (usertab[i].state == USERUSED && strcmp(usertab[i].username, username) == 0)
        {
            printf("ERROR: User '%s' already exists!\n", username);
            return SYSERR;
        }
    }

    // Step 4: Find a free usertab entry
    int freeIndex = -1;
    for (int i = 0; i < MAXUSERS; i++)
    {
        if (usertab[i].state == USERFREE)
        {
            freeIndex = i;
            break;
        }
    }
    if (freeIndex == -1)
    {
        printf("ERROR: No more users available in usertab!\n");
        return SYSERR;
    }

    // Step 5: Prompt for password (twice) and confirm they match
    char password1[MAXPASSLEN];
    char password2[MAXPASSLEN];
    printf("Enter password: ");
    getpassword(password1, MAXPASSLEN); // Assume getpassword() is implemented
    printf("Confirm password: ");
    getpassword(password2, MAXPASSLEN);

    if (strcmp(password1, password2) != 0)
    {
        printf("ERROR: Passwords do not match!\n");
        return SYSERR;
    }

    // Step 6: Generate a hash of the password using xinuhash
    ulong salt = SALT; // Use the predefined SALT constant
    ulong hashedPassword = xinuhash(password1, MAXPASSLEN, salt);

    // Step 7: Populate a new user struct in the usertab
    struct userent *newUser = &usertab[freeIndex];
    strncpy(newUser->username, username, MAXUSERLEN);
    newUser->passhash = hashedPassword;
    newUser->salt = salt;
    newUser->state = USERUSED;

    // Step 8: Call passwdFileWrite() to persist the change
    if (passwdFileWrite() == SYSERR)
    {
        printf("ERROR: Failed to write to passwd file!\n");
        return SYSERR;
    }

    // Step 9: Print success message and return OK
    printf("Successfully created user ID %d\n", freeIndex);
    return OK;
}