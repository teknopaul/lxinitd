

#ifndef LITESH_TEST_H
#define LITESH_TEST_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "../litesh.h"

#pragma GCC diagnostic ignored "-Wunused-function"
int litesh_exitp(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

static int errors = 0;

#pragma GCC diagnostic ignored "-Wunused-function"
static int assert(const char *msg, int thing) 
{
    if ( ! thing) {
        errors++;
        fprintf(stdout, "[\x1B[31merror\x1B[0m] %s\n", msg);
        return 0;
    }
    return 1;
}

#pragma GCC diagnostic ignored "-Wunused-function"
static int assert_null(const char *msg, void *thing) 
{
    if (thing) {
        errors++;
        fprintf(stdout, "[\x1B[31merror not null\x1B[0m] %s\n", msg);
        return 1;
    }
    return 0;
}

#pragma GCC diagnostic ignored "-Wunused-function"
static int assert_same(const char *msg, void *ptr1, void *ptr2)
{
    if (ptr1 != ptr2) {
        errors++;
        fprintf(stdout, "[\x1B[31merror not same\x1B[0m] %s\n", msg);
        return 1;
    }
    return 0;
}
#pragma GCC diagnostic ignored "-Wunused-function"
static int equals(const char * msg, int expected, int actual)
{
    if ( expected != actual) {
        errors++;
        fprintf(stdout, "[\x1B[31merror not equal\x1B[0m] %s expected=%i actual=%i\n", msg, expected, actual);
        return 0;
    }
    return 1;
}


#endif