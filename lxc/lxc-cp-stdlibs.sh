#!/bin/bash
#
# cp dependencies of a any binary in /bin or /sbin into /lib
#
set -e


lxc_base=/var/lib/lxc

append_ld_conf() { 
    locs=$(echo -n $locs; cat /etc/ld.so.conf.d/*.conf| grep -v '^#'| tr '\n' ' ')
}
usage() { 
    echo "Usage: $0 [-n lxc-name] [-r rootfs] -l" 1>&2
    echo "    -l  -  search libs in ld.so.conf dirs" 1>&2
    exit 2
}
die () {
    echo $*
    exit 1
}



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
        l)
            append_ld_conf
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

#
# sanity checks
#
if [ -z "$name" ]
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
    rootfs=$(grep '^lxc.rootfs *=' $lxc_base/$name/config)
    rootfs=${rootfs/lxc.rootfs*=/}
    rootfs=${rootfs// /}

    [ -d $rootfs  ] || die "dir not found: $rootfs"
fi


#
# Copy lib from host to container's $rootfs/lib
#
# @param 1 - library name
#
function copy_lib() {
    found=$1
    if [ -f $found ]
    then
        mkdir -p $(dirname $rootfs/$found)
        test -f "$rootfs/$found" || cp -v $found "$rootfs/$found" && return 0
    fi

}

#
# List all the libs of executables in a given directory
#
# @param 1 - N - list of relative paths  e.g. bin sbin
#
function ls_libs() {
    while [ $# -gt 0 ]
    do
        for prog in $(find $rootfs/$1 -type f -executable)
        do
            if file $prog | grep ASCII >/dev/null 2>/dev/null
            then
                continue
            fi
            ldd $prog | awk '/^\t[^/]* => \// {print $3} /^\t\// {print $1} '
        done
        shift
    done
}

#
# Copy libraries that any executable in /bin or /sbin is dependent on, to /lib.
#
cd $rootfs
for lib in $(ls_libs bin sbin | sort | uniq)
do
    copy_lib $lib
done
