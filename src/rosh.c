/**
 * rosh - readonly shell based on litesh
 *
 * @author teknopaul
 */

#include "litesh.h"
#include "rosh_aliases.h"

static int rosh_execute_argv(char **argv);

/**
 * print message and die.
 */
static int
rosh_exitp(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

/**
 * entry point
 */
int
main(int argc, const char* argv[])
{
	int    rc = 0;
	struct stat stat_s;

	if (argc == 1) rosh_aliases(argv);

	// check we have one argument
	if (argc != 2) rosh_exitp("argument required (script name)");
	
	if ( argc == 2 && strcmp("-v", argv[1]) == 0 ) PRINT_VERSION

	rc = stat(argv[1], &stat_s);
	
	if ( rc != 0 ) {
		fprintf(stderr, "unable to stat the script: %s", argv[1]);
		return 1;
	}

	if ( stat_s.st_size == 0) {
		fprintf(stderr, "empty script :%s", argv[1]);
		return 1;
	}
	
	if ( litesh_parse(argv[1], rosh_execute_argv) == LITESH_OK ) {
		return 0;
	} else {
		rosh_exitp("parse error");
	}
}

/**
 * @param line \0 terminated command line
 */
static int
rosh_execute_argv(char **argv)
{
	
	if ( strcmp("cd", argv[0]) == 0 ) {
		litesh_builtin_cd(argv);
	}
	else if ( strcmp("echo", argv[0]) == 0 ) {
		litesh_builtin_echo(argv);
	}
	else if ( strcmp("error", argv[0]) == 0 ) {
		litesh_builtin_error(argv);
	}
	else if ( strcmp("sleep", argv[0]) == 0 ) {
		litesh_builtin_sleep(argv);
	}
	else if ( strcmp("setuid", argv[0]) == 0 ) {
		litesh_builtin_setuid(argv);
	}
	else if ( strcmp("exec", argv[0]) == 0 ) {
		litesh_builtin_exec(argv);
	}
	else if ( strcmp("spawn", argv[0]) == 0 ) {
		litesh_builtin_spawn(argv);
	}
	else if ( strcmp("wait", argv[0]) == 0 ) {
		litesh_builtin_wait(argv);
	}
	else if ( strcmp("exit", argv[0]) == 0 ) {
		litesh_builtin_exit(argv);
	}
	else {
		litesh_builtin_fork(argv);
	}
	
	return LITESH_OK;
}
