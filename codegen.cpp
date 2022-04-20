#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
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
void printIR(Value* val){
    val->print(errs());
    fprintf(stderr, "\n");
    return ;
}
static Type* typeOf(CodeGenContext& context, const NIdentifier& type){
    if(type.name.compare("int") == 0){
        return Type::getInt64Ty(*context.llvmContext);
    }
    else if(type.name.compare("double") == 0){
        return Type::getDoubleTy(*context.llvmContext);
    }else if(type.name.compare("bool") == 0){
        return Type::getInt1Ty(*context.llvmContext);
    }
    return Type::getVoidTy(*context.llvmContext);
}

void CodeGenContext::generateCode(NBlock& root){
    //cout << "Generating IR code" << endl;

    //cout<<"Creating main function..."<<"\n";
    std::vector<Type*> sysArgs;
    FunctionType* mainFuncType = FunctionType::get(Type::getVoidTy(*this->llvmContext), makeArrayRef(sysArgs), false);
    Function* mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main",*this->theModule.get());
    //cout<<"Creating the block of main function..."<<"\n";
    BasicBlock* block = BasicBlock::Create(*this->llvmContext, "entry", mainFunc);
    this->builder->SetInsertPoint(block);
    pushBlock(block);
    root.codeGen(*this);
    this->builder->CreateRetVoid();
    popBlock();
    
    legacy::PassManager PM;
    PM.add(createPrintModulePass(outs()));
    PM.run(*this->theModule);
    if(verifyModule(*this->theModule)){
        LogErrorV("Module error");
        return ;
    }
    //cout<< "IR code generate success\n";
    return ;
}

Value* NInteger::codeGen(CodeGenContext& context) {
    //cout<<"Generating Integer: "<<this->value<<'\n';
    Value* rst = ConstantInt::get(Type::getInt64Ty(*context.llvmContext), this->value, true);
    //cout<<"Double Integer success: "<<this->value<<'\n';
    return rst;
}

Value* NDouble::codeGen(CodeGenContext& context){
    //cout<<"Generating Double: "<<this->value<<'\n';
    Value* rst = ConstantFP::get(Type::getDoubleTy(*context.llvmContext), this->value);
    //cout<<"Double generate success: "<<this->value<<'\n';
    return rst;
}
Value* NMethodCall::codeGen(CodeGenContext& context){
    //cout << "Generating method call of " << this->Callee.name << "\n";
    Function* function = context.theModule->getFunction(this->Callee.name.c_str());
    //Function* function = context.theModule->getFunction(val->getName());
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
    Value* rst = context.builder->CreateCall(function, argsv, "calltmp");
    //cout << "Method call success: "<< this->Callee.name << "\n";

    return rst;
}
// int a = 10 or int a
Value* NVariableDeclaration::codeGen(CodeGenContext& context){
    //cout << "Generating variable declaration of " << this->type.name << " " << this->id.name << '\n';
    
    //Allocating memory for allocedVar

    Value* allocedVar = context.builder->CreateAlloca(typeOf(context, this->type), nullptr, this->id.name);
    //store allocaed Var into our local symbol table
    context.blocksStack.back()->localsVars[this->id.name] = allocedVar;
    
    //若是int a = 10(右值有東西)，則做assignment
    if( this->assignmentExpr != nullptr ){
        NAssignment assignment(this->id, *(this->assignmentExpr));
        assignment.codeGen(context);
    }
    
    //cout << "Variable declaration generate success: "<< this->type.name << " " << this->id.name <<"\n";
    return allocedVar;
}

Value* NIdentifier::codeGen(CodeGenContext& context){
    //cout << "Find identifier " << this->name << endl;
    //Looking up the identifier on symbol table.
    //Value* value = context.globalVars[this->name];
    Value* value = context.blocksStack.back()->localsVars[this->name];
    Type* Ty = value->getType()->getPointerElementType();
    if(!value)
        return LogErrorV("Unknown variable name");
    Value* rst = context.builder->CreateLoad(Ty, value, false, this->name);
    //cout << "Identifier generate success: "<< this->name<<"\n";
    return rst;
}

