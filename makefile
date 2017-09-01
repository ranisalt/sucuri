BISON ?= bison
CC := g++
CFLAGS += -lfl -g
FLEX ?= flex
LANG = sucuri

all: $(LANG)

$(LANG).tab.c $(LANG).tab.h: $(LANG).yy
	$(BISON) -d -o $(LANG).tab.c $<

$(LANG).yy.c: $(LANG).ll $(LANG).tab.h
	$(FLEX) -o $(LANG).yy.c $<

$(LANG): $(LANG).yy.c $(LANG).tab.c
	$(CC) $(CFLAGS) $(LANG).tab.c $(LANG).yy.c -o $(LANG)

clean:
	$(RM) -f $(LANG).tab.h $(LANG).tab.c $(LANG).yy.c $(LANG)
