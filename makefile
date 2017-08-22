main: sucuri.l sucuri.y
	flex -o sucuri.yy.c sucuri.l
	bison -d -o sucuri.tab.c sucuri.y
	gcc sucuri.tab.c sucuri.yy.c -o sucuri -lfl

clean:
	rm sucuri.tab.h sucuri.tab.c sucuri.yy.c stack.hh
