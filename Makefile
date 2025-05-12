CC = gcc
CFLAGS = -fPIC -Wall -O2
AR = ar
ARFLAGS = rcs

SRC = src/ant.c
SRC_TEST = src/test.c
HEADER = src/ant.h
OBJ = build/ant.o
OBJ_TEST = test/test.o
BIN_TEST = test/test
STATIC_LIB = build/ant/libant.a
SHARED_LIB = build/ant/libant.so
INCLUDE_DIR = build/ant/include
TEST_DIR = test

PREFIX ?= /usr

all: prepare $(STATIC_LIB) $(SHARED_LIB) copy_header

test: $(BIN_TEST) copy_so

copy_so:
	cp $(SHARED_LIB) $(TEST_DIR)/

$(BIN_TEST): $(OBJ_TEST)
	$(CC) -o test/test -Lbuild/ant -lant -Wl,-rpath=. $?

$(OBJ_TEST): $(SRC_TEST)
	$(CC) $(CFLAGS) -Ibuild/ant/include -c $? -o $@

install: all
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	install -m 644 $(STATIC_LIB) $(PREFIX)/lib/
	install -m 644 $(SHARED_LIB) $(PREFIX)/lib/
	install -m 644 $(HEADER) $(PREFIX)/include/

uninstall:
	rm -f $(LIBDIR)/libant.a
	rm -f $(LIBDIR)/libant.so
	rm -f $(INCLUDEDIR)/$(HEADER)

prepare:
	mkdir -p $(INCLUDE_DIR)

copy_header:
	cp $(HEADER) $(INCLUDE_DIR)/

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $? -o $@

$(STATIC_LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

$(SHARED_LIB): $(OBJ)
	$(CC) -shared -o $@ $?

clean:
	rm -f $(OBJ) $(OBJ_TEST) $(BIN_TEST) test/libant.so
	rm -rf build