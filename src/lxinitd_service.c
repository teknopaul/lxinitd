/**
 * Service management code.
 *
 * Services are running processes that get restarted with the same params if they die.
 *
 * @author teknopaul
 */

#include "litesh.h"
#include "lxinitd_service.h"
#include "lxinitd_service_log.h"

// SNIP_service

static lxinitd_service_table_t lxinitd_services;

static void lxinitd_service_signal_terms();
static void lxinitd_service_signal_handler(int sig);
static void lxinitd_service_alarm_handler(int sig);
static void lxinitd_service_signal_hup();
static int lxinitd_service_check(); 

/**
 * init the service table
 */
int
lxinitd_service_services_init()
{
	memset(&lxinitd_services, 0, sizeof(lxinitd_service_table_t));
	
	lxinitd_services.init_pid = getpid();
	time(&lxinitd_services.uptime);
	
	lxinitd_service_log_init();

	signal(SIGTERM, lxinitd_service_signal_handler);
	signal(SIGUSR1, lxinitd_service_signal_handler);
	signal(SIGHUP,  lxinitd_service_signal_handler);
	signal(SIGALRM, lxinitd_service_alarm_handler);
	
	return LITESH_OK;
}

static int
lxinitd_service_is_alive()
{
	for ( int i = 0 ; i < lxinitd_services.size; i++ ) {
		if (lxinitd_services.table[i]->state) {
			return lxinitd_services.table[i]->pid;
		}
	}
	return 0;
}

/**
 * Forward SIGTERM to child processes.
 * What ever happens to children, this method exits the process.
 */
static void
lxinitd_service_signal_terms()
{
	int    i, j;
	
	for ( i = 0 ; i < lxinitd_services.size; i++ ) {
		// lxinitd_service_log("[service=\"%s\"] stopping\n", lxinitd_services.table[i]->argv[0]);
		kill(lxinitd_services.table[i]->pid, SIGTERM);
	}
	
	sleep(0.1);

	lxinitd_service_t   *service;
	
	for ( j = 0 ; lxinitd_service_is_alive() && j < LXINITD_SHUTDOWN_TIMEOUT ; j++) {
		for ( i = 0 ; i < lxinitd_services.size; i++ ) {
			service = lxinitd_services.table[i];
			// kill(0) checks a pid exists
			if ( kill(service->pid, 0) == 0 ) {
				lxinitd_service_log("[service=\"%s\"] shutdown\n", service->argv[0]);
				service->state = 0;
			}
		}
		sleep(1);
	}

	for ( i = 0 ; i < lxinitd_services.size; i++ ) kill(lxinitd_services.table[i]->pid, SIGKILL);

	exit(1);
}

static void
lxinitd_service_signal_hup()
{
	lxinitd_service_print(stderr);
}

static void
lxinitd_service_signal_handler(int sig)
{
	lxinitd_service_log("[sig=\"%i\"] signal\n", sig);
	
	/// lxc.haltsignal || lxc.rebootsignal
	if ( sig == SIGTERM || sig == SIGUSR1 ) {
		lxinitd_service_signal_terms();
	}
	else if ( sig == SIGHUP ) {
		lxinitd_service_signal_hup();
	}
}

static void
lxinitd_service_alarm_handler(int sig)
{
	lxinitd_service_check();
}


/**
 * Create a service record, copying args.
 * 
 * @param pid process id
 * @param argv execpv args
 */
lxinitd_service_t*
lxinitd_service_create(int pid, const char **argv)
{
	int               full_len, i;
	lxinitd_service_t  *service;
	
	if (!argv[0] || !strlen(argv[0])) {
		return NULL;
	}
	
	service = calloc(1, sizeof(lxinitd_service_t));
	if (service == NULL) {
		return NULL;
	}
	
	full_len = 0;
	for ( i = 0 ; i < LITESH_MAX_ARGS; i++ ) {
		if (argv[i]) {
			full_len += strlen(argv[i]);
			full_len++;
		}
		else break;
	}

	service->argv[0] = calloc(1, full_len + sizeof(char));
	if (service->argv == NULL) {
		free(service);
		return NULL;
	}
	memcpy(service->argv[0], argv[0], full_len);
	
	for ( i = 1 ; i < LITESH_MAX_ARGS; i++ ) {
		if (argv[i]) {
			service->argv[i] = service->argv[0] + (argv[i] - argv[0]);
		}
		else break;
	}
	
	service->pid = pid;
	service->state = 1;
	lxinitd_service_add(service);
	
	return service;
}

static inline void
shift_args(char *argv[])
{
	int i;
	for ( i = 1 ; i < LITESH_MAX_ARGS; i++ ) {
		argv[i - 1] = argv[i];
	}
	argv[LITESH_MAX_ARGS - 1] = NULL;
}

