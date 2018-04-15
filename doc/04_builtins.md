# builtins

Builtins are commands that are available within the script runtime.

As with bash they do not require an external binary to run, the command s are "built in" to the shell executable.

### cd

Change currenty directory

example:

	cd /tmp


### echo

Similar to bash `echo`.

example:

	echo hello world

### error

`echo` to stderr.

example:

	error booting servers

### setuid

Set current user id, `setuid` takes 1 or 2 numeric arguments, `uid` and optionally `gid`.

If the script is running as root this changes the processes effective user id, you cannot change back.

example:

	setup 1000 1000
	/home/ubuntu/bin/server.sh

### exec

Similar to bash `exec` but without redirects

	exec /sbin/xtomp

### spawn

Execute a process in the background, akin to bash's trailing `&`

example:

	spawn /sbin/xtomp
	spawn /usr/bin/rsyslog

### reexec

Execute a process, if it ever terminates, restart it.
This must be the last line of a script, it never returns.

	reexec /bin/getty -L tty1 115200 vt100

### respawn

Start a process that, if it ever stops, will be restarted.
`lxinitd` supports up to 20 services.

example:

	respawn /bin/getty -L tty1 115200 vt100
	respawn /usr/bin/rsyslog

