# toyCompiler
## Objective
  This project is mainly praticing the following techniques with a C++ like compiler.
  1. Lexical analyzation(flex)
  2. syntex analyzation + sementic analyzation(bison)
  3. LLVM
## Compile command
  * Create the parser.cpp and parser.hpp files
    ```
    bison -d -o parser.cpp parser.y
    ```

  * Create the tokens.cpp file
    ```
    lex -o tokens.cpp tokens.l
    ```

  * Building AST 
    ```
    clang++ -g -O3 -o main  main.cpp parser.cpp tokens.cpp codegen.cpp filegen.cpp `llvm-config --cxxflags --ldflags --system-libs --libs` -w -pthread
    ```

  * Write down your program and generate corresponding object file(output.o) and bitcode (module.bc). For example,
    ```
    ./main
    double func1(int a, int b){ return a + b + 1.0 }
    func1(1,2)
    (Notice: Quit by sending an EOF via CTRL+D)
    ```

  * Create a analyzer pass library by cmake
    ```
    cd Analyzer
    change CMakeLists.txt line10 by setting your CMAKE_PREFIX_PATH
    cmake .
    make
    ```

  * Run "module.bc"(bitcode) with "analyzer"(pass) from libAnalyzer.so twice.
    ```
    opt -load-pass-plugin ./Analyzer/libAnalyzer.so --passes=analyzer,analyzer -disable-output module.bc
    ```
## Examples
  * Write down your program and generate corresponding object file(output.o) and bitcode (module.bc) 
    ```
    xxxxxx@xxxxxx-xx-xxxx:~/toyCompiler$ ./main
    double func1( int a, int b){ double c = 1 return a+b+c}
    func1(2,3)
    ; ModuleID = 'first project'
    source_filename = "first project"

    define void @main() {
    entry:
      %calltmp = call double @func1(i64 2, i64 3)
      ret void
    }

    define double @func1(i64 %a, i64 %b) {
    entry:
      %a1 = alloca i64, align 8
      store i64 %a, i64* %a1, align 4
      %b2 = alloca i64, align 8
      store i64 %b, i64* %b2, align 4
      %c = alloca double, align 8
      store i64 1, double* %c, align 4
      %a3 = load i64, i64* %a1, align 4
      %b4 = load i64, i64* %b2, align 4
      %c5 = load double, double* %c, align 8
      %ftmp = uitofp i64 %b4 to double
      %addftmp = fadd double %ftmp, %c5
      %ftmp6 = uitofp i64 %a3 to double
      %addftmp7 = fadd double %ftmp6, %addftmp
      ret double %addftmp7
    }
    ```
  * Run Analyzer pass
    ```
    opt -load-pass-plugin ./Analyzer/libAnalyzer.so -passes=analyzer -disable-output module.bc
    (Analyzer)Hello from: main
    (Analyzer)Number of arguments: 0
    (Analyzer)Hello from: func1
    (Analyzer)Number of arguments: 2
    ```
## Issue
  * (Solved, The CMAKE_PREFIX_PATH needs to be specified.)opt unable to recognize my custom analyzer pass and report the following error message. 
    > Failed to load passes from 'libAnalyzer.so'. Request ignored. \
    opt: unknown pass name 'analyzer'
  
## Referencing Websites
  1. [Write you own toy compiler](https://gnuu.org/2009/09/18/writing-your-own-toy-compiler/)
  2. [TinyCompiler-stardust95](https://github.com/stardust95/TinyCompiler)
  3. [My First Language Frontend with LLVM Tutorial](https://github.com/stardust95/TinyCompiler)
  4. [2019 LLVM Developers’ Meeting: A. Warzynski “Writing an LLVM Pass: 101”](https://www.youtube.com/watch?v=ar7cJl2aBuU&t=209s)