/**
 * Create a service record, copying args.
 *
 * @param argv pid_file, then execpv args
 */
lxinitd_service_t*
lxinitd_service_create_pid_file(const char **argv)
{
	lxinitd_service_t* service;
	
	if ( argv[0] && argv[1] ) {
		service = lxinitd_service_create(0, argv);
		if ( service == NULL ) return NULL;
		service->pid_file = service->argv[0];
		shift_args(service->argv);
		if ( ! lxinitd_services.check_pid_files ) {
			lxinitd_services.check_pid_files = 1;
			alarm(1);
		}
		return service;
	}
	return NULL;
}

/**
 * Add a service to the service table.
 */
int
lxinitd_service_add(lxinitd_service_t* service)
{
	if ( lxinitd_services.size == LXINITD_MAX_SERVICES) {
		return LXINITD_SERVICES_FLUP;
	}
	
	lxinitd_services.table[lxinitd_services.size++] = service;
	
	return LITESH_OK;
}

/**
 * Find a configured service by its pid
 * 
 * @param pid process id
 */
lxinitd_service_t*
lxinitd_service_find(int pid)
{
	
	for ( int i = 0 ; i < lxinitd_services.size; i++ ) {
		if ( pid == lxinitd_services.table[i]->pid ) return lxinitd_services.table[i];
	}
	
	return NULL;
}

/**
 * Print ps list of services
 */
void
lxinitd_service_print(FILE *fd)
{
	lxinitd_service_t *service;
	
	fprintf(fd, "pid reboot proc\n");
	for ( int i = 0 ; i < lxinitd_services.size; i++ ) {
		service = lxinitd_services.table[i];
		if ( service ) {
			fprintf(fd, "%i %i %s\n", service->pid, service->reboots, service->argv[0]);
			lxinitd_service_log("[a@1 pid=\"%i\" reboot=\"%i\" service=\"%s\"] - \n", service->pid, service->reboots, service->argv[0]);
		}
	}
	
}
/**
 * @return count of configured services
 */
int
lxinitd_service_size()
{
	return lxinitd_services.size;
}

static int
lxinitd_service_restart(lxinitd_service_t *service)
{
	int         pid, rc;
	struct stat stat_s;

	/*
	 * for pid_file based services we need to check for race conditions wich check() and wait()
	 */
	if ( service->pid_file ) {
		rc = stat(service->pid_file, &stat_s);
		if (rc) {
			if ( errno == ENOENT ) {
				// pid file gone, restart() called twice.
				return LITESH_ERROR;
			}
		}
		service->pid = 0;
		rc = unlink(service->pid_file);
		if (rc) lxinitd_service_log("[a@1 service=\"%s\" file=\"%s\"] could not delete pid file\n", service->argv[0], service->pid_file);
	}

	service->reboots++;
	// lxinitd_service_log("[a@1 service=\"%s\" pid=\"%i\"] restarting\n", service->argv[0], service->pid);

	pid = fork();

	if ( pid == 0 ) {
		// child

		if ( service->reboots > 1) sleep(LITESH_RESPAWN_DELAY);
		if ( service->reboots > 10) sleep(LITESH_RESPAWN_DELAY_2);

		execvp(service->argv[0], service->argv);

		// execvp should not return, if it does we cant write anyway
		fprintf(stderr, "[a@1 service=\"%s\"] %s\n", service->argv[0], strerror(errno));
	}
	// parent
	else if ( pid < 0 ) {
		lxinitd_service_log("[a@1] respawn error\n");
	}
	else {
		if ( ! service->pid_file ) {
			service->pid = pid;
		}
		lxinitd_service_log("[a@1 service=\"%s\" pid=\"%i\"] restarted service\n", service->argv[0], service->pid);
	}
	
	return pid;
}

/**
 * respawn any service that fails, this is like the main() loop.
 */
int
lxinitd_service_wait()
{
	int                 wrc;
	siginfo_t           infop;
	lxinitd_service_t   *service;
	
	while (1) {

		memset(&infop, 0, sizeof(siginfo_t));
		
		wrc = waitid(P_ALL, 0, &infop, WEXITED);
		if ( wrc == -1 && errno == ECHILD ) {
			if ( ! lxinitd_services.check_pid_files ) {
				// everything is gone (no children) might as well die.
				lxinitd_service_log("[a@1] no children, terminating\n");
				exit(0);
			}
			// else we are watching pid files
			sleep(LXINITD_CHECK_INTERVAL);
			continue;
		}
		if ( wrc ) {
			lxinitd_service_log("[a@1 rv=\"%i\"] init panic %s\n", wrc, strerror(errno));
			exit(0);
		}
		
		service = lxinitd_service_find(infop.si_pid);
		
		if ( ! service ) {
			// this can be common if we are pid 1 parent of all orphaned processes
			// lxinitd_service_log("[pid=\"%i\"] non-service child exit\n", infop.si_pid);
		}
		else {

			lxinitd_service_restart(service);

		}
		
	}
	
	return LITESH_OK;
}

