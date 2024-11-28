all:
	make TARGET=w libui_win64.a -j`nproc`
	make TARGET=l libui.so -j`nproc`
	darling shell TARGET=m libui.dylib -j`nproc`

include cross.mk

LIBUI_COMMON := $(patsubst %.c,%.o,$(wildcard common/*.c))
LIBUI_COMMON := $(filter-out %OLD_table.o,$(LIBUI_COMMON))

CFLAGS := -I. -Iinclude/ -g -Isubprojects/cmocka-1.1.5/include

ifeq ($(TARGET),w) # ++++++++++++++++++++++++++
LIBUI_COMMON += extras/favicon/win.o extras/scroll.o extras/win_init.o

LIBUI_WIN := $(patsubst %.cpp,%.o,$(wildcard windows/*.cpp))
LIBUI_WIN := $(filter-out OLD_,$(LIBUI_WIN))

O_FILES := $(LIBUI_COMMON) $(LIBUI_WIN)
O_FILES := $(O_FILES:.o=.$(TARGET).o)

# Yes, lots of libs required
LIBS := -luser32 -lkernel32 -lgdi32 -lcomctl32 -luxtheme -lmsimg32 -lcomdlg32 -ld2d1 -ldwrite -lole32 -loleaut32 -loleacc
LIBS += -lstdc++ -lgcc -lpthread -lssp -lurlmon -luuid

CFLAGS += -Iwindows -fvisibility=hidden -fdiagnostics-color=always -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wnon-virtual-dtor -Wextra -Wpedantic -std=c++11 -Wno-unused-parameter -Wno-switch -D_UI_STATIC -Dlibui_EXPORTS

libui_win64.a: $(O_FILES)
	x86_64-w64-mingw32-ar rsc libui_win64.a $(O_FILES)

endif # ------------------------------

ifeq ($(TARGET),l) # ++++++++++++++++++++++++++
LIBUI_COMMON += extras/favicon/linux.o extras/label.o

LIBUI_UNIX := $(patsubst %.c,%.o,$(wildcard unix/*.c))
LIBUI_UNIX := $(filter-out %OLD_table.o,$(LIBUI_UNIX))
LIBUI_UNIX := $(filter-out %image.o,$(LIBUI_UNIX))
LIBUI_UNIX += extras/image/image.o

O_FILES := $(LIBUI_COMMON) $(LIBUI_UNIX)
O_FILES := $(O_FILES:.o=.$(TARGET).o)

CFLAGS += $(shell pkg-config --cflags gtk+-3.0) -fPIC
LDFLAGS += $(shell pkg-config --libs gtk+-3.0)

libui.so: $(O_FILES)
	$(CC) -shared $(O_FILES) $(LDFLAGS) -o libui.so

ARCH := $(shell uname -m)
install: libui.so
	sudo rm -rf /usr/local/lib/$(ARCH)-linux-gnu/libui.so
	sudo cp libui.so /usr/local/lib/$(ARCH)-linux-gnu/libui.so
	sudo rm -rf /usr/lib/libui.so
	sudo cp libui.so /usr/lib/libui.so
	sudo cp include/ui.h /usr/include/ui.h

ex.out: libui.so example/main.o
	$(CC) $(CFLAGS) example/main.o -L. -lui $(LDFLAGS) -o ex.out

endif # ------------------------------------

ifeq ($(TARGET),m) # ++++++++++++++++++++++++++
LIBUI_COMMON += extras/favicon/darwin.o

LIBUI_DARWIN := $(patsubst %.m,%.o,$(wildcard darwin/*.m))
LIBUI_DARWIN := $(filter-out %OLD_table.o,$(LIBUI_DARWIN))

O_FILES := $(LIBUI_COMMON) $(LIBUI_DARWIN)
O_FILES := $(O_FILES:.o=.$(TARGET).o)

LDFLAGS := -framework Foundation -framework Appkit -framework CoreText -framework CoreGraphics -Wl,-undefined,dynamic_lookup

libui.dylib: $(O_FILES)
	$(CC) -shared $(O_FILES) $(LDFLAGS) -o libui.dylib

install: libui.dylib
	cp libui.dylib /usr/local/lib
	cp ui.h /usr/local/include

endif # ---------------------

clean:
	$(RM) *.o *.exe *.out *.so *.a build *.dylib example/*.out ide/*.o *.dll
	$(RM) `find windows unix darwin test examples -regex ".*\.\(o\|d\|res\)"`
