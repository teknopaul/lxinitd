# lxmenu

a shell that allows executing only a few configurable commands.

`/bin/lxmenu` provides an interactive shell, the user can only run a list of commands specified in a configuration file (`/etc/lxmenu.conf` by default)

The configuration file should be Nagios nrpe command syntax.

e.g.

	command[ps]=ps -efwww
	command[disk]=du -sk /var

When the user is presented an lxmenu shell they are given a prompt.

	[lxmenu:25292]
	lxmenu> 

The commands available are 

- `help` - print the commands available
- `exit` - exit the shell
- `wait` - wait for background processes
- `echo` - echo text

and commands defined in the `/etc/lxmenu.conf`.

Commands do not accept arguments.

The commands can be suffixed with `&`, as in a bash shell, to execute a command in the background.

For example

	lxmenu> backupvar &
	started
	lxmenu> backupetc &
	started
	lxmenu> backupdata &
	started
	lxmenu> wait

`lxmenu` will wait until the background jobs have finished and then the user can exit.

The purpose of `lxmenu` is to provide a very limited shell for LXC containers. By copying the required utilities to `/bin` and symlinking `/bin/sh` to `/bin/lxmenu`, LXC containers can provide a container specific admin UI available with

	lxc-console -n mycont

Users cannot login to the container to run arbitrary commands. This is similar to `rbash` but less powerful and harder to break out of.

## Other uses

`lxmenu` is not limited to LXC containers, it can be used for a general admin shell

	admin:x:1000:1000:admin:/home/admin:/bin/lxmenu

available locally or over ssh.

It can also be used to limit access to specific commands in a production environment, to avoid fat fingered ops (yourself) from making a mistake.

By providing the command

	command[bash]=bash

you can break out to a full shell if required.

To provide different configurations you can symlink the binary to any name starting with `lx`, it will read the equivalent config file in `/etc`.

For example, if you symlink (or rename) the binary to `lxadmin`, it will read commands from `/etc/lxadmin.conf`

## Command file syntax

The configuration file syntax is based on Nagios nrpe command syntax.

e.g.

	command[ps]=ps -efwww
	command[disk]=du -sk /var

However its implemented with _litesh_ so _litesh_ whitespace restrictions apply. ONE space separaces args no `\t` no whitespace prefixes.

Command names should match the regexp `[a-z0-9-]+`

Blank lines and lines starting with # are ignored.  Scripts can be made executable with the shebang

	#!/bin/lxmenu

