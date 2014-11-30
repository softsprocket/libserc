
additional_flags = -std=c11 -I../include -fpic
LIBS = -lfl -lsscont

all: lib$(package).$(version).so

objects = serc.o spb.o serc_parse.o serc_scan.o

serc.o: serc.c
	 $(CC) -c serc.c $(CFLAGS) $(additional_flags) -o $@ 

spb.o: serc_scan.c serc_parse.c spb.c 
	 $(CC) -c spb.c $(CFLAGS) $(additional_flags) -o $@ 

serc_parse.o: serc_parse.c
	$(CC) -c serc_parse.c $(CFLAGS) $(additional_flags) -o $@

serc_scan.o: serc_scan.c
	$(CC) -c serc_scan.c $(CFLAGS) -Wno-unused-function $(additional_flags) -o $@

serc_scan.c: serc_scan.l
	$(LEX) serc_scan.l 

serc_parse.c: serc_scan.c serc_parse.y
	$(YACC) serc_parse.y

lib$(package).$(version).so: $(objects)
	-mkdir -p ../lib
	$(CC) $(CFLAGS) $(additional_flags) -shared -o ../lib/$@ $(objects) $(LIBS)

clean:
	-rm *.o
	-rm serc_scan.c serc_parse.c ../include/serc_parse.h ../include/serc_lexer.h
	-rm ../lib/lib$(package).$(version).so

install:
	install -d  $(DESTDIR)$(prefix)/lib/softsprocket
	install -d  $(DESTDIR)$(prefix)/include/softsprocket
	install -m 0644 ../lib/lib$(package).$(version).so  $(DESTDIR)$(prefix)/lib/softsprocket
	install -m 0644 ../include/serc.h $(DESTDIR)$(prefix)/include/softsprocket
	-ln -s $(DESTDIR)$(prefix)/lib/softsprocket/lib$(package).$(version).so $(DESTDIR)$(prefix)/lib$(package).so
	
uninstall:
	-rm  $(DESTDIR)$(prefix)/lib/softsprocket/lib$(package).$(version).so
	-rm -d  $(DESTDIR)$(prefix)/lib/softsprocket
	-rm $(DESTDIR)$(prefix)/include/softsprocket/serc.h
	-rm -d $(DESTDIR)$(prefix)/include/softsprocket
	-rm $(DESTDIR)$(prefix)/lib$(package).so
	

.PHONY: all clean install uninstall


