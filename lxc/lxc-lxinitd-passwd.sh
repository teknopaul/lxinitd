#!/bin/bash
#
# Set root password in a LXC continer
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

while getopts ":n:r:u:p::i" opt
do
    case "$opt" in
        n)
            rootdir=$lxc_base/$OPTARG
            ;;
        r)
            rootdir=$OPTARG
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

echo -n "enter root password: "
read password

if [ -n "$password" ]
then
    lxpassswd 0 root $password
fi

