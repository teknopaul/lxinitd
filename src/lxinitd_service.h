
#ifndef LXINITD_SERVICE_H
#define LXINITD_SERVICE_H


// return codes
#define LXINITD_SERVICES_FLUP    -3   // hit MAX_SERVICES
#define LXINITD_SERVICE_ERROR    -4   // error setting up services

// constants

#define LXINITD_MAX_SERVICES      20                      // max number of services lxinitd can respawn
#define LXINITD_RESPAWN_DELAY     0.1                     // slight delay respawning to prevent failing proc eating all the CPU
#define LXINITD_RESPAWN_DELAY_2   1                       // incremental backoff for repeatedly failing services
#define LXINITD_SHUTDOWN_TIMEOUT  60                      // if child processes done respond to SIGTERM in ~60 secs SIGKILL is sent
#define LXINITD_CHECK_INTERVAL    3                       // interval between checks of pid_file based services

// SNIP_service.c

typedef struct lxinitd_service_s {
	pid_t   pid;                   // current process id
	int     reboots;               // number of times service has been rebooted
	int     state;                 // 1 (true) running, 0 (false) shutdown
	char   *argv[LITESH_MAX_ARGS]; // arrary of char pointers, all pointers are in the same malloced space.
	char   *pid_file;              // process id file path
} lxinitd_service_t;

typedef struct lxinitd_service_table_s {
	int                 size;
	lxinitd_service_t   *table[LXINITD_MAX_SERVICES];
	time_t              uptime;
	pid_t               init_pid;
	int                 check_pid_files;
} lxinitd_service_table_t;

int lxinitd_service_services_init();
lxinitd_service_t* lxinitd_service_create(int pid, const char **argv);
lxinitd_service_t* lxinitd_service_create_pid_file(const char **argv);
lxinitd_service_t* lxinitd_service_find(int pid);
int lxinitd_service_add(lxinitd_service_t* service);
int lxinitd_service_size();
int lxinitd_service_wait();
void lxinitd_service_print(FILE *fd);
int lxinitd_service_read_pid_file(lxinitd_service_t *service);

int lxinitd_builtin_respawn(char **argv);
int lxinitd_builtin_respawn_pid_file(char **argv);

// SNIP_service.c


#endif // LXINITD_SERVICE_H