Value* NBinaryOperator::codeGen(CodeGenContext& context){
    //cout<<"Generating binary operator"<<'\n';

    Value* L = this->lhs.codeGen(context);
    Value* R = this->rhs.codeGen(context);
    
    //Determine whether this is a float point operator.
    bool fp = false;

    // If one of the operand is double ,do type upgrade(int to double)
    if( (L->getType()->getTypeID() == Type::DoubleTyID) || (R->getType()->getTypeID() == Type::DoubleTyID) ){  
        fp = true;
        if( (R->getType()->getTypeID() != Type::DoubleTyID) ){
            R = context.builder->CreateUIToFP(R, Type::getDoubleTy(*context.llvmContext), "ftmp");
        }
        if( (L->getType()->getTypeID() != Type::DoubleTyID) ){
            L = context.builder->CreateUIToFP(L, Type::getDoubleTy(*context.llvmContext), "ftmp");
        }
    }

    if( !L || !R ){
        return nullptr;
    }

    switch (this->op){
        case TPLUS:
            return fp ? context.builder->CreateFAdd(L, R, "addftmp") : context.builder->CreateAdd(L, R, "addtmp");
        case TMINUS:
            return fp ? context.builder->CreateFSub(L, R, "subftmp") : context.builder->CreateSub(L, R, "subtmp");
        case TMUL:
            return fp ? context.builder->CreateFMul(L, R, "mulftmp") : context.builder->CreateMul(L, R, "multmp");
        case TDIV:
            return fp ? context.builder->CreateFDiv(L, R, "divftmp") : context.builder->CreateSDiv(L, R, "divtmp"); 
        case TAND: 
            return fp ? LogErrorV("Invalid AND operation") : context.builder->CreateAnd(L, R, "andtmp");
        case TOR: 
            return fp ? LogErrorV("Invalid OR operation") : context.builder->CreateOr(L, R, "ortmp");
        case TXOR: 
            return fp ? LogErrorV("Invalid XOR operation") : context.builder->CreateXor(L, R, "xortmp");
        case TLSFT: 
            return fp ? LogErrorV("Invalid LSHIFT operation") : context.builder->CreateShl(L, R, "lsfttmp");
        case TRSFT: 
            return fp ? LogErrorV("Invalid RSHIFT operation") : context.builder->CreateAShr(L, R, "rsfttmp");
        case TCLT:
            return fp ? context.builder->CreateFCmpULT(L, R, "cmpftmp") : context.builder->CreateICmpULT(L, R, "cmptmp");
        case TCLE:
            return fp ? context.builder->CreateFCmpOLE(L, R, "cmpftmp") : context.builder->CreateICmpSLE(L, R, "cmptmp");
        case TCGE:
            return fp ? context.builder->CreateFCmpOGE(L, R, "cmpftmp") : context.builder->CreateICmpSGE(L, R, "cmptmp");
        case TCGT:
            return fp ? context.builder->CreateFCmpOGT(L, R, "cmpftmp") : context.builder->CreateICmpSGT(L, R, "cmptmp");
        case TCEQ:
            return fp ? context.builder->CreateFCmpOEQ(L, R, "cmpftmp") : context.builder->CreateICmpEQ(L, R, "cmptmp");
        case TCNE:
            return fp ? context.builder->CreateFCmpONE(L, R, "cmpftmp") : context.builder->CreateICmpNE(L, R, "cmptmp");
        default:
            return LogErrorV("Unknow binary operator");
    }
    //cout<<"Binary operator generate success"<<'\n';
}

// a = 10
//不會有 int a = 10，那是NVariableDeclaration
Value* NAssignment::codeGen(CodeGenContext& context){
    //cout << "Generating assignment of " << this->lhs.name << " = " << endl;
    Value* dst = nullptr;
    for(auto it = context.blocksStack.rbegin();it!=context.blocksStack.rend();++it){
        if((*it)->localsVars.find(this->lhs.name) !=(*it)->localsVars.end()){
            dst = (*it)->localsVars[this->lhs.name];
        }
    }
    if(!dst)
        return LogErrorV("Undeclared variable");
    

    Value* Val = this->rhs.codeGen(context);
    Value* Ptr = dst;
    MaybeAlign Align = MaybeAlign();
    bool isVolatile = false;
    if (!Align) {
      const DataLayout &DL = context.theModule->getDataLayout();
      Align = DL.getABITypeAlign(Val->getType());
    }
    Value* rst = context.builder->Insert(new StoreInst(Val, Ptr, isVolatile, *Align));
    //cout<<"Assignment generate success: "<< this->lhs.name<<'\n';
    return rst;
}

Value* NBlock::codeGen(CodeGenContext& context){
    //cout<<"Generating block"<<'\n';
    Value* last = nullptr;
    //將block中的一個個statements建構出來。
    for(auto it = this->statements.begin(); it!=this->statements.end();it++){
        last = (*it)->codeGen(context);
    }
    //cout<<"Block generate success"<<'\n';
    return last;
}

Value* NExpressionStatement::codeGen(CodeGenContext& context){
    //cout<<"Generating expression statement"<<'\n';
    //cout<<"Expression statement generate success"<<'\n';
    return this->expression.codeGen(context);
}

