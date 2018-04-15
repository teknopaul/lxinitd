#!/bin/bash
#
# Cp dependencies of an executable
#
set -e

lxc_base=/var/lib/lxc


append_ld_conf() { 
    locs=$(echo -n $locs; cat /etc/ld.so.conf.d/*.conf| grep -v '^#'| tr '\n' ' ')
}

usage() { 
    echo "Usage: $0 [-n lxc-name] [-r rootfs] executable" 1>&2
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
rootfs=

while getopts ":n:r:l" opt
do
    case "$opt" in
        n)
            name=$OPTARG
            ;;
        r)
            rootfs=$OPTARG
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

executable=$1

#
# sanity checks
#
if [ -z "$name" ]
then
    usage
fi

if [ -z "$executable" ]
then
    usage
fi

if [ ! -d $lxc_base ]
then
    echo "cannot find: $lxc_base"
    exit 1
fi


#
# Find the rootfs according to the lxc configuration file.
#
if [ -z "$rootfs" ]
then
    rootfs=$(grep '^lxc.rootfs *=' /var/lib/lxc/$name/config)
    rootfs=${rootfs/lxc.rootfs*=/}
    rootfs=${rootfs// /}

    [ -d $rootfs  ] || die "dir not found: $rootfs"
fi


function copy_lib() {
    found=$1
    if [ -f $found ]
    then
        mkdir -p $(dirname $rootfs/$found)
        test -f $rootfs/$found || cp -v $found $rootfs/$found && return 0
    fi
}

function ls_libs() {
    ldd $executable | awk '/^\t[^/]* => \// {print $3} /^\t\// {print $1} '
}

#
# Copy libraries that $executable is dependent on
#
cd $rootfs
for lib in $(ls_libs)
do
    copy_lib $lib
done
