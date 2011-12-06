# $Id: Makefile 30 2005-07-16 14:31:27Z tim $

################################################################################

FILES=libosinfo.so win_test

CFLAGS=-Wall -fPIC
INC=-I../reglookuplib/include -I../osinfo
LIB=-L../reglookuplib -lreglookup 
TEST_LIB=-L../osinfo -losinfo

all: $(FILES)

libosinfo.so: osinfo.o
	$(CC) $(CFLAGS) $(OPTS) $(LIB) -shared -o $@ osinfo.o $(OBJS)

osinfo.o: osinfo.c
	$(CC) $(CFLAGS) $(OPTS) $(INC) -c -o $@ osinfo.c

win_test:win_test.o
	$(CC) $(CFLAGS) $(OPTS) $(LIB) $(TEST_LIB) -o $@ win_test.o $(OBJS)

win_test.o: win_test.c
	$(CC) $(CFLAGS) $(OPTS) $(INC) -c -o $@ win_test.c

clean:
	rm -f $(FILES) *.o 
