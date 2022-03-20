#pragma once
#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

class Node{
public:
	virtual ~Node() {};
	virtual llvm::Value* codeGen(CodeGenContext& context) {return NULL;};
};

class NExpression : public Node{
};

class NStatement : public Node{
};

class NInteger : public NExpression{
public:
	//int64_t value;
	// NInteger(int64_t value) :
	// 	 value(value){};
	long long value;
	NInteger(long long value):
		value(value){};
	llvm::Value* codeGen(CodeGenContext& context) override;
};

class NDouble : public NExpression{
public:
	double value;
	NDouble(double value) : 
		value(value){};
	llvm::Value* codeGen(CodeGenContext& context) override;
};

//a
class NIdentifier : public NExpression{
public:
	std::string name;
	NIdentifier(const std::string& name) : 
		name(name){};
	llvm::Value* codeGen(CodeGenContext& context) override;
};
// method(int a, int b, int c)
class NMethodCall : public NExpression{
public:
	NIdentifier& Callee;
	ExpressionList Args;
	NMethodCall(NIdentifier& Callee, ExpressionList& Args):
		Callee(Callee), Args(Args){};
	NMethodCall(NIdentifier& Callee): 
		Callee(Callee){}
	llvm::Value* codeGen(CodeGenContext& context) override;
};

class NBinaryOperator : public NExpression{
public:
	int op;
	NExpression& lhs;
	NExpression& rhs;
	NBinaryOperator(NExpression& lhs, int op, NExpression& rhs):
		lhs(lhs), rhs(rhs), op(op){};
	llvm::Value* codeGen(CodeGenContext& context) override;
};

//a = b
class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs) : 
        lhs(lhs), rhs(rhs) { }
    llvm::Value* codeGen(CodeGenContext& context) override;
};

//NBlock 產生一或多個statements
class NBlock : public NExpression{
public:
	StatementList statements;
	NBlock() 
		{};
	llvm::Value* codeGen(CodeGenContext& context) override;
};

//statement 由多至一個express組成
class NExpressionStatement : public NStatement {
public:
	NExpression& expression;
	NExpressionStatement(NExpression& expression) : 
        	expression(expression) {};
	llvm::Value* codeGen(CodeGenContext& context) override;
};

// int a = b or int a
class NVariableDeclaration : public NStatement {
public:
    const NIdentifier& type;
    NIdentifier& id;
    NExpression *assignmentExpr;
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
        type(type), id(id), assignmentExpr(nullptr) {};
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression* assignmentExpr) :
        type(type), id(id), assignmentExpr(assignmentExpr) {};
    llvm::Value* codeGen(CodeGenContext& context) override;
};

//type id arguments block = function ast
class NFunctionDeclaration : public NStatement{
public:
	const NIdentifier& type;
	const NIdentifier& id;
	VariableList arguments;
	NBlock& block;
	NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id, const VariableList& arguments, NBlock& block) :
		type(type), id(id), arguments(arguments), block(block) {};
	llvm::Value* codeGen(CodeGenContext& context) override;
};

