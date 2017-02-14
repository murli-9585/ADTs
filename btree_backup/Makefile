# This make file will only work in cases where all the files are in one
# directory.
# Ref GNU MAKE & www.cs.colby.edu/maxwell/courses/tutorials/maketutor ex:4.

CC=gcc
CFLAGS=-c -Wall -g
# lib include (m=mathlib)
LDFLAGS= -lm
DEPENDENCIES=btree.h btree_priv.h
SOURCES=btree_test.c btree.c
# Create objects for every source; C files.
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=btree_test

# First rule.

all: $(SOURCES) $(EXECUTABLE)

# Format
# Action : prerequsite
# 	recipe
# Other things. $@ is primary file name. so output here would be btree_test

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# This is implied rule that all .c files have .o files.
# $< is first prerequisite. btree.h
.c.o:
	$(CC) $(CFLAGS) $< -o $@

# PHONY target is used in cases where there is a file named clean. 
.PHONY: clean

clean:
	rm -f *.o
	@echo Done!
