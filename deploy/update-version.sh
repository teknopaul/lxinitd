#!/bin/bash
#
#
#
cd $(dirname $0)

. ../version

major=${VERSION%.*}
minor=${VERSION#*.}

sed -i "s/#define LITESH_MAJOR_VERSION .*/#define LITESH_MAJOR_VERSION    $major/" ../src/litesh.h
sed -i "s/#define LITESH_MINOR_VERSION .*/#define LITESH_MINOR_VERSION    $minor/" ../src/litesh.h

grep --color '#define LITESH_.*_VERSION' ../src/litesh.h
