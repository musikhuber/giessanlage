VERSION = 0.1
CC      = /usr/bin/gcc
CFLAGS  = -Wall -g -D_REENTRANT -DVERSION=\"$(VERSION)\" -std=gnu99
LDFLAGS = -lm -lpthread -lwiringPi

OBJ = dojob.o functions.o 
dojob: $(OBJ)
	$(CC) $(CFLAGS) -o dojob  $(OBJ) $(LDFLAGS)
        
%.o: %.c
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm dojob *.o
	
install:
	cp dojob /usr/local/bin                	
	