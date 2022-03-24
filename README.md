# toyCompiler
## Objective
  This project is mainly praticing the following techniques with a C++ like compiler.
  1. Lexical analyzation(flex)
  2. syntex analyzation + sementic analyzation(bison)
  3. LLVM
## Compile command
   ```
   clang++ -g -O3 -o main  main.cpp parser.cpp tokens.cpp codegen.cpp ObjGen.cpp `llvm-config --cxxflags --ldflags --system-libs --libs` -w -pthread
   ```
## Examples
  ```
  xxxxxx@xxxxxx-xx-xxxx:~/toyCompiler$ ./main
  int main(int a){ int b = a + 10 }
  ; ModuleID = 'first jit project'
  source_filename = "first jit project"

  define void @main() {
  entry:
  }

  define i64 @main.1(i64 %a) {
  entry:
    %a1 = alloca i64, align 8
    store i64 %a, i64* %a1, align 8
    %b = alloca i64, align 8
    %a2 = load i64*, i64* %a1, align 8
    %addtmp = add i64* %a2, i64 10
    store i64* %addtmp, i64* %b, align 8
  }
  ```
## TODO
  1. .obj generation
  2. JIT and optimizer support
  3. Cmake build 
## Referencing Websites
  1. [Write you own toy compiler](https://gnuu.org/2009/09/18/writing-your-own-toy-compiler/)
  2. [TinyCompiler-stardust95](https://github.com/stardust95/TinyCompiler)
  3. [My First Language Frontend with LLVM Tutorial](https://github.com/stardust95/TinyCompiler)
