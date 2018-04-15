/**
 * Logging to /var/log/service.log
 * 
 * @author teknopaul
 */

#include "litesh.h"
#include "lxinitd_service_log.h"

// SNIP_service_log

/**
 * enable logging
 */
static int do_log;
static int log_open;
static int log_fd;

/**
 * init the service table
 */
int
lxinitd_service_log_init()
{
	
	do_log      = ( getpid() == 1 || getenv("LITESH_LOG") ) ? 1 : 0;
	log_open    = 0;
	
	return LITESH_OK;
}


/**
 * Simple logger, if it fails you get no logs, tan pis.
 * Hardcoded to syslog rfc5424.
 * 
 * <27>1 2018-04-01 01:03:12 localhost litesh 1 - [a@1 service="./service.sh" pid="0"] restarted service
 */
#define PREFIX_LEN 48
int
lxinitd_service_log(char *format, ...) {
	if ( !do_log ) return LITESH_OK;
	
	int        len;
	char       buf[LITESH_MAX_LOG_LEN + PREFIX_LEN];
	va_list    args;
	time_t     timer;
	struct tm *tm_info;

	
	if ( ! log_open ) {
		log_fd = open(LITESH_SERVICE_LOG, O_WRONLY | O_CREAT | O_APPEND, 0777);
		if ( log_fd == -1 ) {
			log_open = -1;
			return LITESH_ERROR;
		}
		log_open = 1;
	}
	
	if ( log_open == 1 ) {

		time(&timer);
		tm_info = localtime(&timer);
		
		// priority=5(notice) facility+3(daemon)  == 43

		strftime(buf, PREFIX_LEN, "<43>1 %Y-%m-%d %H:%M:%S localhost litesh 1 - ", tm_info);
		va_start(args, format);
		len = vsnprintf((char *)(buf + PREFIX_LEN - 1), 100, format, args);
		va_end (args);
		
		if ( write(log_fd, buf,  PREFIX_LEN + len - 1) ) return LITESH_ERROR;

	}
	
	return LITESH_OK;
}

int
lxinitd_service_log_off()
{
	
	do_log = 0;
	if (log_fd) close(log_fd);
	
	return LITESH_OK;
}
// SNIP_service_log
