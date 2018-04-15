/**
 * lxinitd - a minimal init system.
 * 
 * This should be run as /sbin/init by the LXC container and be pid 1.
 *
 * @author teknopaul
 */

#include "litesh.h"
#include "lxinitd_service.h"
#include "lxinitd_service_log.h"

static const char *init_script = "/etc/rc.local";

static int lxinitd_execute_argv(char **argv);
static int lxinitd_is_runlevel(const char **argv);
/**
 * LXC entry point
 */
int
main(int argc, const char* argv[])
{
	int    rc = 0;
	const char *script;
	struct stat stat_s;

	lxinitd_service_services_init();

	// TODO test for init 6 and the like

	if ( argc == 2 && strcmp("-v", argv[1]) == 0 ) PRINT_VERSION

	if (argc == 2) {
		script = argv[1];
		if ( lxinitd_is_runlevel(argv) ) {
			return 2;
		}
		
	} else {
		script = init_script;
	}

	rc = stat(argv[1], &stat_s);
	
	rc = stat(script, &stat_s);
	
	if ( rc != 0 ) {
		fprintf(stderr, "unable to stat: %s", script);
		return 1;
	}

	if ( stat_s.st_size == 0) {
		fprintf(stderr, "empty script: %s", script);
		return 1;
	}
	
	if ( litesh_parse(script, lxinitd_execute_argv) == LITESH_OK ) {
		if ( lxinitd_service_size() ) {
			lxinitd_service_wait();
		}
		return 0;
	}
	else {
		fprintf(stderr, "parse error: %s", script);
		return 1;
	}
}

/**
 * @param line \0 terminated command line
 */
static int
lxinitd_execute_argv(char **argv)
{

	if ( strcmp("sleep", argv[0]) == 0 ) {
		litesh_builtin_sleep(argv);
	}
	else if ( strcmp("nolog", argv[0]) == 0 ) {
		lxinitd_service_log_off();
	}
	else if ( strcmp("setuid", argv[0]) == 0 ) {
		litesh_builtin_setuid(argv);
	}
	else if ( strcmp("reexec", argv[0]) == 0 ) {
		litesh_builtin_reexec(argv);
	}
	else if ( strcmp("spawn", argv[0]) == 0 ) {
		litesh_builtin_spawn(argv);
	}
	else if ( strcmp("respawn", argv[0]) == 0 ) {
		lxinitd_builtin_respawn(argv);
	}
	else if ( strcmp("service", argv[0]) == 0 ) {
		lxinitd_builtin_respawn_pid_file(argv);
	}
	else {
		litesh_builtin_fork(argv);
	}
	
	return LITESH_OK;
}

static int
lxinitd_is_runlevel(const char **argv)
{
	if ( strncmp("init", bb_basename(argv[0]), 4) == 0 ) {
		 if ( strcmp("0", argv[1]) == 0 ||
			  strcmp("1", argv[1]) == 0 ||
			  strcmp("2", argv[1]) == 0 ||
			  strcmp("3", argv[1]) == 0 ||
			  strcmp("4", argv[1]) == 0 ||
			  strcmp("5", argv[1]) == 0 ||
			  strcmp("6", argv[1]) == 0 )  {
			fprintf(stderr, "lxinitd does not support runlevels");
			return LITESH_ERROR;
		}
	}
	return LITESH_OK;
}
