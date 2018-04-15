/**
 * noopsh - shell that isn't a shell
 *
 * @author teknopaul
 */

#include "litesh.h"
#include "rosh_aliases.h"

static int noopsh_execute_argv(char **argv);

/**
 * entry point
 */
int
main(int argc, const char* argv[])
{
	
	fprintf(stdout, "[\x1B[31merror\x1B[0m] this server does not support interactive commands\n");
	
	if ( litesh_parse_fd(STDIN_FILENO, noopsh_execute_argv, 0) == LITESH_OK ) {
		return 0;
	} else {
		exit(1);
	}

}

/**
 * @param line \0 terminated command line
 */
static int
noopsh_execute_argv(char **argv)
{
	
	if ( strcmp("exit", argv[0]) == 0 ) {
		litesh_builtin_exit(argv);
	}
	else {
		fprintf(stdout, "[\x1B[31merror\x1B[0m] nothing to see here\n");
	}
	
	return LITESH_OK;
}
