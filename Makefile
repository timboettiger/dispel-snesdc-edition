CC=gcc
CFLAGS=-Wall -O2 -Wno-strict-prototypes
LDFLAGS=
SOURCES=main.c 65816.c describe.c helper.c cmdlinetools.c romtools.c patchtools.c disassembler.c
OBJECTS=$(SOURCES:.c=.o)
CAN_INSTALL = no
ifeq ($(OS),Windows_NT)
	EXECUTABLE = dispel.exe
else
	EXECUTABLE = dispel
	UNAME_S = $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CAN_INSTALL = yes
	endif
endif
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
install:
	cp -v $(EXECUTABLE) /usr/bin
	#FIXME It ALWAYS copies dispel to /usr/bin, regardless of OS
uninstall:
	rm -rf /usr/bin/$(EXECUTABLE)
clean:
	rm *.o ${EXECUTABLE}
