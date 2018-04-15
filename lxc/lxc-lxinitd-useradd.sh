#!/bin/bash
#
# Script to add a new user to an LXC continer
#
set -e

usage() {
    echo "Usage: $0 [-n lxc-name] [-r rootfs] -u username -p passwod -i uid" 1>&2
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

while getopts ":n:r:u:p::i" opt
do
    case "$opt" in
        n)
            rootdir=$lxc_base/$OPTARG
            ;;
        r)
            rootdir=$OPTARG
            ;;
        i)
            id=$OPTARG
            ;;
        u)
            username=$OPTARG
            ;;
        p)
            password=$OPTARG
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

rootfs=$rootdir/rootfs

cd $rootfs || die "can't cd to $rootfs"

id=${id:=1000}
username=${username:=bob}
password=${password:=password}

echo "$username:x:$id:$id:$username:/:/bin/sh" >> etc/passwd
echo "$username:x:$id:$username" >> etc/group

pass=$(perl -e "print crypt(\"$password\",\"\\\$6\\\$wibble\\$\") . \"\\n\"")
echo "$username:$pass:17157:0:99999:7:::" >> etc/shadow

