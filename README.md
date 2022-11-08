The source code has LLVMProfiler.so (LLVM Pass code), profiler.cpp and profiler-link.cpp (profiling code).  
While compiling, the target code passes the LLVM Pass code, to insert the functions for profiling into the target code. And the compiled target is executed inside the thread of a executable file.
# Installation
Clang, opt, and llvm-link tools in LLVM is required.
```
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" -DLLVM_ENABLE_PROJECTS="clang" ../llvm
cmake --build . --target opt;clang;llvm-link 
make
cd tools/clang
sudo cmake -P cmake_install.cmake
cd ../opt
sudo cmake -P cmake_install.cmake
cd ../llvm-link
sudo cmake -P cmake_install.cmake
cd ../llvm-symbolizer
sudo cmake -P cmake_install.cmake
```
Check clang, opt, and llvm-link tools are well installed.
```
whereis clang++ clang opt llvm-link
```
PAPI, a pmu monitoring tool is used.
```
git clone https://github.com/pyrovski/papi.git
cd papi/src
./configure --prefix=$PWD/install
make && make install
```
Python3 and python library, matplotlib and PyQt6 are required for drawing graph and displaying data.
```
pip install matplotlib
pip install PyQt6
```
# How to Use
```	
make profiler
```
Run test.
```
make test
```
To execute on the target file, write TARGET\_DIR, TARGET\_CODE, TARGET\_OUTPUT, TARGET\_ARG in Makefile.
If you want to compile inside of the target program directory, not using this Makefile, you may need to define "PROFILER\_DIR" macro in src/profiler.cpp to let know the record folder directory which is also used when drawing graph and displaying results.
```
make target
make run
```
To see the records, execute this.
```
./display.sh {record folder name (something 10 digits)}
```
display.sh executes two python files. One is for drawing graphs and the other is for displaying them with a new interface. If you want only one to be executed, execute each file in the src directory.
```
cd src
python3 graph.py {record folder name}
python3 interface.py {record folder name}
```
You can also check graph png files and data files in the record folder.
