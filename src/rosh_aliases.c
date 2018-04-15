/**
 * Aliases of unix utils that do not write to the filesystem.
 * 
 * Uses a similar approach to busybox.
 *
 * @author teknopaul
 */

#include "litesh.h"

#define ESC "\033"

static const char *alias_true =    "true";
static const char *alias_false =   "false";
static const char *alias_nologin = "nologin";
static const char *alias_sh =      "sh";
static const char *alias_clear =   "clear";

void
rosh_aliases(const char* argv[]) {

	const char* name = bb_basename(argv[0]);
	
	if ( strcmp(alias_true, name) == 0 ) {
		exit(0);
	}
	
	if ( strcmp(alias_false, name) == 0 ) {
		exit(1);
	}
	
	if ( strcmp(alias_nologin, name) == 0 ) {
		puts("This account is currently not available.");
		exit(1);
	}
	
	if ( strcmp(alias_sh, name) == 0 ) {
		puts("[\x1B[31merror\x1B[0m] this server does not support interactive commands\n");
		exit(1);
	}
	
	if ( strcmp(alias_clear, name) == 0 ) {
		puts(ESC"[H" ESC"[J");
		exit(0);
	}

}