# makefiletutorial.com
# software.codidact.com/posts/292774

CC = gcc
CFLAGS = -gdwarf
LINKS = -lm -lpanel -lmenu -lncurses

INCL_FILES = $(wildcard include/*.h)

DIR_SOURCE = src
DIR_OBJ = build

# create a list of all of the source files
# this is only to create the variable OBJECTS
SOURCES := $(wildcard $(DIR_SOURCE)/*.c)

# create a list of all of the object files from the source files
OBJECTS := $(patsubst $(DIR_SOURCE)/%.c,$(DIR_OBJ)/%.o,$(SOURCES))

%: %.c
	$(CC) $(CFLAGS) $@.c -o $@ $(LINKS)
	mv $@ bin

# all depends on all of the objects, so all objects have to be
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
	splint +charindex -incondefs +loopexec $(INCL_FILES) $(FILES)

ctags: $(FILES)
	ctags $(FILES)
