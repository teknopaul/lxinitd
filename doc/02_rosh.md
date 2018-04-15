# rosh

a readonly shell.

`/bin/rosh` uses the _litesh_ [syntax)](03_syntax.html) to provide a scripting environment which is limited to executing commands.  There are no pipes or redirects meaning it can not be abused to corrupt the contents of a chroot if ther container is compromised.

## builtins

The [builtins](04_builtins.html) available for rosh are defined in `rosh.c` function `rosh_execute_argv(char **argv)`

- cd
- echo
- error
- sleep
- setuid
- exec
- spawn
- wait
- exit