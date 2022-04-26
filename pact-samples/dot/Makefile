CLANG ?= clang-13
CLANGXX ?= clang++-13
LLVMOPT ?= opt-13
LLC ?= llc-13
LLVM_AR ?= llvm-ar-13

CXX ?= nvc++

TLANG ?= ../../build/Debug/tlang

PTXAS ?= ptxas
GPU ?= sm_70
CUDA_LIB ?= /usr/local/cuda/lib64

OPTFLAGS ?= -O3
CFLAGS ?= -I../../runtime/include
LFLAGS ?= -L../build/runtime/ -ltlang-device-nvptx -L$(CUDA_LIB) -lcudart -lcuda
EXTRA_FLAGS ?=

project ?= dot

$(project).ll: $(project).tt
	$(TLANG) $(project).tt -o $(project).ll


dump: $(project).tt
	$(TLANG) $(project).tt -n -d -S

tlang: $(project).ll
	$(LLVMOPT) $(OPTFLAGS) -S $(project).ll -o $(project).opt.ll
	$(LLC) $(OPTFLAGS) -filetype=obj $(project).opt.ll -o $(project).o
	$(LLVMOPT) $(OPTFLAGS) -S $(project).device.ll -o $(project).device.opt.ll
	$(LLC) $(OPTFLAGS) -mcpu=$(GPU) $(project).device.opt.ll -o $(project).device.ptxas
	$(PTXAS) $(OPTFLAGS) -arch $(GPU) $(project).device.ptxas -o tlang.device.devbin
	$(CLANGXX) $(CFLAGS) $(OPTFLAGS) $(EXTRA_FLAGS) -c $(project).rt.cpp -o $(project).rt.o
	$(LLVM_AR) rc lib$(addsuffix .a, $(basename $(file))) $(project).rt.o $(project).o
	$(CLANGXX) -std=c++17 $(project).cc -o $(project).tlang.exe -L./ -l$(project) $(CFLAGS) -$(LFLAGS)


opemp:
	$(CXX) $(OPTFLAGS) -arch $(GPU) -DOMP_VER -std=c++17 $(project).cc -o $(project).omp.exe

.PHONY: clean
clean:
	rm -vf *.opt.ll *.a *.o *.exe *.ptxas *.devbin *.ll *.rt.cpp
