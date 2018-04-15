# rosh aliases

`rosh` can be aliased (symlinked) to act like a couple of standard unix utils.

Uses a similar approach to [busybox](https://busybox.net/), and some code from busybox.

All commands supported do not write to the filesystem.

For example `nologin` is typical in  `/etc/passwd` to prevent logins.

## Usage

	cd bin; ln -s rosh nologin

## Aliases

### true

Return 0

### false

Return 1

### nologin

Print a polite message and exit.

### clear

Clear the terminal

### sh

Print a polite message and exit. i.e. you can symlink rosh to `/bin/sh` and logins will be prevented.

