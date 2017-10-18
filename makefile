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
	# sed -i 's/return \*new (yyas_<T> ()) T (t)/return \*new (yyas_<T> ()) T (std::move(t))/' parser.hxx

scanner: $(LANG).l
	$(FLEX) $<

scanner.cxx scanner.hxx: scanner

%.o: %.cxx
	$(CXX) $(CFLAGS) -c $< -o $@

$(LANG): parser.o scanner.o
	$(CXX) $(CFLAGS) parser.o scanner.o ast.cpp main.cpp -o $(LANG)

clean:
	$(RM) *.hh *.cxx *.hxx *.o $(LANG)
