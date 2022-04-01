#include <iostream>
#include "node.h"
#include "codegen.h"
#include "filegen.h"
using namespace std;

//root of AST
extern NBlock* programBlock;
extern int yyparse();

int main(int argc, char** argv){
	yyparse();
	CodeGenContext context;
	context.generateCode(*programBlock);
	ObjGen(context);
	BitGen(context);
	return 0;
}