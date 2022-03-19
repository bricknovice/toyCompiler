#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Support/raw_ostream.h>
#include "node.h"
#include "codegen.h"
#include "parser.hpp"
using namespace std;
using namespace llvm;

//We pass around the llvmContext when travasel through the ASTNodes.
std::unique_ptr<NExpression> LogError(const char *str) {
    fprintf(stderr, "LogError: %s\n", str);
    return nullptr;
}

Value* LogErrorV(const char *str){
    LogError(str);
    return nullptr;
}

Value* LogErrorV(string str){
    return LogErrorV(str.c_str());
}

static Type* typeOf(CodeGenContext& context, const NIdentifier& type){
    if(type.name.compare("int") == 0){
        cout<<"It's type int64"<<endl;
        return Type::getInt64Ty(context.llvmContext);
    }
    else if(type.name.compare("double") == 0){
        cout<<"It's type double"<<endl;
        return Type::getDoubleTy(context.llvmContext);
    }
    cout<<"It's type void"<<endl;
    return Type::getVoidTy(context.llvmContext);
}

void CodeGenContext::generateCode(NBlock& root){
    cout << "Generating IR code" << endl;

    cout<<"Creating main function..."<<"\n";
    std::vector<Type*> sysArgs;
    FunctionType* mainFuncType = FunctionType::get(Type::getVoidTy(this->llvmContext), makeArrayRef(sysArgs), false);
    Function* mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main");

    cout<<"Creating the block of main function..."<<"\n";
    BasicBlock* block = BasicBlock::Create(this->llvmContext, "entry");
    this->builder.SetInsertPoint(block);
    pushBlock(block);
    Value* retValue = root.codeGen(*this);
    popBlock();

    cout<< "IR code generate success\n";

    return ;
}

Value* NInteger::codeGen(CodeGenContext& context) {
    cout<<"Generating Integer: "<<this->value<<'\n';
    Value* rst = ConstantInt::get(Type::getInt64Ty(context.llvmContext), this->value, true);
    cout<<"Double Integer success: "<<this->value<<'\n';
    return rst;
}

Value* NDouble::codeGen(CodeGenContext& context){
    cout<<"Generating Double: "<<this->value<<'\n';
    Value* rst = ConstantFP::get(Type::getDoubleTy(context.llvmContext), this->value);
    cout<<"Double generate success: "<<this->value<<'\n';
    return rst;
}
Value* NMethodCall::codeGen(CodeGenContext& context){
    cout << "Generating method call of " << this->Callee.name << "\n";
    Function* function = context.theModule->getFunction(this->Callee.name.c_str());
    if(function == NULL)
        std::cerr << "no such function" << Callee.name <<'\n';
    if( function->arg_size() != this->Args.size()) 
        LogErrorV("Function arguments size not match, function=" + std::to_string(function->size()) + ", this->arguments=" + std::to_string(this->Args.size()) );
    vector<Value*> argsv;
    for(auto it=this->Args.begin(); it!=this->Args.end(); it++){
        argsv.push_back((*it)->codeGen(context));
        if( !argsv.back() ){        // if any argument codegen fail
            return nullptr;
        }
    }
    Value* rst = context.builder.CreateCall(function, argsv, "calltmp");
    std::cout << "Method call success: "<< this->Callee.name << "\n";
    return rst;
}
// int a = 10 or int a
Value* NVariableDeclaration::codeGen(CodeGenContext& context){
    cout << "Generating variable declaration of " << this->type.name << " " << this->id.name << '\n';
    //依照type配置記憶體空間給identifier，並存到symbol table
    //Value* allocedVar = context.builder.CreateAlloca(typeOf(context, this->type));
    
    Type *Ty = typeOf(context, this->type);
    Value *ArraySize = nullptr;
    const Twine &Name = "";
    const DataLayout &DL = context.theModule->getDataLayout();
    cout<<"1"<<endl;
    Align AllocaAlign = DL.getPrefTypeAlign(Ty);
    cout<<"2"<<endl;
    unsigned AddrSpace = DL.getAllocaAddrSpace();
    cout<<"3"<<endl;
    Value* allocedVar = context.builder.Insert(new AllocaInst(Ty, AddrSpace, ArraySize, AllocaAlign), Name);
    cout<<"4"<<endl;
    context.blocksStack.back()->locals[this->id.name] = allocedVar;
    cout<<"5"<<endl;
    //若是int a = 10(右值有東西)，則做assignment
    if( this->assignmentExpr != nullptr ){
        NAssignment assignment(this->id, *(this->assignmentExpr));
        assignment.codeGen(context);
    }
    cout << "Variable declaration generate success: "<< this->type.name << " " << this->id.name <<"\n";
    return allocedVar;
}

Value* NIdentifier::codeGen(CodeGenContext& context){
    cout << "Generating identifier " << this->name << endl;
    //Looking up the identifier on symbol table.
    Value* value = context.globalVars[this->name];
    if(!value)
        return LogErrorV("Unknown variable name");
    Value* rst = context.builder.CreateLoad(value->getType(),value, false, "");
    cout << "Identifier generate success: "<< this->name<<"\n";
    return rst;
}

