#!/bin/bash
#
# Libray relocator for LXC, copy everything except /etc && /lib dirs that are mapped in the container.
#
set -e

lxc_base=/var/lib/lxc

usage() { 
    echo "Usage: $0 [-n lxc-name] [-r rootfs] -c xxx.deb" 1>&2
    echo "    -c  -  include /etc, overwriting existing config" 1>&2
    exit 2
}
die () {
    echo $*
    exit 1
}


if [ ! -d $lxc_base ]
then
    echo "cannot find: $lxc_base"
    exit 1
fi

#
# Process cli args
#
name=
rootdir=
rootfs=
inc_config=

while getopts ":n:r:c" opt
do
    case "$opt" in
        n)
            name=$OPTARG
            rootdir=$lxc_base/$OPTARG
            ;;
        r)
            rootfs=$OPTARG
            ;;
        c)
            inc_config=true
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

deb=$1

if [ -z "$name" ]
then
    usage
fi

[ -f "$deb" ]    || die "file not found: $deb"
[ -d $rootdir  ] || die "dir not found: $rootdir"

#
# Find the rootfs according to the lxc configuration file.
#
rootfs=$(grep '^lxc.rootfs *=' $rootdir/config)
rootfs=${rootfs/lxc.rootfs*=/}
rootfs=${rootfs// /}

#
# extracting .deb except for directories that we mount in lxc. and /etc
#
if [ -z "$inc_config" ]
then
    excludes="--exclude=./etc" 
else
    excludes=""
fi
for mnt_dir in $(cat $rootdir/config  | awk '/^lxc.mount.entry = \//{ print $3 }')
do
    excludes="$excludes --exclude=.${mnt_dir} "
done

ar p $deb data.tar.xz | xz -d | tar -tv --directory=$rootfs --anchored $excludes
