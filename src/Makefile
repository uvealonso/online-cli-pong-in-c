#
# 'make' build executable file 'pong'
# 'make clean' removes all .o and executable files
#

# Define the C compiler to use
CC = gcc

# Define any compile-time flags
# Compiler flags:
# -std=gnu99
# -lncurses
# -Wall turns on most, but not all, compiler warnings
# -pedantic
CFLAGS = -std=gnu99 -lncurses -Wall -pedantic

# Define any directories for global prerequisites.
DEPENDENCYDIR = ./include
# Define any directories containing header files other than /usr/include
INCLUDES = -I ./include
# Define source directory
SOURCEDIR = ./src
# Define output directory to store object files.
OUTPUTDIR = ../bin

#C FILES
SRC = pong_check.c pong_print.c pong_do.c pong.c

# Define Objects files.
OBJS = $(patsubst %.c, %.o, $(SRC))
# Define Header files.
DEPS = $(patsubst %.c, $(DEPENDENCYDIR)/%.h, $(SRC))

# Define the executable file.
EXE_NAME = pong

TARGET = $(OUTPUTDIR)/$(EXE_NAME)

.PHONY: directories clean all

all: directories $(TARGET)
#	@echo Compiled pong

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDES)

%.o: $(SOURCEDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES)

directories: $(OUTPUTDIR)

$(OUTPUTDIR):
	mkdir -p $(OUTPUTDIR)

clean:
	-rm -f $(OBJS)
	-rm -f $(TARGET)
