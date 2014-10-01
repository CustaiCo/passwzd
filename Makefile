CXX ?= c++
LIBS = config.o
LDFLAGS = -Wl,-s -L. -L./libs/ -lcore -los -lcore -luuid -lkeyutils -lsystemd
CFLAGS =  -Wall -fstrict-enums -Os -Wextra -std=c++11 -march=native -flto -DUNICODE -DNDEBUG

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

clean:
	@echo cleaning
	@rm -f passwzd cachekey ${OBJ} *.rpo

.PHONY: all options clean
