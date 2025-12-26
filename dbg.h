#ifndef __dbg_h__
#define __dbg_h__

#ifdef S_SPLINT_S
// do not define dbg.h macros for splint
#define log_err(M, ...)
#define debug(M, ...)
#define clean_errno()
#define log_warn(M, ...)
#define log_info(M, ...)
#define check(A, M, ...)
#define sentinel(M, ...)
#define check_mem(A)
#define check_debug(A, M, ...)

#else

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef COLOURS
#define KNORMAL "\x1B[0m"
#define KRED "\x1B[31m"
#define KYELLOW "\x1B[33m"
#define KCYAN "\x1B[36m"
#else
#define KNORMAL "\0"
#define KRED "\0"
#define KYELLOW "\0"
#define KCYAN "\0"
#endif

//TODO: Add function to output of the debug, log_err and log_warn

#ifdef NDEBUG
#define debug(M, ...)
#else
/**
 * A debug function that can be disabled by adding #define NDEBUG
 * before dg.h is imported.
 * It prints [DEBUG]: file:line: message\n and can be used like
 * printf for the message.
 */
#define debug(M, ...) fprintf(stderr,\
        "[%sDEBUG%s]: (%s:%s:%d) " M "\n", KCYAN, KNORMAL,\
        __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#endif

/**
 * shows the string that errno outputs. If errno is 0, it
 * prints "None"
 */
#define clean_errno() (errno == 0 ? "None" : strerror(errno))

/**
 * gives an error. can be coloured if COLOURS is defined
 * format: [ERROR]: (file:line: errno: error) message \n
 * can be used like printf for the message.
 * Inputs: M | The message to be shown
 */
#define log_err(M, ...) fprintf(stderr, \
        "[%sERROR%s]: (%s:%s:%d: errno: %s) " M "\n", KRED, KNORMAL,\
        __FILE__,__func__, __LINE__, clean_errno(), ##__VA_ARGS__)

/** gives a warning. can be coloured if COLOURS is defined
 * format: [WARNING]: (file:line: errno: error) message\n
 * can be used like printf for the message.
 * Inputs: M | The message to be shown
 */
#define log_warn(M, ...) fprintf(stderr,\
        "[%sWARNING%s]: (%s:%s:%d: errno: %s) " M "\n", KYELLOW, KNORMAL,\
        __FILE__,__func__, __LINE__, clean_errno(), ##__VA_ARGS__)

/** gives an info message. can be coloured if COLOURS is defined
 * format: [INFO]: (file:line) message\n
 * can be used like printf for the message.
 * Inputs: M | The message to be shown
 */
#define log_info(M, ...) fprintf(stderr,\
    "[INFO] (%s:%s:%d) " M "\n",\
    __FILE__, __func__, __LINE__, ##__VA_ARGS__)

/**
 * checks if the first argument is true. If it is not,
 * make a log error and jump to the error goto
 * Inputs: A | The statement that should be true
 *         M | The message to say if the test fails 
 */
#define check(A, M, ...) if(!(A)) {log_err(M, ##__VA_ARGS__); errno=0; goto error; }

/**
 * if this function is run, make a log error.
 * Inputs: M | Message to say if the test fails
 */
#define sentinel(M, ...) { log_err(M, ##__VA_ARGS__);\
    errno=0; goto error; }

/**
 * checks if A is a valid pointer to memory.
 * It just runs check with the input "Out of memory."
 * Inputs: A | The statement that should be true
 */
#define check_mem(A) check((A), "Out of memory.")

/**
 * just check but it uses debug instead of log error
 * Inputs: A | The statement that should be true
 *         M | The message if the test fails
 */
#define check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__);\
    errno=0; goto error; }

#endif
#endif
