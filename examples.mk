ide/demo.h: ide/test.lua
	cd ide && xxd -i test.lua > demo.h
ide/test.c: ide/demo.h

TESTER_FILES := $(addprefix test/,drawtests.w.o images.w.o main.w.o menus.w.o page1.w.o page2.w.o page3.w.o page4.w.o page5.w.o page6.w.o page7.w.o page7a.w.o page7b.w.o page7c.w.o page11.w.o page12.w.o page13.w.o page14.w.o page15.w.o page16.w.o page17.w.o spaced.w.o)

tester.exe: $(TESTER_FILES) test/resources.res libui_win64.a
	$(CC) -static $(TESTER_FILES) libui_win64.a test/resources.res $(LIBS) -o tester.exe

UNIT_FILES := $(patsubst %.c,%.$(TARGET).o,$(wildcard test/unit/*.c)) subprojects/cmocka-1.1.5/src/cmocka.w.o
unit.exe: $(UNIT_FILES) test/unit/resources.res libui_win64.a
	$(CC) -static $(UNIT_FILES) libui_win64.a test/unit/resources.res $(LIBS) -o unit.exe