Value* NFunctionDeclaration::codeGen(CodeGenContext& context){
    //cout<<"Generating function declaration of "<<this->id.name<<'\n';
    std::vector<Type*> argTypes;
    //記載著參數的型別，並由typeof來取得。
    for(auto& arg: this->arguments){
        argTypes.push_back(typeOf(context,arg->type));
    }
    //取得function的型別
    FunctionType* ftype = FunctionType::get(typeOf(context,this->type), argTypes, false);
    //創建function於context中
    Function* function = Function::Create(ftype, GlobalValue::ExternalLinkage, this->id.name.c_str(), *context.theModule.get());

    //用builder將block插入
    BasicBlock* basicBlock = BasicBlock::Create(*context.llvmContext, "entry", function);
    context.builder->SetInsertPoint(basicBlock);
    context.pushBlock(basicBlock);

    auto origin_arg = this->arguments.begin();

    for(auto& ir_arg_it : function->args()){
        //幫參數取unique name
        ir_arg_it.setName((*origin_arg)->id.name);
        context.builder->CreateStore(&ir_arg_it, (*origin_arg)->codeGen(context), false);
        origin_arg++;
    }
    
    this->block.codeGen(context);

    //Generate return value based on the returnVal
    Value* retVal = context.blocksStack.back()->returnVal;
    //cout<<"function return type: "<<retVal->getType()->getTypeID()<<endl;
    //cout<<"my return type: "<<typeOf(context,this->type)->getTypeID()<<endl;

    if(!typeOf(context,this->type)){
        context.builder->CreateRetVoid();
    }else{
        if(retVal->getType() != typeOf(context,this->type)){
            return LogErrorV("return type doesn't match with function type!");
        }else{
            context.builder->CreateRet(retVal);
        }
    } 
    context.popBlock();
    context.builder->SetInsertPoint(context.blocksStack.back()->block);
    //cout<<"Function declaration generate success: "<<this->id.name<<'\n';

    if(verifyFunction(*function)){
        return LogErrorV("Function error");
    }
    return function;
}

llvm::Value* NReturnStatement::codeGen(CodeGenContext& context) {
    //cout << "Generating return statement" << endl;
    Value* returnValue = this->expression->codeGen(context);
    context.blocksStack.back()->returnVal = returnValue;
    //cout << "Return statement generate success" << endl;
    return returnValue;
}

llvm::Value* NIfStatement::codeGen(CodeGenContext& context){
    Value* CondV = this->condition->codeGen(context);
    //if the condition isn't establish.
    if(!CondV)
        return nullptr;
    if(CondV->getType()->getTypeID() == Type::DoubleTyID){
        CondV =  context.builder->CreateFCmpONE(CondV, ConstantFP::get(*context.llvmContext, APFloat(0.0)), "ifcond");
    }else if(CondV->getType()->getTypeID() == Type::IntegerTyID){
        CondV = context.builder->CreateIntCast(CondV, Type::getInt1Ty(*context.llvmContext), true);
        CondV = context.builder->CreateICmpNE(CondV, ConstantInt::get(Type::getInt1Ty(*context.llvmContext), 0, true));
    }

    Function* TheFunction = context.builder->GetInsertBlock()->getParent();

    BasicBlock* ThenBB = BasicBlock::Create(*context.llvmContext, "then", TheFunction);
    BasicBlock* ElseBB = BasicBlock::Create(*context.llvmContext, "else");
    //MergeBB is used to deal with phi function and return
    BasicBlock* MergeBB = BasicBlock::Create(*context.llvmContext, "ifcont");

    //Establish condition branch，but the blocks are not yet be generated
    context.builder->CreateCondBr(CondV, ThenBB, ElseBB);

//Emit then block
    //Insert the builder to top of the currently empty ThenBB
    context.builder->SetInsertPoint(ThenBB);
    //Recursively generate Thenblock
    Value* ThenV = this->ThenBlock->codeGen(context);
    if(!ThenV)
        return nullptr;
    context.builder->CreateBr(MergeBB);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    ThenBB = context.builder->GetInsertBlock();

//Emit else block
    TheFunction->getBasicBlockList().push_back(ElseBB);
    context.builder->SetInsertPoint(ElseBB);
    Value* ElseV = ElseBlock->codeGen(context);
    if (!ElseV)
        return nullptr;
    context.builder->CreateBr(MergeBB);
    // codegen of 'Else' can change the current block, update ElseBB for the PHI.
    ElseBB = context.builder->GetInsertBlock();

// Emit merge block.
    //add the merge block to the function
    TheFunction->getBasicBlockList().push_back(MergeBB);
    //Change the insertion point so that newly created code will go into the “merge” block.
    context.builder->SetInsertPoint(MergeBB);
    // PHINode *PN = context.builder->CreatePHI(Type::getDoubleTy(*context.llvmContext), 2, "iftmp");
    // PN->addIncoming(ThenV, ThenBB);
    // PN->addIncoming(ElseV, ElseBB);
    // return PN;
    
    return nullptr;
}



