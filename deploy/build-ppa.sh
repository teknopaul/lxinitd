#!/bin/bash -e
#
# Upload github sources to PPA, run this after checking in to github
#
cd $(dirname $0)

function fail(){
    echo $1
    exit 1
}

. ./ppa-version
. ../version

echo "Building version $VERSION-$PPA_VERSION"

tmp_dir=/tmp/ppabuild

rm -rf $tmp_dir
mkdir $tmp_dir

git-build-recipe --distribution xenial --allow-fallback-to-native lxinitd.recipe $tmp_dir

debsign -k dpkg1 $tmp_dir/lxinitd_${VERSION}-${PPA_VERSION}_source.changes

ls -la $tmp_dir

if [ "$1" == --publish ]
then
  dput ppa:teknopaul/lxinitd  $tmp_dir/lxinitd_${VERSION}-${PPA_VERSION}_source.changes
fi