/**
 * Check all services with pid_files are running
 */
static int
lxinitd_service_check() {

	lxinitd_service_t *service;

	for ( int i = 0 ; i < lxinitd_services.size; i++ ) {
		service = lxinitd_services.table[i];
		if ( service->pid == 0 ) {
			lxinitd_service_read_pid_file(service);
		}
		else if ( service->pid_file && kill(service->pid, 0) ) {
			if (errno == ESRCH) {
				// pid has died
				lxinitd_service_restart(service);
			}
		}
	}
	
	alarm(LXINITD_CHECK_INTERVAL);
	
	return LITESH_OK;
}

/**
 * read a service's pid file, if it exists.
 */
int
lxinitd_service_read_pid_file(lxinitd_service_t *service) {

	char   pid_s[10];
	char   c;
	int    in, pid, rc;
	
	in = open(service->pid_file, O_RDONLY);
	if ( in == -1 ) {
		return LITESH_ERROR;
	}
	
	for (int pos = 0; pos < 10 ; pos++) {
		rc = read(in, &c, 1);

		if ( rc == 0 ) {
			// EoF
			pid_s[pos] = '\0';
			pid = atoi(pid_s);
			if (pid > 0) {
				service->pid = pid;
				lxinitd_service_log("[a@1 service=\"%s\" pid=\"%i\"] monitoring\n", service->argv[0], service->pid);
				return LITESH_OK;
			}
		}
		
		if ( rc < 0 ) {
			return LITESH_ERROR;
		}
		
		pid_s[pos] = c;
	}
	
	return LITESH_ERROR;
}



/**
 * spawn a process, registering it as a service so that lxinitd_service_wait() can restart it if necessary.
 *
 * @param argv ["respawn", binary, ...]
 */
int
lxinitd_builtin_respawn(char **argv)
{
	int                 pid;
	lxinitd_service_t   *service;
	
	if ( litesh_parse_argv_count(argv) < 2 ) {
		fprintf(stderr, "service missing arguments\n");
		return LXINITD_SERVICE_ERROR;
	}
	pid = fork();

	if ( pid == 0 ) {
		// child

		execvp(argv[1], (char * const*)argv + 1);

		// execvp should not return, if the service started correctly
		fprintf(stderr, "error '%s': %s\n", argv[1], strerror(errno));
		
	}
	// parent
	else if ( pid < 0 ) {
		fprintf(stderr, "failed to respawn '%s': %s\n", argv[0], strerror(errno));
		return LXINITD_SERVICE_ERROR;
	}
	
	service = lxinitd_service_create(pid, (const char **) argv + 1);
	if (service == NULL ) {
		fprintf(stderr, "failed to register service '%s'\n", argv[0]);
		return LXINITD_SERVICE_ERROR;
	}
	lxinitd_service_log("[a@1 service=\"%s\" pid=\"%i\"] started\n", service->argv[0], service->pid );
	
	return LITESH_OK;
}


/**
 * spawn a process, registering it as a service so that litesh_service_wait() can restart it if necessary.
 * pid is read from a pid_file generated by the deamon itself.
 * 
 * @param argv ["service", pid_file, boot_stript, ...]
 */
int
lxinitd_builtin_respawn_pid_file(char **argv)
{
	int                 pid;
	lxinitd_service_t   *service;
	
	if ( litesh_parse_argv_count(argv) < 3 ) {
		fprintf(stderr, "service missing arguments\n");
		return LXINITD_SERVICE_ERROR;
	}
	pid = fork();

	if ( pid == 0 ) {
		// child

		execvp(argv[2], (char * const*)argv + 2);

		// execvp should not return, if the service started correctly
		fprintf(stderr, "error '%s': %s\n", argv[2], strerror(errno));
		
	}
	// parent
	else if ( pid < 0 ) {
		fprintf(stderr, "failed to respawn '%s': %s\n", argv[0], strerror(errno));
		return LXINITD_SERVICE_ERROR;
	}
	
	service = lxinitd_service_create_pid_file((const char **) argv + 1);
	if (service == NULL ) {
		fprintf(stderr, "failed to register service '%s'\n", argv[2]);
		return LXINITD_SERVICE_ERROR;
	}
	lxinitd_service_log("[a@1 service=\"%s\" pid=\"%i\"] started\n", service->argv[0], service->pid );
	
	return LITESH_OK;
}
// SNIP_service
