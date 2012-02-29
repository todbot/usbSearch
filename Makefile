#OS ?= LINUX
OS ?= WINDOWS
#OS ?= MACOSX
#OS ?= BSD

#CC = i586-mingw32msvc-gcc
CC=gcc
CFLAGS ?= -O2 -Wall -std=c99 
LIBS = -lhid -lsetupapi -lole32 -loleaut32 -luuid
usbSearch.exe: usbSearch.c
	$(CC) $(CFLAGS) -s -DUSE_WIN32 -o usbSearch.exe usbSearch.c disphelper.c $(LIBS)



clean:
	rm -f usbSearch usbSearch.exe
