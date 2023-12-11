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

clean:
	rm *.o *.out
