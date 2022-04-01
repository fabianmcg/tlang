BASE_DIR = ./tblgen
TG ?= $(BASE_DIR)/build/tlang-tblgen
FLAGS ?= "-I $(BASE_DIR)/td/ -I $(BASE_DIR)/nodes/ --write-if-changed"
INC_DIR ?= ./include/AST/
LIB_DIR ?= ./lib/AST/

.PHONY: all
all: ast common traits inc codegen

.PHONY: ast
ast: $(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=$(BASE_DIR)/nodes/Attr.td OUT_DIR=$(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=$(BASE_DIR)/nodes/Attr.td OUT_DIR=$(LIB_DIR)
	
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=$(BASE_DIR)/nodes/Decl.td OUT_DIR=$(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=$(BASE_DIR)/nodes/Decl.td OUT_DIR=$(LIB_DIR)
	
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=$(BASE_DIR)/nodes/Expr.td OUT_DIR=$(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=$(BASE_DIR)/nodes/Expr.td OUT_DIR=$(LIB_DIR)
	
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=$(BASE_DIR)/nodes/Stmt.td OUT_DIR=$(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=$(BASE_DIR)/nodes/Stmt.td OUT_DIR=$(LIB_DIR)
	
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=hh FILE=$(BASE_DIR)/nodes/Type.td OUT_DIR=$(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=nodes MODE=cc FILE=$(BASE_DIR)/nodes/Type.td OUT_DIR=$(LIB_DIR)

.PHONY: common
common: $(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=common MODE=hh FILE=$(BASE_DIR)/nodes/Common.td OUT_DIR=$(INC_DIR) OUT=Common.hh
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=common MODE=cc FILE=$(BASE_DIR)/nodes/Common.td OUT_DIR=$(LIB_DIR) OUT=Common.cc

.PHONY: traits
traits: $(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=traits MODE=hh FILE=$(BASE_DIR)/nodes/Traits.td OUT_DIR=$(INC_DIR) OUT=Traits.hh
	
.PHONY: inc
inc: $(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=inc MODE=hh FILE=$(BASE_DIR)/nodes/All.td OUT_DIR=$(INC_DIR) OUT=Nodes.inc
	
.PHONY: codegen
codegen: $(INC_DIR)
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=codegen MODE=hh FILE=$(BASE_DIR)/nodes/CodeGen.td OUT_DIR=./include/CodeGen OUT=RecursiveVisitor.hh
	$(MAKE) -f $(BASE_DIR)/Make.run run TG=$(TG) FLAGS=$(FLAGS) ACTION=codegen MODE=cc FILE=$(BASE_DIR)/nodes/CodeGen.td OUT_DIR=./include/CodeGen OUT=Emitable.inc

$(INC_DIR):
	mkdir -pv $(INC_DIR)

$(LIB_DIR):
	mkdir -pv $(LIB_DIR)

.PHONY: clean
clean:
	rm -fv *.yy *.dot *.graphml tmp/*

