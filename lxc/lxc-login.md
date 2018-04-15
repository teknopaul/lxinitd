# Linux login process


getty
	opens a tty port, prompts for a login name and invokes /bin/login
	handles /etc/issue
	--autologin bypasses passwords

/bin/login   reads /etc/passwd and chums
also integrates with pam, so we need pam


check ENCRYPT_METHOD SHA512 make sure passwords are OK in /etc/passwd

cd $rootfs

cp /etc/login.defs  etc/
cp /bin/{su,login}  bin/
cp /sbin/{getty}    sbin/

references

mkdir -p  var/mail var/log/btmp etc/pam.d/ etc/security

# ./limits.conf must exist
touch var/log/btmp etc/security/limits.conf

cp /etc/pam.d/login etc/pam.d/


/etc/shells
# /etc/shells: valid login shells
/bin/sh
/bin/dash
/bin/bash
/bin/rbash
