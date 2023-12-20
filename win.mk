LIBUI_WIN:=$(patsubst %.cpp,%.o,$(wildcard $(LIBUI)/windows/*.cpp))
LIBUI_WIN:=$(filter-out OLD_,$(LIBUI_WIN))

O_FILES:=$(LIBUI_COMMON) $(LIBUI_WIN)
O_FILES:=$(O_FILES:.o=.$(TARGET).o)

CPP=x86_64-w64-mingw32-c++
CC=x86_64-w64-mingw32-gcc

LIBS=-luser32 -lkernel32 -lgdi32 -lcomctl32 -luxtheme -lmsimg32 -lcomdlg32 -ld2d1 -ldwrite -lole32 -loleaut32 -loleacc
LIBS+=-lstdc++ -lgcc -static -s -lpthread -lssp
LIBS+=-lurlmon -luuid

LDFLAGS=$(LIBS)

libui_win64.a: $(O_FILES)
	x86_64-w64-mingw32-ar rsc libui_win64.a $(O_FILES)

build: libui_win64.a
	-mkdir build
	cd $(LIBUI) && meson setup build && ninja -C build
	cp $(LIBUI)/build/meson-out/libui.so build/libui_linux64.so
	cp $(LIBUI)/ui.h build/ui.h
	cp libui_win64.a build/libui_win64.a

IDE_FILES=ide/libuilua.w.o ide/test.w.o
CFLAGS+=-I/usr/x86_64-w64-mingw32/include
ide.exe: $(IDE_FILES) libui_win64.a ide/ide.res
	$(CC) $(IDE_FILES) libui_win64.a /usr/x86_64-w64-mingw32/lib/liblua.a ide/ide.res $(LIBS) -o ide.exe

ide/ide.res: ide/a.rc
	x86_64-w64-mingw32-windres -I$(LIBUI)/windows ide/a.rc -O coff -o ide/ide.res

# Test
win.res: example/a.rc
	x86_64-w64-mingw32-windres -I$(LIBUI)/windows example/a.rc -O coff -o win.res
ex.exe: example/main.c win.res libui_win64.a
	$(CC) $(CFLAGS) example/main.c libui.a win.res $(LIBS) -o ex.exe
