#!/bin/bash -e
#
# Build a binary .deb package for lxinitd
#
test $(id -u) == "0" || (echo "Run as root" && exit 1) # requires bash -e

#
# The package name
#
name=lxinitd
arch=$(uname -m)

cd $(dirname $0)/..
project_root=$PWD

#
# Create a temporary build directory
#
tmp_dir=/tmp/$name-debbuild
rm -rf $tmp_dir
mkdir -p $tmp_dir/DEBIAN

make install DESTDIR=$tmp_dir

. ./version
sed -e "s/@PACKAGE_VERSION@/${VERSION}/" $project_root/deploy/DEBIAN/control.in > $tmp_dir/DEBIAN/control

size=$(du -sk ${tmp_dir} | cut -f 1)
sed -i -e "s/@SIZE@/$size/" $tmp_dir/DEBIAN/control

#
# Setup the installation package ownership
#
chown root.root $tmp_dir

#
# Build the .deb
#
dpkg-deb --build $tmp_dir target/$name-$VERSION-1.$arch.deb

test -f target/$name-$VERSION-1.$arch.deb

echo "built target/$name-$VERSION-1.$arch.deb"

if [ -n "$SUDO_USER" ]
then
  chown $SUDO_USER target/$name-$VERSION-1.$arch.deb
fi

rm -rf $tmp_dir
