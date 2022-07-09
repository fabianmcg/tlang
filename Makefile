C_COMPILER ?= cc
CXX_COMPILER ?= c++

CLANG ?= clang-13
CLANGXX ?= clang++-13

LLVMOPT ?= opt-13
OPTFLAGS ?= -O3
TC ?= ./build/Debug/tlang
FLAGS ?= 
file ?= ./examples/main.tt

.PHONY: all
all:

.PHONY: build
build:
	$(MAKE) -f Makefile.build CLANG=$(CLANG) CLANGXX=$(CLANGXX) C_COMPILER=$(C_COMPILER) CXX_COMPILER=$(CXX_COMPILER)

.PHONY: runtime
runtime:
	$(MAKE) -f Makefile.build CLANG=$(CLANG) CLANGXX=$(CLANGXX) C_COMPILER=$(C_COMPILER) CXX_COMPILER=$(CXX_COMPILER) runtime

.PHONY: lang
lang: lexer ast parser

.PHONY: lexer
lexer:
	python3 lang/langGen.py lexer -c lang/Conf -t lang/Templates -o include/Lex

.PHONY: ast
ast:
	$(MAKE) -f tblgen.make

.PHONY: parser
parser:
	python3 lang/langGen.py parser -c lang/Conf -t lang/Templates -o lib/Parser

.PHONY: run
run:
ifneq ($(file),)
	$(TC) $(file) $(FLAGS) -o $(addsuffix .ll,$(basename $(file)))
endif

.PHONY: dump
dump:
ifneq ($(file),)
	$(TC) $(file) $(FLAGS) -n -d
endif

.PHONY: emit
emit:
ifneq ($(file),)
	$(CLANG) -S -emit-llvm $(file) $(FLAGS) -o $(addsuffix .ll,$(basename $(file)))
endif

.PHONY: emit
emitCXX:
ifneq ($(file),)
	$(CLANGXX) -S -emit-llvm $(file) $(FLAGS) -o $(addsuffix .ll,$(basename $(file)))
endif

.PHONY: clean
clean:
	rm -vf *.hh *.yy *.ll examples/*.ll examples/*.rt.cpp examples/*devbin

.PHONY: purge
purge:
	rm -vf *.hh *.yy *.ll examples/*.ll examples/*.rt.cpp examples/*devbin
	$(MAKE) -f Makefile.build purge
