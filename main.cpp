#include <iostream>
#include "node.h"
//#include "codegen.h"
using namespace std;

extern NBlock* programBlock;
extern int yyparse();

int main(int argc, char** argv){
	yyparse();
	std::cout<<programBlock<<std::endl;
	// 	CodeGenContext context;
	// 	context.generateCode(*programBlock);
	return 0;
}