# Switched the Makefile to work with windows as a .dll with MinGW64

CC      = gcc
CFLAGS  = -Wall -m64
DLLNAME = rijndael.dll

# Source files for the AES library
SRCS    = rijndael.c
OBJS    = $(SRCS:.c=.o)

all: $(DLLNAME)

# Link the object files into a shared library (DLL)
$(DLLNAME): $(OBJS)
	$(CC) -shared -m64 -o $(DLLNAME) $(OBJS)

# Compile .c files into .o files
%.o: %.c rijndael.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(DLLNAME) $(OBJS)
