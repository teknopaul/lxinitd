#!/bin/bash


cd $(dirname $0)

for name in true false nologin sh clear
do
    ln -s ../../target/rosh $name
done

