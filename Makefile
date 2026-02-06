# makefiletutorial.com
# software.codidact.com/posts/292774

# note: all paths are grounded at the position of the Makefile

CC = gcc
CFLAGS = -gdwarf
LINKS = -lm -lpanel -lmenu -lncurses

INCL_FILES = $(wildcard include/*.h)

DIR_SOURCE = src
DIR_OBJ = build

# create a list of all of the source files
# this is used to create the variable OBJECTS
# this is used instead of wildcarding $(DIR_OBJ)/*.o
# to ensure that new files create an object file
# automatically
SOURCES := $(wildcard $(DIR_SOURCE)/*.c)

# create a list of all of the object files from the source files
OBJECTS := $(patsubst $(DIR_SOURCE)/%.c,$(DIR_OBJ)/%.o,$(SOURCES))

%: %.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LINKS)
	mv $@ bin

# all depends on all of the objects, so all .c files have to be
# turned into objects
all: ./bin/main $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o bin/main $(LINKS)

# create an individual rule for every object that depends on
# its respective source file
$(DIR_OBJ)/%.o: $(DIR_SOURCE)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LINKS)
# should expand to
# $(CC) $(CFLAGS) -c $(DIR_SOURCE)/%.c -o $(DIR_OBJ)/%.o $(LINKS)

splint:
	splint +charindex -incondefs +loopexec $(INCL_FILES) $(SOURCES)

ctags: $(FILES)
	ctags $(SOURCES) $(INCL_FILES)
vim:
	vim $(SOURCES) $(INCL_FILES)
