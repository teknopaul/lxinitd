

#include "litesh_test.h"
#include "../lxinitd_service.h"
#include "../lxinitd_service_log.h"


/**
 * test hook
 */
int
main(int argc, const char* argv[])
{
	lxinitd_service_t   *service;
	
	equals("init", LITESH_OK, lxinitd_service_services_init());
	equals("init service size", 0, lxinitd_service_size());
	
	const char *litesh_argv[LITESH_MAX_ARGS];
	
	/*
	 * Test copying arguments
	 */
	memset(litesh_argv, 0, sizeof(void*) * LITESH_MAX_ARGS);
	
	litesh_argv[0] = "xtomp\0-f\0/etc/xtomp.conf";
	litesh_argv[1] = litesh_argv[0] + 6;
	litesh_argv[2] = litesh_argv[1] + 3;
	service = lxinitd_service_create(21, litesh_argv);
	assert("not null", service != NULL);
	
	equals("pid", 21, service->pid);
	equals("copy1", 0, strcmp("xtomp", service->argv[0]));
	equals("copy2", 0, strcmp("-f", service->argv[1]));
	equals("copy3", 0, strcmp("/etc/xtomp.conf", service->argv[2]));
	assert_null("null4", service->argv[3]);

	/*
	 * Test copying arguments
	 */
	memset(litesh_argv, 0, sizeof(void*) * LITESH_MAX_ARGS);
	
	litesh_argv[0] = "xtomp\0-v";
	litesh_argv[1] = litesh_argv[0] + 6;
	service = lxinitd_service_create(22, litesh_argv);
	assert("not null", service != NULL);
	
	equals("copy1", 0, strcmp("xtomp", service->argv[0]));
	equals("copy2", 0, strcmp("-v", service->argv[1]));
	assert_null("null3", service->argv[2]);

	/*
	 * Test copying arguments
	 */
	memset(litesh_argv, 0, sizeof(void*) * LITESH_MAX_ARGS);
	
	litesh_argv[0] = "xtomp";
	service = lxinitd_service_create(23, litesh_argv);
	assert("not null", service != NULL);
	
	equals("copy1", 0, strcmp("xtomp", service->argv[0]));
	assert_null("null 2", service->argv[1]);

	equals("service size", 3, lxinitd_service_size());
	assert_same("find", service, lxinitd_service_find(23));
	
	/*
	 * add
	 */
	lxinitd_service_print(stdout);
	
	/*
	 * negs
	 */
	memset(litesh_argv, 0, sizeof(void*) * LITESH_MAX_ARGS);
	
	litesh_argv[0] = "";
	service = lxinitd_service_create(23, litesh_argv);
	assert_null("invalid service should be null", service);
	
	lxinitd_service_log("[a@1 pid=\"%i\"] blah blah\n", 23);
	
	/*
	 * test reading pid file
	 */
	litesh_argv[0] = "xtomp";
	service = lxinitd_service_create(23, litesh_argv);
	
	int fd = open("./test.pid", O_WRONLY | O_CREAT, 0660);
	if (fd < 0) puts("open borked");
	if ( write(fd, "1234\n", 5) < 0) puts("write borked");
	service->pid_file = "./test.pid";
	lxinitd_service_read_pid_file(service);
	equals("pid file read", 1234, service->pid);
	unlink("./test.pid");
	
		
	/*
	 * test creating pid_file service
	 */
	memset(litesh_argv, 0, sizeof(void*) * LITESH_MAX_ARGS);
	
	litesh_argv[0] = "/var/run/xtomp.pid\0/sbin/stomp\0-d";
	litesh_argv[1] = litesh_argv[0] + 19;
	litesh_argv[2] = litesh_argv[0] + 31;
	service = lxinitd_service_create_pid_file(litesh_argv);
	assert("not null", service != NULL);

	equals("pid_file copy1", 0, strcmp("/sbin/stomp", service->argv[0]));
	equals("pid_file copy1", 0, strcmp("-d", service->argv[1]));
	assert_null("pid_file null2", service->argv[2]);
	assert_null("pid_file null3", service->argv[3]);
	equals("pid_file pid", 0, service->pid);
	equals("pid_file copy2", 0, strcmp("/var/run/xtomp.pid", service->pid_file));

	return errors ? 1 : 0;
}
