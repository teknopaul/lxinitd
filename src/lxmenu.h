

#ifndef LXMENU_H
#define LXMENU_H


// errors
#define LXMENU_FLUP         -3     // max commands reached

// constants
#define LXMENU_MAX_ITEMS      20    // max lxmenu commands

// SNIP_lxmenu.c

typedef struct lxmenu_item_s {
	char   *name;                  // command name
	char   *argv[LITESH_MAX_ARGS]; // arrary of char pointers, all pointers are in the same malloced space.
} lxmenu_item_t;

typedef struct lxmenu_table_s {
	int             size;
	lxmenu_item_t  *table[LXMENU_MAX_ITEMS];
} lxmenu_table_t;

int lxmenu_init();
int lxmenu_size();
int lxmenu_read();
int lxmenu_print_table();
void lxmenu_dump();
// SNIP_lxmenu.c


#endif // LXMENU_H