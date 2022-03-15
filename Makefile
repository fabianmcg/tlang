TC ?= ./build/Debug/tlang
LLVMOPT ?= opt-13
FLAGS ?= ""
file ?= "./examples/EP.tt"

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

.PHONY: EP
EP:
	$(TC) ./examples/EP.tt -o EP.ll
	$(LLVMOPT) -O3 EP.ll -o EP_opt.ll

.PHONY: CG
CG:
	$(TC) ./examples/CG.tt -o CG.ll
	$(LLVMOPT) -O3 CG.ll -o CG_opt.ll

.PHONY: clean
clean:
	rm -vf *.hh *.yy *.ll
