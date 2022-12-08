
#This is a hack to pass arguments to the run command and probably only 
#works with gnu make. 
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif


all: CSftp

#The following lines contain the generic build options
CC=gcc
CPPFLAGS=
CFLAGS=-g -Werror-implicit-function-declaration
CLIBS = -pthread

#List all the .o files here that need to be linked 
OBJS=CSftp.o usage.o dir.o command.o

usage.o: usage.c usage.h

dir.o: dir.c dir.h

command.o: command.c command.h

CSftp.o: CSftp.c dir.h usage.h command.h

CSftp: $(OBJS) 
	$(CC) -o CSftp $(OBJS) $(CLIBS) 

clean:
	rm -f *.o
	rm -f CSftp

.PHONY: run
run: CSftp  
	./CSftp $(RUN_ARGS)
