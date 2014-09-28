CXX ?= c++
LIBS = config.o
LDFLAGS = -L. -L./libs/ -los -lcore -los -luuid -lxerces-c -lkeyutils -lsystemd
CFLAGS = -std=c++11 -DUNICODE -O2

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
	@${CXX} -o $@ main.o ${LIBS} ${LDFLAGS}

cachekey : ${OBJ} 
	@echo CXX -o $@
	@${CXX} -o $@ $@.o ${LIBS} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f passwzd cachekey ${OBJ}

.PHONY: all options clean
