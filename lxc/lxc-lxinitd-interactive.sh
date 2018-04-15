#!/bin/bash
#
# Script to make a non-interactive lxinitd LXC container interactive. 
# By interactive we mean you can login wtih lxc-console.
#
set -e

usage() { 
    echo "Usage: $0 [-n lxc-name] [-r rootfs]" 1>&2
    exit 2
}
die () {
    echo $*
    exit 1
}

lxc_base=/var/lib/lxc

if [ ! -d $lxc_base ]
then
    echo "cannot find: $lxc_base"
    exit 1
fi

rootdir=

while getopts ":n:r:" opt
do
    case "$opt" in
        n)
            rootdir=$lxc_base/$OPTARG
            ;;
        r)
            rootdir=$OPTARG
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

rootfs=$rootdir/rootfs

cd $rootfs || die "can't cd to $rootfs"

#
# getty & /bin/login setup setup
#

pc /bin/login   bin/
cp /bin/su      bin/
cp /sbin/getty  sbin/

mkdir -p  var/mail etc/security etc/default

echo 'PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"' > etc/environment

echo '# /etc/shells: valid login shells
/bin/sh
/bin/bash
/bin/rbash
/bin/rosh
/bin/lxmenu
' > etc/shells

echo '
MAIL_DIR         /var/mail
FAILLOG_ENAB     no
LOG_UNKFAIL_ENAB no
LOG_OK_LOGINS    no
SYSLOG_SU_ENAB   yes
SYSLOG_SG_ENAB   yes
SU_NAME          su
HUSHLOGIN_FILE   .hushlogin
ENV_SUPATH       PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
ENV_PATH         PATH=/usr/local/bin:/usr/bin:/bin
TTYGROUP         tty
TTYPERM          0600
ERASECHAR        0177
KILLCHAR         025
UMASK            022
PASS_MAX_DAYS    99999
PASS_MIN_DAYS    0
PASS_WARN_AGE    7
UID_MIN          1000
UID_MAX          60000
GID_MIN          1000
GID_MAX          60000
LOGIN_RETRIES    5
LOGIN_TIMEOUT    60
CHFN_RESTRICT    rwh
DEFAULT_HOME     yes
USERGROUPS_ENAB  yes
ENCRYPT_METHOD   SHA512
' > etc/login.defs

#
# PAM setup
#
touch etc/security/limits.conf
touch etc/security/pam_env.conf

mkdir -p etc/pam.d/

cp /etc/pam.d/login \
   /etc/pam.d/sudo \
   /etc/pam.d/su \
   /etc/pam.d/common-auth \
   /etc/pam.d/common-account \
   /etc/pam.d/common-password \
   /etc/pam.d/common-session \
   /etc/pam.d/common-session-noninteractive \
  etc/pam.d/

cp /etc/default/locale  etc/default

