/**
 * Builtins are commands affect the current process, like bash builtins.
 * 
 * @author teknopaul
 */

#include "litesh.h"

// SNIP_builtins

// busybox Copyright (C) 2001  Manuel Novoa III  <mjn3@codepoet.org>
const char*
bb_basename(const char *name)
{
	const char *cp = strrchr(name, '/');
	if (cp) return cp + 1;
	return name;
}

static int
litesh_argv_count(char **argv)
{
	for ( int i = 0 ; i < LITESH_MAX_ARGS; i++ ) {
		if ( ! argv[i] ) return i;
	}
	return LITESH_MAX_ARGS;
}

/**
 * Change directory
 * 
 * @param argv ["cd", dir ]
 */
int
litesh_builtin_cd(char **argv)
{
	int    rc;
	
	if ( litesh_argv_count(argv) == 2 ) {
		rc = chdir(argv[1]);
		if ( rc ) return LITESH_ERROR;
	}
	
	return LITESH_OK;
}

/**
 * Print output
 *
 * @param argv ["echo", ... ]
 */
int
litesh_builtin_echo(char **argv)
{
	int i;

	for ( i = 1 ; i < LITESH_MAX_ARGS; i++ ) {
		if (argv[i]) {
			if (i > 1) putchar(' ');
			fputs(argv[i], stdout);
		}
		else {
			putchar('\n');
			break;
		}
	}
	return LITESH_OK;
}

/**
 * Print output to stderr
 *
 * @param argv ["error", ... ]
 */
int
litesh_builtin_error(char **argv)
{
	int i;

	for ( i = 1 ; i < LITESH_MAX_ARGS; i++ ) {
		if (argv[i]) {
			if (i > 1) fputc(' ', stderr);
			fputs(argv[i], stderr);
		}
		else {
			fputc('\n', stderr);
			break;
		}
	}
	return LITESH_OK;
}

/**
 * sleep
 *
 * @param argv ["sleep", seconds ]
 */
int
litesh_builtin_sleep(char **argv)
{
	int  time = 1;
	
	if ( litesh_argv_count(argv) == 2 ) time = atoi(argv[1]);
	
	sleep(time);
	
	return LITESH_OK;
}

/**
 * exec, similar to bashes builtin, without redirects
 *
 * @param argv ["exec", bianry, ...]
 */
int
litesh_builtin_exec(char **argv)
{

	if ( litesh_argv_count(argv) > 1 ) {
		execvp(argv[1], argv + 1);
	}

	// execvp should not return
	fprintf(stderr, "error '%s' : %s\n", argv[1], strerror(errno));
	return LITESH_ERROR;
}

/**
 * reexec fork a process and hang, reexecuting if necessary
 *
 * @param argv ["reexec", binary, ...]
 */
int
litesh_builtin_reexec(char **argv)
{
	int     status, pid, rc, reboots = 0;

	while (1) {
		pid = fork();

		if ( pid == 0 ) {
			// child

			execvp(argv[1], argv + 1);

			// execvp should not return
			fprintf(stderr, "error '%s': %s\n", argv[0], strerror(errno));

		}
		// parent
		else if ( pid < 0 ) {
			fprintf(stderr, "respawn error");
			return LITESH_ERROR;
		}

		if ( wait(&status) == -1 ) exit(1);
		
		rc = WEXITSTATUS(status);
		
		if ( rc ) {
			fprintf(stderr, "error '%s': %d\n", argv[0], rc);
			if ( reboots > 0 )  sleep(LITESH_RESPAWN_DELAY);
			if (reboots++ > 10) sleep(LITESH_RESPAWN_DELAY_2);
		}
		else {
			if ( reboots > 0 )  sleep(LITESH_RESPAWN_DELAY);
			if (reboots++ > 10) sleep(LITESH_RESPAWN_DELAY_2);
		}
	}
	
	return LITESH_OK;
}

/**
 * setuid - reduce privs
 *
 * Accepts 1 arg - numeric user id
 * or 2 args - user id and group id
 * 
 * @param argv ["setuid", uid, (gid) ]
 */
int
litesh_builtin_setuid(char **argv)
{
	int uid, gid;
	
	if (argv[2]) {
		gid = atoi(argv[1]);
		if (gid >= 500 && gid <= 60000) {
			if ( setgid(gid) ) return LITESH_ERROR;
		}
	}
	if (argv[1]) {
		uid = atoi(argv[1]);
		if (uid >= 500 && uid <= 60000) {
			if ( setuid(uid) ) return LITESH_ERROR;
			return LITESH_OK;
		}
	}
	
	fprintf(stderr, "setuid error '%s' : %s\n", argv[1], strerror(errno));
	return LITESH_ERROR;
}

/**
 * execute a command line, fork a process and wait for it to return.
 * 
 * @param argv [binary, ...]
 */
int
litesh_builtin_fork(char **argv)
{
	int     status, pid, rc;
	
	pid = fork();

	if ( pid == 0 ) {
		// child

			execvp(argv[0], argv);

			// execvp should not return
			fprintf(stderr, "error '%s': %s\n", argv[0], strerror(errno));
			exit(1);
	}
	// parent
	else if ( pid < 0 ) {
		fprintf(stderr, "fork error");
		return LITESH_ERROR;
	}

	if ( wait(&status) == -1 ) {
		fprintf(stderr, "wait error");
		return LITESH_ERROR;
	}
	
	rc = WEXITSTATUS(status);
	
	if ( rc != 0 ) {
		fprintf(stderr, "error executing: '%s'\n", argv[0]);
		return LITESH_ERROR;
	}

	return LITESH_OK;
}

/**
 * spawn/fork a process and don't wait for it to return
 *
 * @param argv ["spawn", binary, ...]
 */
int
litesh_builtin_spawn(char **argv)
{
	int     pid;
	
	pid = fork();

	if ( pid == 0 ) {
		// child

		execvp(argv[1], argv + 1);

		// execvp should not return
	}
	// parent
	else if ( pid < 0 ) {
		fprintf(stderr, "spawn error");
		return LITESH_ERROR;
	}

	return LITESH_OK;
}

/**
 * wait for all spawned processes to finish
 *
 * @param argv ["wait"]
 */
int
litesh_builtin_wait(char **argv)
{
	siginfo_t   infop;
	memset(&infop, 0, sizeof(siginfo_t));

	waitid(P_ALL, 0, &infop, WEXITED);
	
	return LITESH_OK;
}

/**
 * Exit the process
 *
 * @param argv ["exit", (code)]
 */
int
litesh_builtin_exit(char **argv)
{
	int code = 0;
	
	if ( argv[1] && atoi(argv[1]) > 0 ) {
		exit(atoi(argv[1]));
	}
	exit(code);
}
// SNIP_builtins
