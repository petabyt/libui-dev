LIBUI=$(abspath libui-ng)

LIBUI_COMMON:=$(patsubst %.c,%.o,$(wildcard $(LIBUI)/common/*.c))
LIBUI_COMMON:=$(filter-out %OLD_table.o,$(LIBUI_COMMON))

LIBUI_COMMON+=extras/favicon.o

CFLAGS=-I$(LIBUI)

ifeq ($(TARGET),win) # ---------------------------
include win.mk
endif # ------------------------------

ifeq ($(TARGET),linux) # ------------------------------
LIBUI_UNIX:=$(patsubst %.c,%.o,$(wildcard $(LIBUI)/unix/*.c))
LIBUI_UNIX:=$(filter-out %OLD_table.o,$(LIBUI_UNIX))

O_FILES:=$(LIBUI_COMMON) $(LIBUI_UNIX)
O_FILES:=$(O_FILES:.o=.$(TARGET).o)

CFLAGS+=$(shell pkg-config --cflags gtk+-3.0) -fPIC
LDFLAGS+=$(shell pkg-config --libs gtk+-3.0)

libui.so: $(O_FILES)
	$(CC) -shared $(O_FILES) $(LDFLAGS) -o libui.so

install:
	sudo rm -rf /usr/local/lib/x86_64-linux-gnu/libui.so
	sudo ln -s $(PWD)/libui.so /usr/local/lib/x86_64-linux-gnu/libui.so

endif # ------------------------------------

%.$(TARGET).o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

%.$(TARGET).o: %.cpp
	$(CPP) -c $< $(CFLAGS) -o $@

clean:
	make TARGET=linux clean_
	make TARGET=win clean_

clean_:
	$(RM) $(O_FILES) *.o *.exe *.out *.res *.so *.a
