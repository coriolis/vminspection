# $Id: Makefile 30 2005-07-16 14:31:27Z tim $

################################################################################

FILES=libosinfo.so 

CFLAGS=-Wall -fPIC -ggdb -DREGLOOKUP 
#INC=-I../../hivex/lib/ -I../osinfo
INC=-I../reglookuplib/include -I../osinfo
#LIB=-L../../hivex/lib/.libs -lhivex  
LIB=../reglookuplib/libreglookup.so

TEST_LIB=-L. -L../osinfo -losinfo

all: $(FILES)

libosinfo.so: osinfo.o
	$(CC) $(CFLAGS) $(OPTS) $(LIB) -shared -o $@ osinfo.o $(OBJS)
	bash make_osinfo_js.sh
	cp libosinfo.bc ../../sleuthkit-1/tools/fstools/libosinfo.bc	

osinfo.o: osinfo.c
	$(CC) $(CFLAGS) $(OPTS) $(INC) -c -o $@ osinfo.c

win_test:win_test.o
	$(CC) $(CFLAGS) $(OPTS) -o $@ win_test.o $(OBJS) $(TEST_LIB) $(LIB)

win_test.o: win_test.c
	$(CC) $(CFLAGS) $(OPTS) $(INC) -c -o $@ win_test.c 

clean:
	rm -f $(FILES) *.o 
