LUAVER	:= 5.3

CFLAGS	+= -g
CFLAGS	+= -Wall
CFLAGS	+= $(shell pkg-config --cflags lua-$(LUAVER))

LDFLAGS	+= -lui -ldl $(shell pkg-config --libs lua-$(LUAVER))

test.out: test.o libuilua.o
	$(CC) $(CFLAGS) test.o libuilua.o $(LDFLAGS) -o $@

demo.h: test.lua
	xxd -i test.lua > demo.h
test.o: demo.h

appimage:
	mv test.out ide
	linuxdeploy --appdir=AppDir --executable=ide -d ide.desktop -i libui.png
	appimagetool AppDir

clean:
	rm *.o *.out
