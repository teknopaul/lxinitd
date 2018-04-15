
#include "litesh.h"
#include "lxmenu.h"

static const char *cmd_list = "/etc/lxmenu.conf";

int
main(int argc, const char* argv[])
{
	char script[256];
	
	if ( argc == 2 && strcmp("-v", argv[1]) == 0 ) PRINT_VERSION

	if (argc == 2) {
		
		strcpy(script, argv[1]);
		
	}
	else {
		
		const char* basename = bb_basename(argv[0]);
		
		if ( strncmp("lx", basename, 2) == 0 ) {
			
			sprintf(script, "/etc/%s.conf", basename);
			
		} else {
			
			strcpy(script, cmd_list);
			
		}
		
	}
	
	if ( lxmenu_init(script) == LITESH_OK ) {

		if ( ! lxmenu_size() ) {
			fprintf(stderr, "no commands\n");
			return 1;
		}

		/*
		 * main loop
		 */
		lxmenu_read();
	}

	return 2;
}
