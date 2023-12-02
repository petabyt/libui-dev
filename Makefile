CC=x86_64-w64-mingw32-gcc
CC=x86_64-w64-mingw32-c++
LIBUI=$(abspath libui-ng)

LIBUI_WIN=$(patsubst %.cpp,%.o,$(wildcard $(LIBUI)/windows/*.cpp))
LIBUI_COMMON=$(patsubst %.c,%.o,$(wildcard $(LIBUI)/common/*.c))

LIBUI_WIN := $(filter-out OLD_,$(LIBUI_WIN))
LIBUI_COMMON := $(filter-out %OLD_table.o,$(LIBUI_COMMON))

CPP=x86_64-w64-mingw32-c++

CFLAGS=-I$(LIBUI)

O_FILES=$(LIBUI_COMMON) $(LIBUI_WIN)

LIBS=-luser32 -lkernel32 -lgdi32 -lcomctl32 -luxtheme -lmsimg32 -lcomdlg32 -ld2d1 -ldwrite -lole32 -loleaut32 -loleacc
LIBS+=-lstdc++ -lgcc -static -s -lpthread -lssp
LIBS+=-lurlmon -luuid

LDFLAGS=$(LIBS)

libui_win64.a: $(O_FILES)
	x86_64-w64-mingw32-ar rsc libui.a $(O_FILES)

build: libui_win64.a
	-mkdir build
	cd $(LIBUI) && meson setup build && ninja -C build
	cp $(LIBUI)/build/meson-out/libui.so build/libui_linux64.so
	cp $(LIBUI)/ui.h build/ui.h
	cp libui_win64.a build/libui_win64.a

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

%.o: %.cpp
	$(CPP) -c $< $(CFLAGS) -o $@

# Test
win.res: example/a.rc
	x86_64-w64-mingw32-windres -I$(LIBUI)/windows example/a.rc -O coff -o win.res
ex.exe: example/main.c win.res libui_win64.a
	$(CC) $(CFLAGS) example/main.c libui.a win.res $(LIBS) -o ex.exe

clean:
	$(RM) $(O_FILES) *.o *.exe *.out *.res *.so *.a
