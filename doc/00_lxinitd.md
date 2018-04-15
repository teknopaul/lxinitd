# lxinitd

an init process for LXC containers, configurable with a single file `/etc/rc.local`.

`/etc/rc.local` is a litesh syntax script with the [builtins](02_builtins.html) `spawn`, `respawn`, and `service`.

example:

	#!/bin/lxinitd
	
	respawn /sbin/getty -L tty1 115200 vt100
	
	service /var/run/xtomp.pid /sbin/xtomp

"Services" are simply processes that are restarted if they fail.

When running services `lxinitd` will not return until it is sent either SIGUSR1, SIGTERM or SIGKILL.  
SIGUSR1, SIGTERM will be forwarded to all services.

If you HUP the lxinitd process it will print the service table to stderr.

	pid reboot proc
	2 0 /sbin/xtomp


## Usage as /sbin/init

When `lxinitd` is sylinked and run as `/sbin/init` (pid 1) without a script argument, it will look for a `litesh` script called `/etc/rc.local`.  
N.B. this is _not_ a bash script as it might be in other systems.

Thus, when a container is booted with `lxinitd` as `/sbin/init` it will execute the commands in `/etc/rc.local`, and nothing else.


	#!/bin/lxinitd
	
	ip addr add 10.0.3.100 dev eth0
	ip route add default via 10.0.3.1
	
	/usr/sbin/rsyslogd
	service /var/run/xtomp.pid /sbin/xtomp

This way xtomp will run in a container with no interactive shell available to it. With no `getty` instances there is no way to log into this container.

This script `lxc-lxinitd` is added to `/usr/share/lxc/templates/` enabling creating LXC containers of this type.

	lxc-create -t lxinitd -n mylxcguest

They have `/lib` mounted from the parent and very few binary files in side the container (`lxinitd`, `lxmenu`, `rosh`, `noopsh`) and the binary you are interested in running in a container.

`lxinitd` does not support runlevels, its either on or off. Use lxc commands to reboot the container.

N.B. If there are no `service` or `respawn` lines in `/etc/rc.local`,  `lxinitd` will terminate and the container will too.


## Usage as service container

`lxinitd` respawn features work outside of `/sbin/init`. This provides a simple alternative to system-v and systemd.

A file called, for example,  `service.sh` containing...

	#!/bin/lxinitd

	respawn /home/me/dev/maypp/bin/myapp-msg
	
	respawn /home/me/dev/maypp/bin/myapp

run with

	./serivce.sh

will create a process that keeps services alive and requires no futher configuration.

It can also be run inside a chroot jail.

## Logging

Since `/sbin/init`'s stdout is not generally visible, `lxinitd` will log information about services.

If the `lxinitd` has permissions, it will generate logs in `/var/log/service.log` in syslog rfc5424 format.  
There is no way to change the file written to.

This feature can be tested by setting `LITESH_LOG=true`. LITESH_LOG should not be used outside of testing, multiple instances of `lxinitd` can not write to the same file.

This feature can be disabled with the `nolog` builtin.

## User id

LXC supports [setting the user id of the](https://linuxcontainers.org/lxc/manpages/man5/lxc.container.conf.5.html#lbAL) `/sbin/init` process.

	lxc.init.uid
	lxc.init.gid

From the LXC docs: _Note that using a non-root UID when booting a system container will likely not work due to missing privileges_.  `listesh` supports dropping privileges in the middle of the script.

	#!/bin/lxinitd
	ip addr add 10.0.3.100 dev eth0

	setuid 1000 1000
	service /run/xtomp.pid /sbin/xtomp

This enables running the first few commands as root.  When restarting services the current UID will be set. YMMV.



## builtins

The [builtins](04_builtins.html) available for lxinitd scripts (rc.local) are defined in `lxinitd.c` in the function `lxinitd_execute_argv(char **argv)`

- sleep
- nolog
- setuid
- reexec
- spawn
- respawn
- service





