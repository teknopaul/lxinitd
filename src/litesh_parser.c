/**
 * litesh script parser.
 *
 * @author teknopaul
 */

#include "litesh.h"

static int  litesh_split_line(char **argv, char *line, int len);
static void litesh_zero_buffers(char *current_line, char **argv);

static char*
ltrim(char *line) {
	while (*line == ' ') line++;
	return line;
}

int
litesh_parse(const char* script, litesh_execute_argv_pt litesh_execute_argv)
{

	int in = open(script, O_RDONLY);
	if ( in == -1 ) {
		fprintf(stderr, "cannot open script %s\n", script);
		return 3;
	}
	
	return litesh_parse_fd(in, litesh_execute_argv, 1);
}

/**
 * @param in - file descriptor of commands stream, (script file or stdin)
 * @param litesh_execute_argv - pluggable cmd handlers
 * @param expect_shebang -  expect #!/bin/rosh prefix
 */
int
litesh_parse_fd(int in, litesh_execute_argv_pt execute_argv, int flags)
{

	// read the script line by line
	int   rc;
	int   first_char = 1;
	int   first_line = 1;
	int   line_num = 0;
	int   line_pos = 0;
	char *argv[LITESH_MAX_ARGS + 1];
	char  current_line[LITESH_MAX_LINE_LENGTH + 1];
	char  c;
	
	litesh_zero_buffers(current_line, argv);

	while ( 1 ) {

		rc = read(in, &c, 1);

		if ( rc == 0 ) {
			// EoF
			return LITESH_OK;
		}
		
		if ( rc < 0 ) {
			fprintf(stderr, "read error\n");
			return LITESH_ERROR;
		}

		if ( flags & LITESH_EXPECT_SHEBANG && first_char ) {
			if ( c != '#' ) {
				fprintf(stderr, "missing shebang\n");
				return LITESH_SYNTAX;
			}
			first_char = 0;
			continue;
		}

		if ( flags & LITESH_EXPECT_SHEBANG && first_line ) {
			if ( c == '\n' ) {
				first_line = 0;
				line_num++;
			}
			continue;
		}

		// Reached the end of a line

		if ( c == '\n' ) {
			line_num++;
			
			// empty lines
			if ( line_pos == 0 ) {
				if ( flags & LITESH_EXEC_BLANKS ) execute_argv(argv);
				continue;
			}
			// comments
			if ( *current_line == '#' ) {
				line_pos = 0;
				litesh_zero_buffers(current_line, argv);
				if ( flags & LITESH_EXEC_BLANKS ) execute_argv(argv);
				continue;
			}
			//printf("line_num: %i: %i\n", line_num, line_pos);
	
			current_line[line_pos] = '\0';
			if ( litesh_split_line(argv, ltrim(current_line), line_pos) ) return LITESH_SYNTAX;

			rc = execute_argv(argv);
			if ( rc && flags & LITESH_FAIL_ON_ERROR) {
				return rc;
			}
			
			// reset
			line_pos = 0;
			litesh_zero_buffers(current_line, argv);
			continue;
		}

		if ( c > '~' || c < ' ' ) {
			fprintf(stderr, "non-ascii characters\n");
			return LITESH_SYNTAX;
		}
		
		if ( line_pos >= LITESH_MAX_LINE_LENGTH ) {
			fprintf(stderr, "line too long\n");
			return LITESH_SYNTAX;
		}

		current_line[line_pos++] = c;

	}

	// unreachable
	return LITESH_OK;

}


/**
 * @param argv array must be continuous, i.e. the array genreated by the parser.
 * 
 * @return length of the argv array (less than LITESH_MAX_LINE_LENGTH, may be greater than the sum of strlen())
 */
int
litesh_parse_argv_len(char **argv) {
	char *start = argv[0];
	char *last = start;
	for ( int i = 0 ; i < LITESH_MAX_ARGS ; i++ ) {
		if ( ! argv[i] ) {
			last = argv[i - 1];
			break;
		}
	}
	return (last - start) + strlen(last) + 1;
}
int
litesh_parse_argv_count(char **argv) {
	for ( int i = 0 ; i < LITESH_MAX_ARGS ; i++ ) {
		if ( ! argv[i] ) {
			return  i;
		}
	}
	return LITESH_MAX_ARGS;
}

static void
litesh_zero_buffers(char *current_line, char **argv) {
	memset(current_line, 0, LITESH_MAX_LINE_LENGTH + 1);
	memset(argv, 0 , (LITESH_MAX_ARGS + 1) * sizeof(char*) );
}

/**
 * Split line in to tokens. Strings with spaces can be quoted "like \"this\" \\ ".
 * 
 * @param line \0 terminated command line
 * @param len length of the line
 */
static int
litesh_split_line(char **argv, char *line, int len)
{
	int in_string = 0;
	
	if ( line[0] == '\"' ) {
		fprintf(stderr, "leading string error\n");
		return LITESH_SYNTAX;
	}
	
	argv[0] = line;

	for ( int i = 0, a = 1 ; i < len; i++ ) {
		// " must terminate an argument we dont support """"
		if (in_string && line[i] == '\"' ) {
			if (i < len - 1 && line[i + 1] != ' ') {
				fprintf(stderr, "string syntax error\n");
				return LITESH_SYNTAX;
			}
			line[i] = '\0';
			in_string = 0;
			continue;
		}
		
		// escaping \\  and \"
		if (in_string && line[i] == '\\' ) {
			if (i < len - 1 && (line[i + 1] == '\"' || line[i + 1] == '\\' ) ) {
				//shift
				memcpy(line + i, line + i + 1, len - i);
				len--;
			}
			continue;
		}
		
		if (!in_string && line[i] == '\"' ) {
			in_string = 1;
			continue;
		}
		if ( in_string && line[i] == ' ' ) {
			continue;
		}
		
		if ( line[i] == ' ') {
			line[i] = '\0';
			if ( line[i + 1] == '\"') {
				argv[a++] = line + i + 2;
			} else {
				argv[a++] = line + i + 1;
			}
			if ( a > LITESH_MAX_ARGS ) {
				fprintf(stderr, "too many arguments\n");
				return LITESH_SYNTAX;
			}
		}
	}
	if (in_string) {
		fprintf(stderr, "unclosed string\n");
		return LITESH_SYNTAX;
	}

	return LITESH_OK;
}
