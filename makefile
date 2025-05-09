# libtrackmem/
# ├── include/
# │   └── trackmem.h       # Public API
# ├── src/
# │   └── trackmem.c       # Implementation files
# ├── tests/
# │   └── test_trackmem.c  # Unit tests
# ├── Makefile
# ├── README.md
# └── LICENSE

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/trackmem.c
OBJ = $(SRC:.c=.o)
TARGET = libtrackmem.a
TEST_SRC = tests/test_trackmem.c
TEST_EXEC = test_trackmem

PREFIX ?= /usr/local
INCLUDEDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

install:
	mkdir -p $(INCLUDEDIR)
	cp include/trackmem.h $(INCLUDEDIR)
	mkdir -p $(LIBDIR)
	cp libtrackmem.a $(LIBDIR)

uninstall:
	rm -f $(INCLUDEDIR)/trackmem.h


all: $(TARGET) $(TEST_EXEC)

$(TARGET): $(OBJ)
	ar rcs $@ $^

$(TEST_EXEC): $(TEST_SRC) $(TARGET)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_EXEC)
