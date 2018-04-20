PREFIX ?= /bin

ROSH_SOURCES = src/rosh.c src/litesh_builtins.c src/rosh_aliases.c src/litesh_parser.c
LXINITD_SOURCES = src/lxinitd.c src/litesh_builtins.c src/lxinitd_service.c src/lxinitd_service_log.c src/litesh_parser.c
LXMENU_SOURCES = src/lxmenu.c src/lxmenu_main.c src/litesh_builtins.c src/litesh_parser.c
NOOPSH_SOURCES = src/noopsh.c src/litesh_builtins.c src/litesh_parser.c
$(shell mkdir -p target/)
#
# Small binaries
#
CC = musl-gcc
CFLAGS += -Os
SUPER_STRIP = strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag

all: target target/rosh target/noopsh target/lxmenu target/lxinitd target/lxpasswd

src/litesh.h: version
	deploy/update-version.sh
	

target/rosh: $(ROSH_SOURCES) src/litesh.h
	${CC} ${CFLAGS} -Wall -Werror $(ROSH_SOURCES) -static -o $@ ${LDFLAGS}
	${SUPER_STRIP} $@

target/noopsh: $(NOOPSH_SOURCES)
	${CC} ${CFLAGS} -Wall -Werror $(NOOPSH_SOURCES) -static -o $@ ${LDFLAGS}
	${SUPER_STRIP} $@

target/lxinitd: $(LXINITD_SOURCES) src/lxinitd_service.h src/lxinitd_service_log.h
	${CC} ${CFLAGS} -Wall -Werror $(LXINITD_SOURCES) -static -o $@ ${LDFLAGS}
	${SUPER_STRIP} $@

target/lxmenu: $(LXMENU_SOURCES) src/lxmenu.h
	${CC} ${CFLAGS} -Wall -Werror $(LXMENU_SOURCES) -static -o $@ ${LDFLAGS}
	${SUPER_STRIP} $@

	target/lxpasswd: src/lxpasswd.c
	${CC} -Wall -Werror src/lxpasswd.c -lcrypt -static -o $@ ${LDFLAGS}
	${SUPER_STRIP} $@


test:
	${CC} ${CFLAGS} -g -Wall -Werror src/test/service_test.c src/lxinitd_service.c src/lxinitd_service_log.c src/litesh_builtins.c src/litesh_parser.c -o target/service_test ${LDFLAGS}
	target/service_test
	${CC} ${CFLAGS} -g -Wall -Werror src/test/lxmenu_test.c src/lxmenu.c src/litesh_builtins.c src/litesh_parser.c -o target/lxmenu_test ${LDFLAGS}
	target/lxmenu_test
	${CC} ${CFLAGS} -g -Wall -Werror src/test/parser_test.c src/litesh_parser.c -o target/parser_test ${LDFLAGS}
	target/parser_test
	bash -n src/sh/lxdeluser
	bash -n lxc/lxc-*.sh

doc:
	rm -rf target/site
	mkdir target/site
	markbook doc target/site

clean:
	rm -rf target/

distclean: clean
	rm -rf target/

install:
	@mkdir -p ${DESTDIR}${PREFIX} ${DESTDIR}/usr/sbin
	install target/lxinitd ${DESTDIR}${PREFIX}
	install target/rosh ${DESTDIR}${PREFIX}
	install target/noopsh ${DESTDIR}${PREFIX}
	install target/lxmenu ${DESTDIR}${PREFIX}
	install target/lxpasswd ${DESTDIR}${PREFIX}
	install src/sh/lxdeluser ${DESTDIR}${PREFIX}
	mkdir -p ${DESTDIR}/usr/share/lxc/templates
	install lxc/lxc-lxinitd ${DESTDIR}/usr/share/lxc/templates
	install lxc/lxc-*.sh ${DESTDIR}/usr/sbin

uninstall:
	rm ${DESTDIR}${PREFIX}/lxinitd
	rm ${DESTDIR}${PREFIX}/rosh
	rm ${DESTDIR}${PREFIX}/noopsh
	rm ${DESTDIR}${PREFIX}/lxmenu
	rm ${DESTDIR}${PREFIX}/lxpasswd
	rm ${DESTDIR}${PREFIX}/lxdeluser


.PHONY: all clean distclean install uninstall test doc
