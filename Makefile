-include config.mk

INPUT       = infantry.dat
OUTPUT      = infantry.exe
LDS         = infantry.lds

IMPORTS     = 0x179000 260
LOADCONFIG  = 0x12A538 64
TLS         = 0x0 0
IAT         = 0x179790 1676

LDFLAGS     = --subsystem=windows --disable-nxcompat --enable-stdcall-fixup --disable-dynamicbase --disable-reloc-section
ASFLAGS     = -Iinc/
NFLAGS      = -f elf -Iinc/
CFLAGS      = -Iinc/ -O2 -march=pentium4 -Wall
CXXFLAGS    = -Iinc/ -O2 -march=pentium4 -Wall

LIBS        = -luser32 -ladvapi32 -lshell32 -lmsvcrt -lkernel32 -lgdi32
CXXLIBS     = =./lib/crt2.o -lstdc++ -lgcc -lpthread -lmingw32 -lmoldname -lmingwex -lgcc

GCCVERSION  = $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
SEARCHDIRS  = -L=./../lib/gcc/i686-w64-mingw32/$(GCCVERSION) -L=./lib/gcc/i686-w64-mingw32/$(GCCVERSION)

OBJS        = \
				sym.o \
				rsrc.o \
				src/imports.o \
				src/processes-fix.o \
				src/start.o

PETOOL     ?= petool
STRIP      ?= strip
NASM       ?= nasm
WINDRES    ?= windres

all: $(OUTPUT)

%.o: %.asm
	$(NASM) $(NFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

%.o: %.rc
	$(WINDRES) $(WINDRES_FLAGS) $< $@

rsrc.o: $(INPUT)
	$(PETOOL) re2obj $(INPUT) $@

$(OUTPUT): $(LDS) $(INPUT) $(OBJS)
	$(LD) $(LDFLAGS) -T $(LDS) -o "$@" $(OBJS) $(CXXLIBS) $(LIBS) $(SEARCHDIRS)
ifneq (,$(IMPORTS))
	$(PETOOL) setdd "$@" 1 $(IMPORTS) || ($(RM) "$@" && exit 1)
endif
ifneq (,$(LOADCONFIG))
	$(PETOOL) setdd "$@" 10 $(LOADCONFIG) || ($(RM) "$@" && exit 1)
endif
	$(PETOOL) setdd "$@" 9 $(TLS) || ($(RM) "$@" && exit 1)
	$(PETOOL) setdd "$@" 12 $(IAT) || ($(RM) "$@" && exit 1)
	$(PETOOL) patch "$@" || ($(RM) "$@" && exit 1)
	$(STRIP) -R .patch "$@" || ($(RM) "$@" && exit 1)
	$(PETOOL) dump "$@"

clean:
	$(RM) $(OUTPUT) $(OBJS)
