# Installation
clang, opt, and llvm-link tools in LLVM is required
	git clone https://github.com/llvm/llvm-project.git
	cd llvm-project
	mkdir build
	cd build
	cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" -DLLVM_ENABLE_PROJECTS="clang" ../llvm
	cmake --build . --target opt;clang;llvm-link -j {the number of parallel jobs you want}
	cd tools/clang
	sudo cmake -P cmake_install.cmake
	cd ../opt
	sudo cmake -P cmake_install.cmake
	cd ../llvm-link
	sudo cmake -P cmake_install.cmake
	cd ../llvm-symbolizer
	sudo cmake -P cmake_install.cmake
check clang, opt, and llvm-link tools are well installed
	whereis clang++
	whereis clang
	whereis opt
	whereis llvm-link
PAPI, a pmu monitoring tool is used
	git clone https://github.com/pyrovski/papi.git
	cd papi/src
	./configure --prefix=$PWD/install
	make && make install
Pyhon3 is required. Install matplotlib if not exist
	pip install matplotlib
	pip install PyQt6
# Code
It consists of LLVMProfiler.so (LLVM Pass code), profiler.bc (profiling code)
# How to Use
	make profiler
	make test # to test
	write TARGET_DIR, TARGET_CODE, TARGET_OUTPUT, TARGET_ARG in Makefile and run make target
# If you want to write code yourself
	clang(++) -emit-llvm -c {target code (.c, .cpp)}
	opt -load {location to LLVMProfiler.so} -enable-new-pm=0 --profilerModule <{target code (.bc)}> /dev/null -o {loaded code (.bc)}
	llvm-link profiler.bc {loaded code (.bc)} -o {linked code (.bc)}
	clang(++) {PAPI install location}/lib/libpapi.so.6.0 -c {linked code (.bc)}
	LD_LIBRARY_PATH=:{PAPI install locatoin}/lib {execution file}
