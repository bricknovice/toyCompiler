#include <iostream>
#include "node.h"
#include "codegen.h"
#include "ObjGen.h"
using namespace std;

//root of AST
extern NBlock* programBlock;
extern int yyparse();

int main(int argc, char** argv){
	yyparse();
	CodeGenContext context;
	context.generateCode(*programBlock);
	context.theModule->print(llvm::errs(), nullptr);
	cout<<"\n";
	ObjGen(context);
	
	return 0;
}