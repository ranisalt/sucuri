# Project
#-----------------------------------------------------------------------------
# Output
LANG         := sucuri
BISONREPORT  := bison-report.output

# Directories
BUILDDIR     := $(CURDIR)/build
GRAMMARDIR   := $(CURDIR)/grammar
INCLUDEDIR   := $(CURDIR)/include
SRCDIR       := $(CURDIR)/src

# Flex/Bison generated files
SCANNERHEADER  := $(INCLUDEDIR)/scanner.hxx
SCANNERSOURCE  := $(SRCDIR)/scanner.cxx

PARSERHEADER  := $(INCLUDEDIR)/parser.hxx
PARSERSOURCE  := $(SRCDIR)/parser.cxx

FLEXBISONFILES := \
				  $(BISONREPORT) \
	              $(PARSERHEADER) \
	              $(SCANNERHEADER) \
				  $(INCLUDEDIR)/location.hh \
				  $(INCLUDEDIR)/position.hh \
				  $(INCLUDEDIR)/stack.hh

LLVM_MODULES := core
LLVM_CXXFLAGS := $(shell llvm-config --cxxflags) -fexceptions
LLVM_LDFLAGS := $(shell llvm-config --ldflags)
LLVM_LIBRARIES := $(shell llvm-config --libs $(LLVM_MODULES))

# Tools
#-----------------------------------------------------------------------------
BISON        ?= bison
FLEX         ?= flex

CXX          ?= g++
CXXFLAGS     += $(LLVM_CXXFLAGS) -std=c++17 -Wall -Wextra -g -I$(INCLUDEDIR) -O0
LDFLAGS      += $(LLVM_LDFLAGS) $(LLVM_LIBRARIES) -lstdc++fs

# Automation
#-----------------------------------------------------------------------------
LEXFILE      := $(GRAMMARDIR)/$(LANG).l
SYNTAXFILE   := $(GRAMMARDIR)/$(LANG).y

SUBDIRS      := $(wildcard */)

CXXSOURCES   := $(SCANNERSOURCE) $(PARSERSOURCE)
CXXOBJECTS   := $(patsubst %.cxx,%.oxx,$(CXXSOURCES))

CPPSOURCES   := $(wildcard $(SRCDIR)/*.cpp)
CPPOBJECTS   += $(patsubst %.cpp,%.o,$(CPPSOURCES))

OBJECTS      := $(CXXOBJECTS) $(CPPOBJECTS)

# Rules
#-----------------------------------------------------------------------------
all: $(LANG)

.PHONY: parser scanner clean

parser: $(PARSERSOURCE)
scanner: $(SCANNERSOURCE)

$(PARSERSOURCE): $(SYNTAXFILE)
	@echo -e \
		"\e[32m====================================================================\
	     \n=\e[1m Compiling parser...\e[0;32m\
	     \n====================================================================\e[0m"
	@mkdir -p $(INCLUDEDIR)
	@mkdir -p $(SRCDIR)
	$(BISON) --defines=$(PARSERHEADER) --report-file=$(BISONREPORT) -o $@ $<
	mv $(SRCDIR)/*.hh $(INCLUDEDIR)
	@echo -e \
	    "\e[32m=\e[1m Done...\e[0;32m\
	     \n====================================================================\e[0m"

$(SCANNERSOURCE): $(LEXFILE) $(PARSERSOURCE)
	@echo -e \
		"\e[32m====================================================================\
	     \n=\e[1m Compiling scanner...\e[0;32m\
	     \n====================================================================\e[0m"
	@mkdir -p $(INCLUDEDIR)
	@mkdir -p $(SRCDIR)
	$(FLEX) --header-file=$(SCANNERHEADER) -o $@ $<
	@echo -e \
	    "\e[32m=\e[1m Done...\e[0;32m\
	     \n====================================================================\e[0m"

$(CXXOBJECTS): %.oxx : %.cxx
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(CPPOBJECTS): %.o : %.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(LANG): $(OBJECTS)
	@echo -e \
		"\e[32m====================================================================\
	     \n=\e[1m Linking...\e[0;32m\
	     \n====================================================================\e[0m"
	@echo $^
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
	@echo -e \
	    "\e[32m=\e[1m Done...\e[0;32m\
	     \n====================================================================\e[0m"

# Directories
# General rules
clean:
	$(RM) $(OBJECTS) $(LANG)
	$(RM) $(CXXSOURCES)
	$(RM) -r $(BUILDDIR)
	$(RM) $(FLEXBISONFILES)
