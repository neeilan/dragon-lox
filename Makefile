CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
TARGET := bin/dragon-lox

LLVM_CONFIG = /Users/neeilanselvalingam/Desktop/llvm/build/bin/llvm-config
LLVM_COMPILE_FLAGS = --cxxflags
LLVM_LINK_FLAGS = --ldflags --libs --system-libs
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall -std=c++11
INC := -I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ `$(LLVM_CONFIG) $(LLVM_LINK_FLAGS)` -o $(TARGET)"; $(CC) $^ `$(LLVM_CONFIG) $(LLVM_LINK_FLAGS)` -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) `$(LLVM_CONFIG) $(LLVM_COMPILE_FLAGS)`  $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) `$(LLVM_CONFIG) $(LLVM_COMPILE_FLAGS)` $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

test:
	@echo " Running functional tests...";
	python test/functional_tests.py

# Tools
generate_ast: $(TARGET)
	$(CC) $(CFLAGS) tools/generate_ast.cpp $(INC) -o bin/generate_ast

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cpp $(INC) -o bin/tester

.PHONY: clean test
