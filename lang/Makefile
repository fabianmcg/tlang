.PHONY: all
all:
	python3 langGen.py

.PHONY: frmt
frmt:
	black -t py39 -l 120 langGen.py Conf/ Lang/ Lexer/ Utility/ Grammar/ Cxx/ AST/

.PHONY: clean
clean:
	rm -v *.hh *.yy
