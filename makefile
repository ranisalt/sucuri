BISON ?= bison
CC ?= gcc
CFLAGS += -lfl
FLEX ?= flex
LANG = sucuri

all: $(LANG)

$(LANG).tab.c $(LANG).tab.h: $(LANG).y
	$(BISON) -d -o $(LANG).tab.c $<

$(LANG).yy.c: $(LANG).l $(LANG).tab.h
	$(FLEX) -o $(LANG).yy.c $<

$(LANG): $(LANG).yy.c $(LANG).tab.c
	$(CC) $(CFLAGS) $(LANG).tab.c $(LANG).yy.c -o $(LANG)

clean:
	$(RM) -f $(LANG).tab.h $(LANG).tab.c $(LANG).yy.c $(LANG)
