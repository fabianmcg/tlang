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
	./build/Debug/tlang

.PHONY: clean
clean:
	rm -v *.hh *.yy
