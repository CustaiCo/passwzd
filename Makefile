CC = c++
SRC = config.cpp main.cpp
LDFLAGS = -L. -L./libs/ -los -lcore -los -luuid -lxerces-c -lkeyutils -lsystemd
CFLAGS = -std=c++11 -DUNICODE

OBJ = ${SRC:.cpp=.o}

all: options passwzd

options:
	@echo build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.cpp.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

passwzd: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f passwzd ${OBJ}

.PHONY: all options clean
