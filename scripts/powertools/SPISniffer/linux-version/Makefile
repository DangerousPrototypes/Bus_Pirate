#CC	=	gcc
CFLAGS	=	-Wall -Os -DTRUE=1 -DFALSE=0

VERSION	=	\"V0.10\"
CFLAGS	+=	-DVERSION=$(VERSION)
#LDFLAGS += 	-lcurses

#######################################################################

SRC	=	serial.c buspirate.c main.c
OBJ	=	serial.o buspirate.o main.o

all:	spisniffer

spisniffer:	$(OBJ)
	$(CC) -s -o spisniffer $(OBJ) $(LDFLAGS)

serial.o: serial.c serial.h
buspirate.o: buspirate.c buspirate.h
main.o: main.c

clean:
	rm -f $(OBJ) spisniffer
