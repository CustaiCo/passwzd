CXX ?= c++
LIBS = config.o
LDFLAGS = -L. -L./libs/ -lcore -los -lcore -luuid -lkeyutils -lsystemd
CFLAGS =  -Wall -fstrict-enums -Os -Wextra -std=c++11 -march=native -flto -DUNICODE -DNDEBUG
PREFIX = /usr/local

OBJ = config.o cachekey.o main.o

all: options passwzd cachekey

options:
	@echo build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CXX      = ${CXX}"



%.o : %.cpp
	@echo CXX $<
	@${CXX} -c ${CFLAGS} $<

passwzd : ${OBJ}
	@echo CXX -o $@
	@${CXX} -o $@ main.o ${LIBS} ${CFLAGS} ${LDFLAGS}

cachekey : ${OBJ} 
	@echo CXX -o $@
	@${CXX} -o $@ $@.o ${LIBS} ${CFLAGS} ${LDFLAGS}

install: all
	@echo installing under ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f passwzd ${DESTDIR}${PREFIX}/bin/
	@cp -f cachekey ${DESTDIR}${PREFIX}/bin/
	@chmod 755 ${DESTDIR}${PREFIX}/bin/passwzd
	@chmod 755 ${DESTDIR}${PREFIX}/bin/cachekey
	@echo Installing manual pages to ${DESTDIR}${PREFIX}/share/man/
	@mkdir -p ${DESTDIR}${PREFIX}/share/man/man1
	@cp -f passwzd.1 ${DESTDIR}${PREFIX}/share/man/man1/
	@cp -f cachekey.1 ${DESTDIR}${PREFIX}/share/man/man1/
	@chmod 644 ${DESTDIR}${PREFIX}/share/man/man1/passwzd.1
	@chmod 644 ${DESTDIR}${PREFIX}/share/man/man1/cachekey.1

clean:
	@echo cleaning
	@rm -f passwzd cachekey ${OBJ} *.rpo

.PHONY: all options clean
