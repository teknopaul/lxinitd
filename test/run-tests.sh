#!/bin/bash
#
# Run the tests suite
#
function die() {
    echo $*
    exit 1
}

cd $(dirname $0)

#
# test echo
#
/bin/rosh echo.rosh | grep 'foo $USER' >/dev/null || die "echo.rosh borked"

#
# test error
#
/bin/rosh error.rosh 2>/dev/null | grep 'long tings' && die "error to stdout"
/bin/rosh error.rosh 2>&1 | grep 'long tings' | cat >/dev/null || die "error.rosh borked"

#
# test running other scripts
#
rm -f output.txt
/bin/rosh test.rosh || die "test.rosh failed"
test -f output.txt || die "run1.sh failed"
rm output.txt

#
# test changing directory
#
rm -f subdir/output.txt
/bin/rosh cd.rosh || die "cd.rosh failed"
test -f subdir/output.txt || die "cd output failed"
rm subdir/output.txt

#
# test making a directory
#
rm -f subdir1
/bin/rosh mkdir.rosh || die "mkdir.rosh failed"
test -d subdir1 || die "mkdir failed"
rmdir subdir1

#
# test exec, listesh should not be visible in the ps list
#
/bin/rosh exec.rosh >/dev/null || die "exec.rosh failed" &
sleep 0.1
ps -efwww | grep "rosh exec.rosh" >/dev/null || die "exec.rosh not visible"
sleep 1
ps -efwww | grep "rosh exec.rosh" | grep -v grep && die "exec.rosh visible"
wait

#
# test reexec
#
./reexec.rosh 2>/dev/null &
sleep 2
ps -efwww | grep ./reexec.rosh | grep -v grep | grep rosh | cut -c 10-15 | xargs kill -9 || die "where did ./reexec.rosh go?"


#
# test respawn
#
rm -f ./tmp.log
./respawn.rosh 2> ./tmp.log &
pid=$(ps -efwww | grep ./respawn.rosh | grep -v grep | grep rosh | cut -c 10-15)

# HUP prints service table to sdterr
kill -SIGHUP  $pid || die "HUP failed"
sleep 1
[ $(grep -c "pid .* proc" ./tmp.log) -eq 1 ] || die "service list not printed"
rm ./tmp.log
kill -SIGTERM $pid || die "TERM failed"

#
# test sleep alias
#
./sleep.rosh


#
# test pidfile based services
#
rm -f  ./service.pid
./service.rosh &
sleep 1
test -f ./service.pid || die "./service.sh did not create a pid file"
sleep 7
ps -efwww | grep ./service.sh | grep -v grep >/dev/null || die "./service.sh was not restarted"
ps -efwww | grep ./service.rosh | grep -v grep | grep rosh | cut -c 10-15 | xargs kill -9 || die "where did ./service.rosh go?"
rm -f  ./service.pid

mkdir -p etc

../target/lxpasswd 1000 admin 'password'

