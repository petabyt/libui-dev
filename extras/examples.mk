#ide/demo.h: ide/test.lua
#	cd ide && xxd -i test.lua > demo.h
#ide/test.c: ide/demo.h

TESTER_FILES := $(addprefix test/,drawtests.o images.o main.o menus.o page1.o page2.o page3.o page4.o page5.o page6.o page7.o page7a.o page7b.o page7c.o page11.o page12.o page13.o page14.o page15.o page16.o page17.o spaced.o)

TESTER_FILES_WIN := $(call convert_target,$(TESTER_FILES),w)
tester.exe: $(TESTER_FILES_WIN) test/resources.res libui_x86_64_win.a
	$(CC) -static $(TESTER_FILES_WIN) libui_x86_64_win.a test/resources.res $(LIBS) -o tester.exe

TESTER_FILES_UNIX := $(call convert_target_to,$(TESTER_FILES),l)
tester.out: $(TESTER_FILES_UNIX)
	$(CC) $(TESTER_FILES_UNIX) -lui -o tester.out

UNIT_FILES := $(patsubst %.c,%.$(TARGET).o,$(wildcard test/unit/*.c)) subprojects/cmocka-1.1.5/src/cmocka.o
unit.exe: $(UNIT_FILES) test/unit/resources.res libui_x86_64_win.a
	$(CC) -static $(UNIT_FILES) libui_x86_64_win.a test/unit/resources.res $(LIBS) -o unit.exe
