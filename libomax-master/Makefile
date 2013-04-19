OMAX_BASENAMES = omax_util omax_class omax_doc
OMAX_OBJECTS = $(foreach OBJ, $(OMAX_BASENAMES), $(OBJ).o) 
OMAX_CFILES = $(foreach F, $(OMAX_BASENAMES), $(F).c)
OMAX_HFILES = $(foreach F, $(OMAX_BASENAMES), $(F).h)

C74SUPPORT = ../max6-sdk/c74support
MAX_INCLUDES = $(C74SUPPORT)/max-includes

MAC_SYSROOT = MacOSX10.8.sdk 
MAC-CFLAGS = -arch i386 -O3 -funroll-loops -isysroot /Developer/SDKs/$(MAC_SYSROOT) -mmacosx-version-min=10.5
WIN-CFLAGS = -O3 -funroll-loops -DWIN_VERSION -DWIN_EXT_VERSION -U__STRICT_ANSI__ -U__ANSI_SOURCE -std=c99

MAC-INCLUDES = -I$(MAX_INCLUDES) -I../libo -F/System/Library/Frameworks -I/usr/include
WIN-INCLUDES = -I../../../../c74support/max-includes -I../libo

all: CFLAGS += $(MAC-CFLAGS)
all: CC = clang
all: I = $(MAC-INCLUDES)
all: $(LIBO_CFILES) $(LIBO_HFILES) libomax.a
all: LIBTOOL = libtool -static -o libomax.a $(OMAX_OBJECTS)

win: CFLAGS += $(WIN-CFLAGS)
win: CC = gcc
win: I = $(WIN-INCLUDES)
win: $(LIBO_CFILES) $(LIBO_HFILES) libomax.a
win: LIBTOOL = ar cru libomax.a $(OMAX_OBJECTS)

libomax.a: $(OMAX_OBJECTS) $(OMAX_CFILES) $(OMAX_HFILES)
	$(LIBTOOL)

%.o: %.c %.h 
	$(CC) $(CFLAGS) $(I) -o $(notdir $(basename $@)).o -c $(notdir $(basename $@)).c


.PHONY: clean
clean:
	rm -f *.o *.a

.PHONY: doc
doc:
	cd doc && doxygen Doxyfile
