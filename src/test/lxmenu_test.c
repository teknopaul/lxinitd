

#include "litesh_test.h"
#include "../lxmenu.h"

/**
 * test hook
 */
int
main(int argc, const char* argv[])
{
	
	equals("init", LITESH_OK, lxmenu_init("./test/lxmenu.conf"));
	equals("init lxmenu size", 1, lxmenu_size());
	//litesh_rpc_dump();
	
	equals("flup", LITESH_OK, lxmenu_init("test/lxmenu-min.conf"));
	equals("init min size", 1, lxmenu_size());
	//litesh_rpc_dump();
	
 	equals("flup", LXMENU_FLUP, lxmenu_init("test/lxmenu-limits.conf"));
	
	return errors ? 1 : 0;
}
