#pragma once
#ifndef __CODEGEN_H__
#define __CODEGEN_H__
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include "node.h"
#include <stack>
#include <vector>
#include <memory>
#include <string>
#include <map>

using SymTable = std::map<std::string, llvm::Value*>;

class CodeGenBlock {
public:
    llvm::BasicBlock *block;
    llvm::Value* returnVal;
    //local variables
    std::map<std::string, llvm::Value*> locals;
};


class CodeGenContext {
public:
    std::vector<CodeGenBlock *> blocksStack;
    std::unique_ptr<llvm::LLVMContext> llvmContext;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> theModule;

    //global variables
    SymTable globalVars;

    // 利用context建立一個Module空間，並用ptr指向它
    CodeGenContext(){
        this->llvmContext = std::make_unique<llvm::LLVMContext>();
        this->theModule = std::make_unique<llvm::Module>("first project", *llvmContext);
        this->builder = std::make_unique<llvm::IRBuilder<>>(*llvmContext);
    }

    void pushBlock(llvm::BasicBlock* block){
        CodeGenBlock* codeGenBlock = new CodeGenBlock();
        codeGenBlock->block = block;
        blocksStack.push_back(codeGenBlock);
    }

    void popBlock(){
        CodeGenBlock* codeGenBlock = blocksStack.back();
        blocksStack.pop_back();
        delete codeGenBlock;
    }
    void generateCode(NBlock& );
    
};

#endif