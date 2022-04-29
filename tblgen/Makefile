TG ?= ./build/tlang-tblgen
FLAGS ?= "-I td/ -I nodes/"
OUTPUT_DIR ?= tmp

C_COMPILER ?= clang
CXX_COMPILER ?= clang++

.PHONY: build
build:
	mkdir -pv build
	cd build/ && cmake ../ -DCMAKE_C_COMPILER=$(C_COMPILER) -DCMAKE_CXX_COMPILER=$(CXX_COMPILER) && $(MAKE) -j4

.PHONY: clean_build
clean_build: purge build

.PHONY: all
all: ast common traits inc

.PHONY: ast
ast: $(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=nodes/Attr.td OUT_DIR=$(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=nodes/Attr.td OUT_DIR=$(OUTPUT_DIR)

	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=nodes/Decl.td OUT_DIR=$(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=nodes/Decl.td OUT_DIR=$(OUTPUT_DIR)

	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=nodes/Expr.td OUT_DIR=$(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=nodes/Expr.td OUT_DIR=$(OUTPUT_DIR)

	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=nodes/Stmt.td OUT_DIR=$(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=nodes/Stmt.td OUT_DIR=$(OUTPUT_DIR)

	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=nodes/Type.td OUT_DIR=$(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=nodes/Type.td OUT_DIR=$(OUTPUT_DIR)

.PHONY: common
common: $(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=common MODE=hh FILE=nodes/Common.td OUT_DIR=$(OUTPUT_DIR) OUT=Common.hh
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=common MODE=cc FILE=nodes/Common.td OUT_DIR=$(OUTPUT_DIR) OUT=Common.cc

.PHONY: traits
traits: $(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=traits MODE=hh FILE=nodes/Traits.td OUT_DIR=$(OUTPUT_DIR) OUT=Traits.hh

.PHONY: inc
inc: $(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=inc MODE=hh FILE=nodes/All.td OUT_DIR=$(OUTPUT_DIR) OUT=Nodes.inc

.PHONY: codegen
codegen: $(OUTPUT_DIR)
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=codegen MODE=hh FILE=nodes/CodeGen.td OUT_DIR=$(OUTPUT_DIR) OUT=RecursiveVisitor.hh
	$(MAKE) -f Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=codegen MODE=cc FILE=nodes/CodeGen.td OUT_DIR=$(OUTPUT_DIR) OUT=Emitable.inc

$(OUTPUT_DIR):
	mkdir -pv $(OUTPUT_DIR)

.PHONY: clean
clean:
	rm -fv *.yy *.dot *.graphml tmp/*


.PHONY: purge
purge:
	rm -fv *.yy *.dot *.graphml tmp/*
	rm -rf build
