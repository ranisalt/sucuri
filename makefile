BISON ?= bison
CXX ?= g++
CFLAGS += -std=c++14 -Wall -Wextra -g
FLEX ?= flex
LANG = sucuri

all: $(LANG)

.PHONY: parser scanner clean

parser: $(LANG).y
	$(BISON) $<

parser.cxx parser.hxx: parser

scanner: $(LANG).l
	$(FLEX) $<

scanner.cxx scanner.hxx: scanner

%.o: %.cxx
	$(CXX) $(CFLAGS) -c $< -o $@

$(LANG): parser.o scanner.o symbol.o
	$(CXX) $(CFLAGS) parser.o scanner.o symbol.o ast.cpp main.cpp -o $(LANG)

clean:
	$(RM) *.hh *.cxx *.hxx *.o $(LANG)
