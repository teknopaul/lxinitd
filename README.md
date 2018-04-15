<img src="doc/lxinitd-text-small.png" style="float:right"/>
# lxinitd

A shell for starting LXC containers.

The idea is to provide enough of a scripting environment to init processes (and keep them alive) but no more.

The project consists of `rosh` a readonly shell and `lxinitd` extensions for `/sbin/init` and `lxmenu`, providing tools to view inside an LXC container _without_ a full featured shell such as [bash](https://www.gnu.org/software/bash/).

`lxinitd` provides a minimal [systemd](https://www.freedesktop.org/wiki/Software/systemd/) alternative.

topAll projects use a common scripting syntax called "litesh".

# Intall

	make
	sudo make install
	
Or install the `.deb`

Create containers with 

	lxc-create -t lxinitd

## Basic Syntax

"litesh" scripts contain one command per line with arguments.

Here's an example from an lxc container.

	#!/sbin/init
	
	# boot script
	
	/sbin/mount -a
	/sbin/ip addr add 10.0.3.27 dev eth0
	/sbin/ip route add default via 10.0.3.1
	
	service /run/xtomp.pid /sbin/xtomp

Blank lines are ignored, lines that start with `#` are comments.

Each command is executed as a subprocess, unless it is a [builtin](http://lxinitd.tp23.org/manual/04_builtins.html).

`rosh` is deliberatly limited.

* Its not interactive, it can only run scripts.
* No ability to create files, no pipes or redirects.

[full syntax](http://lxinitd.tp23.org/manual/03_syntax.html).


## Whats the point?

This project was originally created post [shell-shock](http://seclists.org/oss-sec/2014/q3/650) as a shell with the minimal requirements for initalizing an LXC container. Subsequently it matured into a full init system.

The idea is to be able to create _configurable_ (not hardcoded) Linux init scripts for containers without having to put  `bash` or `sh` inside the container.

Docker promotes single process per container, LXC provides a whole OS of tools.  With `lxinitd` and `rosh`you can build something in between. 
Very small containers with the tools you want, just by copying files around in the host.

# /sbin/init

`lxinitd` init is inspired by [busybox](http://busybox.net), (and borrows some code) it takes a slightly different approach.

With busybox you start with a full toolkit and exclude unused utilities, with `lxinitd` you start minimal and build up.

To remove utilities in `busybox` you recompile the code setting flags. With `lxinitd` adding utilities is done by copying files in to `/var/lib/lxc/cont/rootfs/`.

`lxinitd` init is targeted specifically for LXC container init, rather than physical machine init. For example, it does not differentiate runlevels.

[init](http://lxinitd.tp23.org/manual/00_lxinitd.html).
