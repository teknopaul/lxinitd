# lxinitd services

In `lxinitd` has simple service management.

`lxinitd` have two states, either process is running or its not: `lxinitd` will try to restart it.

`lxinitd` keeps track of the number of times it reboots a process.

The first time it reboots a process it will be instantaneous, subsequent reboots will have a slight delay to prevent a process that continuously fails from taking all the CPU.
e.g. a process fails because it runs out of diskspace, will likely not restart.
`lxinitd` will keep retrying to start it indefinitely.

## respawn

The `respawn` builtin restarts a process if it exits, almost immediately.

example

	respawn /sbin/getty -L tty1 115200 vt100

## service

The `service` builtin, checks for the creation of a pid file, it reads the pid from this file and monitors the process's existence every 3 seconds.  

example

	service /run/nginx.pis /sbin/nginx

If the process dies it will be restarted.  If `lxinitd` is pid 1, i.e. the `/sbin/init` process started by the Linux kernel, it will restart the process immediately since all processes are its children.  `lxinitd` can be run from the command line, in which case, it may take 3 seconds to restart the process.
