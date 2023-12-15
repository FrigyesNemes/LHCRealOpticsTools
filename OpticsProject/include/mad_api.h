#ifndef MAD_API_H
#define MAD_API_H

#ifdef __cplusplus
extern "C" {
#endif


// types

struct sequence_list;

// variables

/**
 * @file madextern.h Calls to use for external libraries
 *
 * This is a header that can be included in external libraries that wants to 
 * link against the mad-x library. When you want to send commands to madX you must
 * first start with the command madextern_start(), before you can parse
 * character pointers to the command madextern_input(char*).
 * You must free memory before exiting with the function madextern_end(),
 * otherwise you will get a segmentation fault.
 *
 * This library can be extended with commands like e.g. madextern_twiss().
 * Add declaration in this file (and preferably documentation), and then implement
 * it in madxp.c or another preferred source file. Always use naming convention
 * madxp_nameOfFunc(), and always declare the function extern.
 */
extern struct sequence_list* sequences;

// interface

/**
 * @brief Initialize the mad-x library.
 * 
 * This function is part of the madextern external functions.
 * These are not supposed to be used by any internal mad-x code.
 *
 * This function must be called before you can call other
 * functions in the mad-x library.
 */
void madextern_start(void);

/**
 * @brief Close mad-x library
 *
 * This function is part of the madextern external functions.
 * These are not supposed to be used by any internal mad-x code.
 *
 * Use this function as the last command when you want to free memory used by objects
 * in mad-x.
 */
void madextern_end(void);

/**
 * @brief Send command to the mad-x library.
 *
 * This function is part of the madextern external functions.
 * These are not supposed to be used by any internal mad-x code.
 *
 * This function works in the same way as the mad-x interpreter,
 * ie you send a command like e.g. 'call,file="filename.madx";'
 * Note that commands should have a ';' at the end, and that several
 * commands can be sent at once separated by ';'
 *
 * @param ch The command to execute
 */
void madextern_input(char*);

/**
 * @brief Send command to the mad-x library.
 *
 * This function is part of the madextern external functions.
 * These are not supposed to be used by any internal mad-x code.
 *
 * This function returns a name_list structure of all currently
 *  available sequences. Reason we do not return sequence_list
 *  is that name_list are "cleaner" structures which are easier
 *  to access in python (currently only use-case).
 *
 */
struct sequence_list* madextern_get_sequence_list(void);

#ifdef __cplusplus
}
#endif

#endif // MAD_API_H

