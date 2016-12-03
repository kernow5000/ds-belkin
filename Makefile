# rambo daemon framework project makefile
# kernow september 2005

CC = gcc
CFLAGS = -Wall -I./include

PING_BIN = ds-belkin
PING_SRC = ds-belkin.c

ARCHIVE = ds-belkin
BACKUPPOINT = ~/dev/

all: clean ping

ping:	   
	   $(CC) $(CFLAGS) $(PING_SRC) -o $(PING_BIN)
    
clean:	      
	   rm -Rf *.o *~ include/*~ $(PING_BIN)
	      
backup:		
	   tar cf $(ARCHIVE).tar *[^*.tar]
	   gzip $(ARCHIVE).tar
	   mv $(ARCHIVE).tar.gz $(BACKUPPOINT)
	      
	      
	     
      	       
