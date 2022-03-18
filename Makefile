CLANG ?= clang-13
CLANGXX ?= clang++-13
LLVMOPT ?= opt-13
OPTFLAGS ?= -O3
TC ?= ./build/Debug/tlang
FLAGS ?= 
file ?= ./examples/main.tt

.PHONY: all
all:

.PHONY: lang
lang: lexer ast parser

.PHONY: lexer
lexer:
	python3 lang/langGen.py lexer -c lang/Conf -t lang/Templates -o include/Lexer

.PHONY: ast
ast:
	python3 lang/langGen.py ast -c lang/Conf -t lang/Templates -o include/AST

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
	$(TC) $(file) $(FLAGS) -n -d -S
endif

.PHONY: EP
EP:
	$(TC) ./examples/EP.tt -o EP.ll
	$(LLVMOPT) $(OPTFLAGS) -S EP.ll -o EP_opt.ll

.PHONY: CG
CG:
	$(TC) ./examples/CG.tt -o CG.ll
	$(LLVMOPT) $(OPTFLAGS) -S CG.ll -o CG_opt.ll

.PHONY: emit
emit:
ifneq ($(file),)
	$(CLANG) -S -emit-llvm $(file) -o $(addsuffix .ll,$(basename $(file)))
endif

.PHONY: emit
emitCXX:
ifneq ($(file),)
	$(CLANGXX) -S -emit-llvm $(file) -o $(addsuffix .ll,$(basename $(file)))
endif

.PHONY: clean
clean:
	rm -vf *.hh *.yy *.ll
