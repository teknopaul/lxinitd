
#ifndef LXINITD_SERVICE_LOG_H
#define LXINITD_SERVICE_LOG_H

#define LITESH_MAX_LOG_LEN       200                     // max length of a syslog message
#define LITESH_SERVICE_LOG       "/var/log/service.log"  // not configurable, you cant write to files

// SNIP_service_log.c

int lxinitd_service_log_init();
int lxinitd_service_log(char *format, ...);
int lxinitd_service_log_off();

// SNIP_service_log.c


#endif // LXINITD_SERVICE_LOG_H
