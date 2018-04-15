

#include "litesh_test.h"

static int
litesh_print_args(char **argv)
{
	for (int i = 0 ; i < LITESH_MAX_ARGS ; i++ ) {
		if (argv[i]) {
			printf("arg%i='%s'\n", i, argv[i]);
		}
	}
	return LITESH_OK;
}

/**
 * @param line \0 terminated command line
 */
static int
litesh_execute_argv(char **argv)
{
	//puts("litesh_execute_argv");
	for (int i = 0 ; i < LITESH_MAX_ARGS ; i++ ) {
		if (argv[i]) {
			assert("arglen", strlen(argv[i]));
		}
	}
	return LITESH_OK;
}

static int
litesh_assert_ten_args(char **argv)
{
	for( int i = 0 ; i < 10 ; i++ ) assert("ten args", argv[i] != NULL);
	return LITESH_OK;
}

static int
litesh_assert_one_arg(char **argv)
{
	//puts("litesh_assert_one_arg");
	//puts(argv[2]);
	if (argv[2]) assert("strings not joined", 0);
	//puts(argv[1]);
	assert("strings not joined", strcmp("arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9", argv[1]) == 0);
	return LITESH_OK;
}

static int
litesh_assert_string_arg(char **argv)
{
	if ( strcmp("line1", argv[0]) == 0 ) {
		assert("hello world", strcmp("hello world", argv[1]) == 0);
	}
	if ( strcmp("line2", argv[0]) == 0 ) {
		assert("hello", strcmp("hello", argv[1]) == 0);
		assert("world", strcmp("world", argv[2]) == 0);
	}
	if ( strcmp("line3", argv[0]) == 0 ) {
		assert("hello", strcmp("hello", argv[1]) == 0);
		assert("world", strcmp("world", argv[2]) == 0);
	}
	if ( strcmp("line4", argv[0]) == 0 ) {
		assert("hello", strcmp("hello", argv[1]) == 0);
		assert("world", strcmp("world", argv[2]) == 0);
		assert("empty string", strcmp("",      argv[3]) == 0);
	}
	if ( strcmp("line5", argv[0]) == 0 ) {
		assert("empty string", strcmp("",      argv[1]) == 0);
		assert("empty string", strcmp("",      argv[2]) == 0);
		assert("empty string", strcmp("",      argv[3]) == 0);
	}
	if ( strcmp("line6", argv[0]) == 0 ) {
		assert("empty string", strcmp("",      argv[1]) == 0);
	}
	if ( strcmp("line7", argv[0]) == 0 ) {
		assert("you can \"quote\" me on that", strcmp("you can \"quote\" me on that", argv[1]) == 0);
	}
	if ( strcmp("line8", argv[0]) == 0 ) {
		assert("foo\\baa", strcmp("foo\\baa", argv[1]) == 0);
	}

	//litesh_print_args(argv);
	return LITESH_OK;
}


/**
 * test hook
 */
int
main(int argc, const char* argv[])
{

	//equals("empty",      LITESH_SYNTAX, litesh_parse("./test/parse_test1.rosh", litesh_execute_argv));
	equals("line flup",  LITESH_SYNTAX, litesh_parse("./test/parse_test2.rosh", litesh_execute_argv));
	equals("no shebang", LITESH_SYNTAX, litesh_parse("./test/parse_test3.rosh", litesh_execute_argv));
	equals("non ascii",  LITESH_SYNTAX, litesh_parse("./test/parse_test4.rosh", litesh_execute_argv));
	equals("args flup",  LITESH_SYNTAX, litesh_parse("./test/parse_test5.rosh", litesh_execute_argv));
	equals("OK",         LITESH_OK,     litesh_parse("./test/parse_test6.rosh", litesh_execute_argv));
	equals("10 args",    LITESH_OK,     litesh_parse("./test/parse_test7.rosh", litesh_assert_ten_args));
	equals("strings",    LITESH_OK,     litesh_parse("./test/parse_test8.rosh", litesh_assert_one_arg));
	equals("strings",    LITESH_OK,     litesh_parse("./test/parse_test9.rosh", litesh_assert_string_arg));
	
	return errors ? 1 : 0;
}
