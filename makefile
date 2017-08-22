BISON ?= bison
CC ?= gcc
FLEX ?= flex

main: sucuri.l sucuri.y
	$(FLEX) -o sucuri.yy.c sucuri.l
	$(BISON) -d -o sucuri.tab.c sucuri.y
	$(CC) sucuri.tab.c sucuri.yy.c -o sucuri -lfl

clean:
	$(RM) -f sucuri.tab.h sucuri.tab.c sucuri.yy.c
