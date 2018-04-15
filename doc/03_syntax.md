# syntax

 `lxinitd`, `lxmenu`, and `rosh` share the same _litesh_ syntax.

## Basic Syntax

_litesh_ scripts contain one command per line with arguments.

Here's an example from an lxc container `/etc/init.d/rcS`.

	#!/bin/rosh
	
	/sbin/mount -a
	/sbin/ip addr add 10.0.3.27 dev eth0
	/sbin/ip route add default via 10.0.3.1
	# start the process
	/sbin/xtomp

Blank lines are ignored, lines that start with `#` are comments.

Each line is executed as a subprocess, except limited builtins.

Commands are executed with `execv()` so they repect `$PATH` and require absolute paths or `./`.

## Shebang

The shebang (`#!/bin/rosh`) is required, even if started with `/bin/rosh script`

The shebang does not accept arguments or switches.

## Textual content

The following rules apply to the content of scripts

* max 2048 line length
* max 10 arguments to a command
* ascii 7bit only, no tabs, no invisible chars apart from space ' ' and \n
* no \0 characters
* no utf-8


## Whitespace

`\n` _exclusivly_ splits commands, use of `\r` is an error.

**ONE** space character (ascii 20) _exclusivly_ splits args.

The double quote character `"` is used to wrap strings with spaces.  To print quote or slash use standard C string escaping `\\` and `\"`.

## Builtins

builtins are commands that can be executed without `./`

[builtins](04_builtins.html)

The builtins available are dependent on the context.