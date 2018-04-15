/**
 * Create a new user for an LXC container.
 * 
 * This is designed to work in a chroot. It writes to ./etc, to use inside a container  cd /  first.
 *
 * @author teknopaul
 */

#include <stdlib.h>
#include <crypt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>


static const char charset[] = "0123456789./abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

static void
rand_salt(char *dest, size_t length) {

	*dest++ = '$';
	*dest++ = '6';
	*dest++ = '$';
	length -= 5;
	while (length-- > 0) {
		size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
		*dest++ = charset[index];
	}
	*dest++ = '$';
	*dest = '\0';
}

/**
 * @param uid userid and gid
 * @param username
 * @param password or NULL
 */
static int
lxpasswd(int uid, const char *username, /*@null@*/ const char *password) {
	FILE        *fd;
	struct stat stat_s;

	char  salt[12];
	char *enc_pass;

	memset(&stat_s, 0, sizeof(struct stat));
	memset(salt, 0, 12);

	if ( stat("etc", &stat_s) != 0 ) {
		fprintf(stderr, "unable to read ./etc");
		return 1;
	}

	if ( password ) {
		rand_salt(salt, 12);
		enc_pass = crypt(password, salt);
	}
	else {
		enc_pass = "*";
	}

	fd = fopen("etc/shadow", "a+");
	if ( fd ) {
		fprintf(fd, "%s:%s:1986:0:99999:7:::\n", username, enc_pass);
		fclose(fd);
	}

	fd = fopen("etc/group", "a+");
	if ( fd ) {
		fprintf(fd, "%s:x:%i:%s\n", username, uid, username);
		fclose(fd);
	}

	fd = fopen("etc/passwd", "a+");
	if ( fd ) {
		fprintf(fd, "%s:x:%i:%i:%s:/home/%s:/bin/sh\n", username, uid, uid, username, username);
		fclose(fd);
	}
	
	// try to make home directory
	if ( chdir("home") == 0 ) mkdir(username, 0750);
	
	return 0;
}

int
main(int argc, const char* argv[])
{
	if (argc == 4) {
		return lxpasswd(atoi(argv[1]), argv[2], argv[3]);
	}
	if (argc == 3) {
		return lxpasswd(atoi(argv[1]), argv[2], NULL);
	}

	fprintf(stderr, "Create etc/passwd, etc/shadow, etc/group entries for a new user\n"
					"Usage: %s uid username 'password'\n", argv[0]);
	return 1;
	
}