# Installation
clang, opt, and llvm-link tools in LLVM is required
```
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
```
check clang, opt, and llvm-link tools are well installed
```
	whereis clang++
	whereis clang
	whereis opt
	whereis llvm-link
```
PAPI, a pmu monitoring tool is used
```
	git clone https://github.com/pyrovski/papi.git
	cd papi/src
	./configure --prefix=$PWD/install
	make && make install
```
Pyhon3 is required. Install matplotlib if not exist
```
	pip install matplotlib
	pip install PyQt6
```
# Code
This consists of LLVMProfiler.so (LLVM Pass code), profiler.cpp and profiler-link.cpp (profiling code)
# How to Use
```	
	make profiler
```
Run test
```
	make test
```
Write TARGET\_DIR, TARGET\_CODE, TARGET\_OUTPUT, TARGET\_ARG in Makefile and run make target