Value* NBinaryOperator::codeGen(CodeGenContext& context){
    cout<<"Generating binary operator"<<'\n';

    Value* L = this->lhs.codeGen(context);
    Value* R = this->rhs.codeGen(context);
    bool fp = false;//Determine whether this is a float point operator.

    if( (L->getType()->getTypeID() == Type::DoubleTyID) || (R->getType()->getTypeID() == Type::DoubleTyID) ){  // If one of the operand is double ,do type upgrade(int to double)
        fp = true;
        if( (R->getType()->getTypeID() != Type::DoubleTyID) ){
            R = context.builder.CreateUIToFP(R, Type::getDoubleTy(context.llvmContext), "ftmp");
        }
        if( (L->getType()->getTypeID() != Type::DoubleTyID) ){
            L = context.builder.CreateUIToFP(L, Type::getDoubleTy(context.llvmContext), "ftmp");
        }
    }
    //???
    if( !L || !R ){
        return nullptr;
    }

    switch (this->op){
        case TPLUS:
            return fp ? context.builder.CreateFAdd(L, R, "addftmp") : context.builder.CreateAdd(L, R, "addtmp");
        case TMINUS:
            return fp ? context.builder.CreateFSub(L, R, "subftmp") : context.builder.CreateSub(L, R, "subtmp");
        case TMUL:
            return fp ? context.builder.CreateFMul(L, R, "mulftmp") : context.builder.CreateMul(L, R, "multmp");
        case TDIV:
            return fp ? context.builder.CreateFDiv(L, R, "divftmp") : context.builder.CreateSDiv(L, R, "divtmp");//What does that S mean ？ 
        default:
            return LogErrorV("Unknow binary operator");
    }
    cout<<"Binary operator generate success"<<'\n';
}

// a = 10
//不會有 int a = 10，那是NVariableDeclaration
Value* NAssignment::codeGen(CodeGenContext& context){
    cout << "Generating assignment of " << this->lhs.name << " = " << endl;
    Value* dst = nullptr;
    for(auto it = context.blocksStack.rbegin();it!=context.blocksStack.rend();++it){
        if((*it)->locals.find(this->lhs.name) !=(*it)->locals.end()){
            dst = (*it)->locals[this->lhs.name];
        }
    }
    if(!dst)
        return LogErrorV("Undeclared variable");
    cout<<"store int issue"<<endl;
    Value *Val = this->rhs.codeGen(context);
    Value *Ptr = dst;
    MaybeAlign Align = MaybeAlign();
    bool isVolatile = false;
    if (!Align) {
      const DataLayout &DL = context.theModule->getDataLayout();
      Align = DL.getABITypeAlign(Val->getType());
    }
    Value* rst = context.builder.Insert(new StoreInst(Val, Ptr, isVolatile, *Align));
    //Value* rst =  context.builder.CreateStore(this->rhs.codeGen(context), dst, false);
    cout<<"store int no issue"<<endl;
    cout<<"Assignment generate success: "<< this->lhs.name<<'\n';
    return rst;
}

Value* NBlock::codeGen(CodeGenContext& context){
    cout<<"Generating block"<<'\n';
    Value* last = nullptr;
    //將block中的一個個statements建構出來。
    for(auto it = this->statements.begin(); it!=this->statements.end();it++){
        last = (*it)->codeGen(context);
    }
    cout<<"Block generate success"<<'\n';
    return last;
}

Value* NExpressionStatement::codeGen(CodeGenContext &context){
    cout<<"Generating expression statement"<<'\n';
    cout<<"Expression statement generate success"<<'\n';
    return this->expression.codeGen(context);
}

Value* NFunctionDeclaration::codeGen(CodeGenContext& context){
    cout<<"Generating function declaration of "<<this->id.name<<'\n';
    std::vector<Type*> argTypes;
    //記載著參數的型別，並由typeof來取得。
    for(auto& arg: this->arguments){
        argTypes.push_back(typeOf(context,arg->type));
    }
    //取得function的型別
    FunctionType* ftype = FunctionType::get(typeOf(context,this->type), argTypes, false);
    //創建function於context中
    Function* function = Function::Create(ftype, GlobalValue::ExternalLinkage, this->id.name.c_str(), context.theModule.get());

    //用builder將block插入
    BasicBlock* basicBlock = BasicBlock::Create(context.llvmContext, "entry", nullptr);
    context.builder.SetInsertPoint(basicBlock);
    context.pushBlock(basicBlock);

    auto origin_arg = this->arguments.begin();

    for(auto& ir_arg_it : function->args()){
        //幫參數取unique name
        ir_arg_it.setName((*origin_arg)->id.name);

        Value* argAlloc = (*origin_arg)->codeGen(context);

        context.builder.CreateStore(&ir_arg_it, argAlloc, false);
        origin_arg++;
    }
    this->block.codeGen(context);
    ReturnInst::Create(context.llvmContext, basicBlock);
    context.popBlock();

    cout<<"Function declaration generate success: "<<this->id.name<<'\n';
    return function;
}





