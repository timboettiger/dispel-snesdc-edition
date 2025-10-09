CC=gcc
CFLAGS=-Wall -O2 -Wno-strict-prototypes -g
LDFLAGS=
LDLIBS=
SOURCES=main.c 65816.c config.c describe.c helper.c cmdlinetools.c romtools.c patchtools.c disassembler.c
OBJECTS=$(SOURCES:.c=.o)
CAN_INSTALL = no
INSTALL_PATH = /usr/bin

ifeq ($(OS),Windows_NT)
	EXECUTABLE = dispel.exe
else
	EXECUTABLE = dispel
	UNAME_S = $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CAN_INSTALL = yes
	endif
	ifeq ($(UNAME_S),Darwin)
		CAN_INSTALL = yes
		INSTALL_PATH = /usr/local/bin
	endif
endif

# Detecting MinGW-/MSYS2 for regex.h -> libgnurx
ifneq (,$(findstring mingw,$(shell $(CC) -dumpmachine 2>/dev/null)))
	LDLIBS += -lgnurx
endif

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	-rm *.o ${EXECUTABLE}

ifeq ($(CAN_INSTALL),yes)
install:
	cp -v $(EXECUTABLE) $(INSTALL_PATH)
uninstall:
	rm -rf $(INSTALL_PATH)/$(EXECUTABLE)
else
install:
	@echo "Install not supported on this platform."
uninstall:
	@echo "Uninstall not supported on this platform."
endif

.PHONY: all clean install uninstall
