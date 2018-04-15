# lxpasswd

`lxpasswd` is a utility for creating users inside a chroot.

It can be run without having to have the LXC container running or have the standard Linux utilities available.

You dont have to have any files in the container yet.

`lxpasswd` appends to `./etc/passwd`, `./etc/group`, and `./etc/shadow`.  

N.B. **not** using absolute paths `/etc/...`.

Example: 

	cd /var/lib/lxc/mycontainer/rootfs
	
	lxpasswd 0 root
	lxpasswd 1000 bob   '$whatEvA'
	lxpasswd 1001 alice '0hhhArrh'

The first arguments is the userid _and_ the groupid, the second argument is the username, the third argument is a plain text password. If password is ommitted `lxpasswd` puts `*` in `etc/shadow` indicating that this user can not login with a password, (common for root user).



To create passwords when inside a chroot or virtual machine cd to `/` first.

	cd /
	lxpasswd 1000 bob '$whatEvA'


`lxpasswd` is linked statically so you don't need to have `/lib` mounted yet.

	rootfs=/var/lib/lxc/mycontainer/rootfs
	cp /bin/lxpasswd $rootfs/bin
	
	chroot $rootfs /bin/lxpasswd 1000 bob '$whatEvA'

This creates the minimal entries for login

	etc/group
	  bob:x:1000:bob
	  
	etc/passwd
	  bob:x:1000:1000:bob:/home/bob:/bin/sh
	  
	etc/shadow
	  bob:$6$PnMNUaj$Dt.jgDm1fSgfiznupMlZsY7m6iqcS9URPNQoWzkt2OszC0G2KoidhYdSxdgKkdpeOElb8t8meeU7gyGnxbrnH1:1986:0:99999:7:::

N.B. `lwpasswd` currently does no validation that the arguments provided are sane: if you run it when a given UID exists you will create duplicate entries.



