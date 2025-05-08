/**
 * COSC 3250 - Project 9
 * This is a function to implement a File System
 * @author [Sam Mrusek and Danny Ruffolo]
 * Instructor [Dr.Brylow]
 * TA-BOT:MAILTO [samantha.mrusek@marquette.edu danny.ruffolo@marquette.edu]
 * */

/**
 * @file     xsh_chpass.c
 * @provides xsh_chpass
 *
 */
/* Embedded XINU, Copyright (C) 2024.  All rights reserved. */

#include <xinu.h>

/**
 * Shell command (chpass) changes an existing user password.
 * @param args array of arguments
 * @return OK for success, SYSERR for errors.
 */
command xsh_chpass(int nargs, char *args[])
{
/**
 * TODO:
 * This function creates a new password for an existing user.
 * You may break this task down into any number of new helper
 * functions within this file, and also may rely on helper functions
 * that already exist, such as getusername(), hasspassword(), and
 * passwdFileWrite().
 *
 * Steps:
 * 1) If no user name was provided to the shell command, superusr should
 *    be prompted for one.  Normal users are not prompted, because we
 *    default to changing their own password.
 * 2) Search for the user name in the usertab.
 * 3) If the current user is not superusr, prompt for the previous password.
 *    Prompt text = "Enter previous password for user %s: ".
 * 4) If the hash of the previous password matched what is on record,
 *    prompt for new password.
 *    Prompt test = "Enter new password for user %s: ".
 * 5) Place the new password hash into the user entry, and commit to disk.
 * 6) Printf "Successfully changed password for user ID %d\n" with user ID.
 *
 * Errors to watch for:
 * 1) There is not already a user logged in.
 *    Error text = "Must login first\n".
 * 2) The logged in userid is not SUPERUID, but is trying to change someone
 *    else's password.
 *    Error text = "ERROR: Only superusr can change other passwords!\n".
 * 3) The given user name cannot be found in the existing user table.
 *    Error text = "User name %s not found.\n".   
 * 4) The password change failed.  (i.e., passwords didn't match.)
 *    Error text = "Password for user %s does not match!\n".
 */

    // Step 1: Verify that a user is logged in
    if (userid < 0)
    {
        printf("Must login first\n");
        return SYSERR;
    }

    char *username;
    if (nargs == 2)
    {
        // If a username is provided, check if the current user is superusr
        if (userid != SUPERUID)
        {
            printf("ERROR: Only superusr can change other passwords!\n");
            return SYSERR;
        }
        username = args[1];
    }
    else if (nargs == 1)
    {
        // If no username is provided, default to the current user
        username = usertab[userid].username;
    }
    else
    {
        printf("Usage: chpass [username]\n");
        return SYSERR;
    }

    // Step 2: Search for the user in the usertab
    struct userent *targetUser = NULL;
    for (int i = 0; i < MAXUSERS; i++)
    {
        if (usertab[i].state == USERUSED && strcmp(usertab[i].username, username) == 0)
        {
            targetUser = &usertab[i];
            break;
        }
    }
    if (targetUser == NULL)
    {
        printf("User name %s not found.\n", username);
        return SYSERR;
    }

    // Step 3: If not superusr, verify the old password
    if (userid != SUPERUID)
    {
        char oldPassword[MAXPASSLEN];
        printf("Enter previous password for user %s: ", username);
        getpassword(oldPassword, MAXPASSLEN);

        ulong hashedOldPassword = xinuhash(oldPassword, MAXPASSLEN, targetUser->salt);
        if (hashedOldPassword != targetUser->passhash)
        {
            printf("Password for user %s does not match!\n", username);
            return SYSERR;
        }
    }

    // Step 4: Prompt for the new password twice
    char newPassword1[MAXPASSLEN];
    char newPassword2[MAXPASSLEN];
    printf("Enter new password for user %s: ", username);
    getpassword(newPassword1, MAXPASSLEN);
    printf("Confirm new password for user %s: ", username);
    getpassword(newPassword2, MAXPASSLEN);

    if (strcmp(newPassword1, newPassword2) != 0)
    {
        printf("ERROR: Passwords do not match!\n");
        return SYSERR;
    }

    // Step 5: Generate a new salt
    ulong newSalt = SALT; // Use the predefined SALT constant

    // Step 6: Hash the new password with the new salt
    ulong newHashedPassword = xinuhash(newPassword1, MAXPASSLEN, newSalt);

    // Step 7: Overwrite the user's existing salt and hash
    targetUser->salt = newSalt;
    targetUser->passhash = newHashedPassword;

    // Step 8: Persist the changes to disk
    if (passwdFileWrite() == SYSERR)
    {
        printf("ERROR: Failed to write to passwd file!\n");
        return SYSERR;
    }

    // Step 9: Print success message and return OK
    printf("Successfully changed password for user ID %d\n", targetUser - usertab);
    return OK;
}
