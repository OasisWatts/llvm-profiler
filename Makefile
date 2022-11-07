CLANG = clang++
opt = -emit-llvm -c
PASS_LOC = ~/llvm-project/clang_build/lib/LLVMProfiler.so
PAPI_DIR = /mnt/pm9a1/papi/src/install#path to papi install
papi_opt = -I${PAPI_DIR}/include
papi_lib = ${PAPI_DIR}/lib/libpapi.so.6.0

TARGET_DIR =
TARGET_CODE =
TARGET_OUTPUT = a.out
TARGET_ARG =
TEST_DIR = test
TEST_CODE = test.cpp test2.cpp


all: profiler comp run
.PHONY: all

profiler: src/profiler-link.cpp
	$(CLANG) $(opt) $^ -o src/profiler-link.bc

%-link.bc: %.cpp
	$(CLANG) $(opt) $^ -o $(^:%.cpp=%.bc) 	
	opt -load $(PASS_LOC) -enable-new-pm=0 --profilerModule <$(^:%.cpp=%.bc)> /dev/null -o $(^:%.cpp=%-pass.bc)
	llvm-link $(^:%.cpp=%-pass.bc) src/profiler-link.bc -o $(^:%.cpp=%-link.bc)

target: $(TARGET_CODE:%.cpp=TARGET_DIR/%-link.bc)
	$(CLANG) $(papi_opt) $(papi_lib) src/profiler.cpp $^ /home/pm/pmdk/src/nondebug/libvmem.a -pthread -DPROFILER_DIR=/home/oasis/profiler -o $(TARGET_DIR)/$(TARGET_OUTPUT) $(TARGET_ARG)

test: $(TEST_CODE:%.cpp=$(TEST_DIR)/%-link.bc)
	$(CLANG) $(papi_opt) $(papi_lib) src/profiler.cpp $^ /home/pm/pmdk/src/nondebug/libvmem.a -pthread -DPROFILER_DIR=/home/oasis/profiler -o $(TEST_DIR)/a.out
	LD_LIBRARY_PATH=:$(PAPI_DIR)/lib ./$(TEST_DIR)/a.out

clean: 
	rm $(TEST_DIR)/*.bc $(TEST_DIR)/*.out

record-clean-all:
	rm -rf record/*

run :
	LD_LIBRARY_PATH=:$(PAPI_DIR)/lib ./$(TARGET_DIR)/$(TARGET_OUTPUT)
.PHONY: run
