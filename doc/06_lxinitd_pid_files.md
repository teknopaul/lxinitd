# lxinitd pid files

How lxinitd processes pid files.

If you configure a service with a pid file.

	service /var/run/nginx.pid /sbin/nginx

`lxinitd` will ensure the service is restarted if it fails. This page details how the monitoring works.

## fork()

Some services fork() a daemon process and exit when they boot, the `lxinitd` process does not know the pid to monitor.

A forked process is not a child process, so `lxinitd` can not necessarily wait for SIGCHLD signal to indicate it has exited.

These types of daemon write their pid to a file.

`lxinitd` periodically checks for the existence of the file until it knows the service's pid.

If the pid disappears from the ps list `lxinitd` restarts the process.

The existence of a service pid is checked every 3 seconds. This is called _check()_.

## re-parenting

When any process forks and the parent process terminates, the Linux kernel re-parents the orphaned process to pid 1.  `lxinitd` may _or may not_ be pid 1.

If re-parenting happens `lxinitd` _will_ be notified with SIGCHLD when the process dies. This is called _wait()_.

If `lxinitd` _is_ pid 1, both _wait()_ and _check()_ notify `lxinitd` of the same process's death; there is a race condition that must be fixed.

The filesystem's atomic operations are used. 

The first of _wait()_ and _check()_ to notice process has died deletes the pid file.

The second to notice the process has died, if the pid file is missing, does nothing.

If the pid file exists, _check()_ verifies the pid is alive.

If the pid file exists, and _check()_ fails, this indicates the process has failed again.

If an _external_ process deletes the pid file, `lxinitd` will reboot the process if it knows the pid, but may not be able to reboot it a second time.

