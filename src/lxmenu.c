/**
 * lxmenu - a menu usystem for executing commands inside an LXC container.
 * 
 * This provides a feature to execute a process in the process group of the container guest.
 *
 * Commands to execute are defined in a file with Nagios nrpe syntax.
 *
 * @author teknopaul
 */

#include "litesh.h"
#include "lxmenu.h"

static int  lxmenu_add_command(char *name, char **argv, int len);
static int  lxmenu_parser_argv(char **argv);
static int  lxmenu_execute_argv(char **argv);
static void lxmenu_prompt();
static void lxmenu_welcome();

static lxmenu_table_t commands;

int
lxmenu_init(const char *cmd_list)
{
	
	memset(&commands, 0, sizeof(lxmenu_table_t));

	int in = open(cmd_list, O_RDONLY);
	if ( in == -1 ) {
		in = 0;
		fprintf(stderr, "cannot open lxmenu config: %s\n", cmd_list);
		return LITESH_ERROR;
	}
	int rv = litesh_parse_fd(in, lxmenu_parser_argv, LITESH_FAIL_ON_ERROR);
	close(in);
	return rv;

}

/**
 * cmd file is in Nagios nrpe format
 * 
 * @param line - \0 delimited line  "command[name]=/bin/program\0arg1\0arg2\0"
 */
static int
lxmenu_parser_argv(char **argv)
{
	char *start = argv[0];
	char *close_bracket;
	char *name;
	char *bin;

	
	if ( strncmp("command[", start, 8) == 0 ) {
		if ( lxmenu_size() == LXMENU_MAX_ITEMS ) {
			return LXMENU_FLUP;
		}
		
		name = start + 8;
		close_bracket  = strchr(name, ']');
		bin = strchr(name, '=');
		if (bin && bin + 1) bin++;
		else return LITESH_SYNTAX;
		
		if (close_bracket) *close_bracket = '\0';
		else return LITESH_SYNTAX;
		
		argv[0] = bin;
		
		lxmenu_add_command(name, argv, litesh_parse_argv_len(argv));
		
	}
	else {
		// skip
	}
	
	return LITESH_OK;
}

int
lxmenu_size()
{
	return commands.size;
}


static lxmenu_item_t*
lxmenu_find(char *line, int len)
{
	for ( int i = 0 ; i < commands.size ; i++ ) {
		if ( strncmp( commands.table[i]->name, line, len) == 0 ) {
			return commands.table[i];
		}
	}
	return NULL;
}

void
lxmenu_dump()
{
	for ( int i = 0 ; i < commands.size ; i++ ) {
		printf("rpc-name='%s' :: ", commands.table[i]->name);
		for ( int j = 0 ; j < LXMENU_MAX_ITEMS ;j ++ ) {
			if ( ! commands.table[i]->argv[j] ) break;
			printf("arg%i='%s' ", j, commands.table[i]->argv[j]);
		}
		puts("");
	}
}

/**
 * @param name \0 terminated menu_item name
 * @param argv \0 list of execv args
 * @param len length of all args 
 */
static int
lxmenu_add_command(char *name, char **argv, int len)
{
	lxmenu_item_t *menu_item = calloc(1, sizeof(lxmenu_item_t));
	if (menu_item == NULL) {
		return LITESH_ERROR;
	}
	
	menu_item->name = calloc(1, sizeof(char) * strlen(name) + 1);
	if (menu_item->name == NULL) {
		free(menu_item);
		return LITESH_ERROR;
	}
		
	menu_item->argv[0] = calloc(1, sizeof(char) * (len + 1));
	if (menu_item->argv[0] == NULL) {
		free(menu_item->name);
		free(menu_item);
		return LITESH_ERROR;
	}

	strcpy(menu_item->name, name);
	memcpy(menu_item->argv[0], argv[0], len);

	// copy offsets
	for ( int i = 1 ; i < LITESH_MAX_ARGS; i++ ) {
		if ( argv[i] ) {
			menu_item->argv[i] = menu_item->argv[0] + (argv[i] - argv[0]);
		}
	}
	
	commands.table[commands.size++] = menu_item;
	return LITESH_OK;
}

//////// rpc execution //////////////

/**
 * Read commands from stdin.
 */
int
lxmenu_read()
{
	lxmenu_welcome();
	
	while ( litesh_parse_fd(STDIN_FILENO, lxmenu_execute_argv, LITESH_EXEC_BLANKS | LITESH_FAIL_ON_ERROR ) ) ;
	
	puts("");
	return LITESH_OK;
}

/**
 * spawn/fork a process and don't wait for it to return
 */
static int
lxmenu_spawn(char **argv) {

	pid_t pid;
	
	if ( (pid = fork()) == 0 ) {
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		execvp(argv[0], argv);
	}

	if ( pid < 0 ) return LITESH_ERROR;
	return LITESH_OK;
}

static void
lxmenu_prompt()
{
	printf("\x1B[34mlxmenu\x1B[0m> ");
	fflush(stdout);
}

static void
lxmenu_started()
{
	printf("\x1B[32mstarted\x1B[0m\n");
	lxmenu_prompt();
}

static void
lxmenu_error()
{
	printf("\x1B[31merror\x1B[0m\n");
	lxmenu_prompt();
}

static void
lxmenu_command_not_found(char **argv)
{
	printf("\x1B[31mcommand not found\x1B[0m %s\n", argv[0]);
	lxmenu_prompt();
}


static void
lxmenu_welcome()
{
	printf("[\x1B[31mlxmenu:%i\x1B[0m]\n", getpid());
	lxmenu_prompt();
}

static int 
lxmenu_help()
{
	printf("commands: help exit wait echo");
	for ( int i = 0 ; i < commands.size ; i++ ) {
		printf(" %s", commands.table[i]->name);
	}
	puts("");

	lxmenu_prompt();
	return LITESH_OK;
}



/**
 * Command line handler.
 * 
 * @param line \0 terminated command line
 */
static int
lxmenu_execute_argv(char **argv)
{
	// printf("\x1B[31mexec=\x1B[0m%s\n", argv[0]);
	if ( argv[0] == NULL ) {
		lxmenu_prompt();
	}
	else if ( strcmp("help", argv[0]) == 0 ) {
		lxmenu_help();
	}
	else if ( strcmp("exit", argv[0]) == 0 ) {
		litesh_builtin_exit(argv);
		exit(0);
	}
	else if ( strcmp("wait", argv[0]) == 0 ) {
		litesh_builtin_wait(argv);
		lxmenu_prompt();
	}
	else if ( strcmp("echo", argv[0]) == 0 ) {
		litesh_builtin_echo(argv);
		lxmenu_prompt();
	}
	else {
		lxmenu_item_t *menu_item = lxmenu_find(argv[0], strlen(argv[0]));
		if ( menu_item ) {
			if ( argv[1] && strncmp("&", argv[1], 1) == 0 ) {
				lxmenu_spawn(menu_item->argv);
				lxmenu_started();
			}
			else {
				if ( litesh_builtin_fork(menu_item->argv) == LITESH_OK ) {
					lxmenu_prompt();
				}
				else {
					lxmenu_error();
				}
			}
		}
		else {
			lxmenu_command_not_found(argv);
			return LITESH_ERROR;
		}
	}
	
	return LITESH_OK;
}

