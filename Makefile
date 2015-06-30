SHELL=/bin/sh

PROGRAM = flwm
VERSION = 1.14

CXXFILES = main.C Frame.C Menu.C FrameWindow.C Desktop.C Hotkeys.C

LIBS = -lfltk

MANPAGE = 1

################################################################

OBJECTS = $(CXXFILES:.C=.o)

all:	makeinclude $(PROGRAM)

$(PROGRAM) : $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(PROGRAM) $(OBJECTS) $(LIBS) $(LDLIBS)

configure: configure.in
	autoconf

makeinclude: configure makeinclude.in
	./configure
include makeinclude

.SUFFIXES : .fl .do .C .c .H

.C.o :
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<
.C :
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<
.fl.C :
	-fluid -c $<
.fl.H :
	-fluid -c $<

clean :
	-@ rm -f *.o $(PROGRAM) $(CLEAN) core *~ makedepend
	@touch makedepend

depend:
	$(MAKEDEPEND) -I.. $(CXXFLAGS) $(CXXFILES) $(CFILES) > makedepend
makedepend:
	touch makedepend
include makedepend

install: $(PROGRAM)
	$(INSTALL) -s $(PROGRAM) $(bindir)/$(PROGRAM)
	$(INSTALL) $(PROGRAM).$(MANPAGE) $(mandir)/man$(MANPAGE)/$(PROGRAM).$(MANPAGE)

uninstall:
	-@ rm -f $(bindir)/$(PROGRAM)
	-@ rm -f $(mandir)/man$(MANPAGE)/$(PROGRAM).$(MANPAGE)

dist:
	cat /dev/null > makedepend
	-@mkdir $(PROGRAM)-$(VERSION)
	-@ln README Makefile configure install-sh makedepend *.C *.H *.h *.in *.fl $(PROGRAM).$(MANPAGE) flwm_wmconfig $(PROGRAM)-$(VERSION)
	tar -cvzf $(PROGRAM)-$(VERSION).tgz $(PROGRAM)-$(VERSION)
	-@rm -r $(PROGRAM)-$(VERSION)

exedist:
	-@mkdir $(PROGRAM)-$(VERSION)-x86
	-@ln README $(PROGRAM) $(PROGRAM).$(MANPAGE) flwm_wmconfig $(PROGRAM)-$(VERSION)-x86
	tar -cvzf $(PROGRAM)-$(VERSION)-x86.tgz $(PROGRAM)-$(VERSION)-x86
	-@rm -r $(PROGRAM)-$(VERSION)-x86

################################################################

PROGRAM_D = $(PROGRAM)_d

debug: $(PROGRAM_D)

OBJECTS_D = $(CXXFILES:.C=.do) $(CFILES:.c=.do)

.C.do :
	$(CXX) -I.. $(CXXFLAGS_D) -c -o $@ $<
.c.do :
	$(CC) -I.. $(CFLAGS_D) -c -o $@ $<

$(PROGRAM_D) : $(OBJECTS_D)
	$(CXX) $(LDFLAGS) -o $(PROGRAM_D) $(OBJECTS_D) $(LIBS) $(LDLIBS)

