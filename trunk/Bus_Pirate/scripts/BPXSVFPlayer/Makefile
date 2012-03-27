EXE=BPXSVFplayer
CC = gcc
CFLAGS = -g -O0 -std=gnu99
LDFLAGS =

OBJS = buspirate.o serial.o main.o

all:  $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LFD_OBJS) $(LDFLAGS)

%.o:	%.c
	$(CC) $(CFLAGS) $(DEFS) -c $<

clean:
	rm -f $(EXE) *.o

