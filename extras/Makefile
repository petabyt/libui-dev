include cross.mk

# Daniel's target
all:
	make ARCH=x86_64-w64-mingw32 libui_x86_64-w64-mingw32.a -j`nproc`
	make ARCH=x86_64-linux-gnu libui_gtk3_x86_64-linux-gnu.so -j`nproc`
	make ARCH=aarch64-linux-gnu libui_gtk3_aarch64-linux-gnu.so -j`nproc`
	darling shell make TARGET=m libui_x86_64.dylib -j`nproc`

LIBUI_COMMON := $(patsubst %.c,%.o,$(wildcard common/*.c))
LIBUI_COMMON := $(filter-out %OLD_table.o,$(LIBUI_COMMON))

CFLAGS := -I. -Iinclude/ -O2 -Isubprojects/cmocka-1.1.5/include

ifeq ($(TARGET),w) # ++++++++++++++++++++++++++
LIBUI_COMMON += extras/favicon/win.o extras/scroll.o extras/win_init.o

LIBUI_WIN := $(patsubst %.cpp,%.o,$(wildcard windows/*.cpp))
LIBUI_WIN := $(filter-out OLD_,$(LIBUI_WIN))

O_FILES := $(LIBUI_COMMON) $(LIBUI_WIN)
O_FILES := $(call convert_target,$(O_FILES))

# Yes, lots of libs required
LIBS := -luser32 -lkernel32 -lgdi32 -lcomctl32 -luxtheme -lmsimg32 -lcomdlg32 -ld2d1 -ldwrite -lole32 -loleaut32 -loleacc
LIBS += -lstdc++ -lgcc -lpthread -lssp -lurlmon -luuid

CFLAGS += -Iwindows -fvisibility=hidden -fdiagnostics-color=always -D_FILE_OFFSET_BITS=64 -Wall -Winvalid-pch -Wextra -Wpedantic -Wno-unused-parameter -Wno-switch -D_UI_STATIC -Dlibui_EXPORTS
CXXFLAGS += -std=c++11 -Wnon-virtual-dtor

libui_$(ARCH).a: $(O_FILES)
	$(AR) rsc libui_$(ARCH).a $(O_FILES)

endif # ------------------------------

ifeq ($(TARGET),l) # ++++++++++++++++++++++++++
LIBUI_COMMON += extras/favicon/linux.o extras/label.o

LIBUI_UNIX := $(patsubst %.c,%.o,$(wildcard unix/*.c))
LIBUI_UNIX := $(filter-out %OLD_table.o,$(LIBUI_UNIX))
LIBUI_UNIX := $(filter-out %image.o,$(LIBUI_UNIX))
LIBUI_UNIX += extras/image/image.o

O_FILES := $(LIBUI_COMMON) $(LIBUI_UNIX)
O_FILES := $(call convert_target,$(O_FILES))

CFLAGS += $(shell pkg-config --cflags gtk+-3.0) -fPIC
LDFLAGS += $(shell pkg-config --libs gtk+-3.0)

libui_gtk3_$(ARCH).so: $(O_FILES)
	$(CC) -shared $(O_FILES) $(LDFLAGS) -o libui_gtk3_$(ARCH).so

ARCH := $(shell uname -m)
install: libui_gtk3_$(ARCH).so
	cp -rf libui_gtk3_$(ARCH).so /usr/lib/$(ARCH)/libui.so
	cp -rf include/ui.h /usr/include/$(ARCH)/ui.h

ex.out: libui.so example/main.o
	$(CC) $(CFLAGS) example/main.o -L. -lui $(LDFLAGS) -o ex.out

endif # ------------------------------------

ifeq ($(TARGET),m) # ++++++++++++++++++++++++++
LIBUI_COMMON += extras/favicon/darwin.o

LIBUI_DARWIN := $(patsubst %.m,%.o,$(wildcard darwin/*.m))
LIBUI_DARWIN := $(filter-out %OLD_table.o,$(LIBUI_DARWIN))

O_FILES := $(LIBUI_COMMON) $(LIBUI_DARWIN)
O_FILES := $(call convert_target,$(O_FILES))

LDFLAGS := -framework Foundation -framework Appkit -framework CoreText -framework CoreGraphics -Wl,-undefined,dynamic_lookup

libui_x86_64.dylib: $(O_FILES)
	$(CC) -shared $(O_FILES) $(LDFLAGS) -o libui_x86_64.dylib

install: libui_x86_64.dylib
	cp libui_x86_64.dylib /usr/local/lib
	cp ui.h /usr/local/include

endif # ---------------------

include examples.mk

clean:
	$(RM) *.o *.exe *.out *.so *.a build *.dylib example/*.out ide/*.o *.dll
	$(RM) `find windows unix darwin test examples -regex ".*\.\(o\|d\|res\)"`
