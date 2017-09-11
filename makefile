BISON ?= bison
CXX ?= g++
CFLAGS += -O2 -Wall -Wextra -lfl
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

$(LANG): parser scanner
	$(CXX) $(CFLAGS) parser.cxx scanner.cxx -o $(LANG)

clean:
	$(RM) *.hh *.cxx *.hxx $(LANG)
