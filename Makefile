LIBUI=$(abspath libui-ng)

LIBUI_COMMON:=$(patsubst %.c,%.o,$(wildcard $(LIBUI)/common/*.c))
LIBUI_COMMON:=$(filter-out %OLD_table.o,$(LIBUI_COMMON))

LIBUI_COMMON+=extras/favicon.o

CFLAGS=-I$(LIBUI)

ifeq ($(TARGET),w) # ---------------------------
include win.mk
endif # ------------------------------

ifeq ($(TARGET),l) # ------------------------------
LIBUI_UNIX:=$(patsubst %.c,%.o,$(wildcard $(LIBUI)/unix/*.c))
LIBUI_UNIX:=$(filter-out %OLD_table.o,$(LIBUI_UNIX))

O_FILES:=$(LIBUI_COMMON) $(LIBUI_UNIX)
O_FILES:=$(O_FILES:.o=.$(TARGET).o)

CFLAGS+=$(shell pkg-config --cflags gtk+-3.0) -fPIC
LDFLAGS+=$(shell pkg-config --libs gtk+-3.0)

libui.so: $(O_FILES)
	$(CC) -shared $(O_FILES) $(LDFLAGS) -o libui.so

install: libui.so
	sudo rm -rf /usr/local/lib/x86_64-linux-gnu/libui.so
	sudo cp libui.so /usr/local/lib/x86_64-linux-gnu/libui.so

ex.out: example/main.c
	$(CC) $(CFLAGS) example/main.c -lui -o ex.out

ide.out: ide/libuilua.c ide/test.c
	$(CC) ide/libuilua.c ide/test.c -lui -ldl $(shell pkg-config --libs --cflags lua-5.3) -o ide.out

ide.AppImage:
	linuxdeploy --appdir=AppDir --executable=ide.out -d ide/ide.desktop -i ide/libui.png
	appimagetool AppDir

endif # ------------------------------------

ifeq ($(TARGET),m) # ------------------

LIBUI_DARWIN:=$(patsubst %.m,%.o,$(wildcard $(LIBUI)/darwin/*.m))
LIBUI_DARWIN:=$(filter-out %OLD_table.o,$(LIBUI_DARWIN))

O_FILES:=$(LIBUI_COMMON) $(LIBUI_DARWIN)
O_FILES:=$(O_FILES:.o=.$(TARGET).o)

LDFLAGS=-framework Foundation -framework Appkit

libui.dylib: $(O_FILES)
	$(CC) -shared $(O_FILES) $(LDFLAGS) -o libui.dylib

ex-mac:
	$(CC) $(CFLAGS) example/main.c -L. -lui -o ex.out

endif # ---------------------

%.$(TARGET).o: %.m
	$(CC) -c $< $(CFLAGS) -o $@

%.$(TARGET).o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

%.$(TARGET).o: %.cpp
	$(CPP) -c $< $(CFLAGS) -o $@

clean:
	make TARGET=l clean_
	make TARGET=w clean_

clean_:
	$(RM) $(O_FILES) *.o *.exe *.out *.res *.so *.a build *.dylib example/*.out ide/*.o *.res
