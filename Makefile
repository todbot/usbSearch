#OS ?= LINUX
OS ?= WINDOWS
#OS ?= MACOSX
#OS ?= BSD

#CC = i586-mingw32msvc-gcc
CC=gcc
CFLAGS ?= -O2 -Wall -std=c99 
LIBS = -lhid -lsetupapi -lole32 -loleaut32 -luuid

all: usbSearch.exe listComPorts.exe

usbSearch.exe: usbSearch.c
	$(CC) $(CFLAGS) -s -DUSE_WIN32 -o usbSearch.exe usbSearch.c disphelper.c $(LIBS)

listComPorts.exe: listComPorts.c
	$(CC) $(CFLAGS) -s -DUSE_WIN32 -o listComPorts.exe listComPorts.c disphelper.c $(LIBS)

clean:
	rm -f usbSearch.exe
	rm -f listComPorts.exe

