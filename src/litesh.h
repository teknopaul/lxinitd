
#ifndef LITESH_H
#define LITESH_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


// return codes
#define LITESH_OK                0
#define LITESH_ERROR            -1   // general error code
#define LITESH_SYNTAX           -2   // syntax error in conf file

// constants
#define LITESH_MAJOR_VERSION    0
#define LITESH_MINOR_VERSION    4

#define PRINT_VERSION  { printf("lxinitd %i.%i\n", LITESH_MAJOR_VERSION, LITESH_MINOR_VERSION); exit(0); }

#define LITESH_MAX_ARGS          10                      // max number of command line arguments
#define LITESH_MAX_LINE_LENGTH   2048                    // line length of litesh scripts
#define LITESH_RESPAWN_DELAY     0.1                     // slight delay respawning to prevent failing proc eating all the CPU
#define LITESH_RESPAWN_DELAY_2   1                       // incremental backoff for repeatedly failing services

int litesh_exitp(const char *message);

// SNIP_parser.c

#define LITESH_EXPECT_SHEBANG   1
#define LITESH_FAIL_ON_ERROR    2
#define LITESH_EXEC_BLANKS      4


typedef int (*litesh_execute_argv_pt)(char **argv);
int litesh_parse(const char* script, litesh_execute_argv_pt litesh_execute_argv);
int litesh_parse_fd(int in, litesh_execute_argv_pt litesh_execute_argv, int flags);
int litesh_parse_argv_len(char **argv);
int litesh_parse_argv_count(char **argv);

// SNIP_parser.c

// SNIP_builtins.c
const char* bb_basename(const char *name);

int litesh_builtin_cd(char **argv);
int litesh_builtin_echo(char **argv);
int litesh_builtin_error(char **argv);
int litesh_builtin_sleep(char **argv);
int litesh_builtin_setuid(char **argv);
int litesh_builtin_fork(char **argv);
int litesh_builtin_exec(char **argv);
int litesh_builtin_reexec(char **argv);
int litesh_builtin_spawn(char **argv);
int litesh_builtin_wait(char **argv);
int litesh_builtin_exit(char **argv);

// SNIP_builtins.c

#endif // LITESH_H
