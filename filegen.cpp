#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/ADT/Optional.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include "codegen.h"
#include "filegen.h"

using namespace llvm;
using namespace std;


void BitGen(CodeGenContext& context){
    auto filename = "module.bc";
    //File descriptor
    std::error_code EC;
    raw_fd_ostream dest(filename, EC, sys::fs::OF_None);
    if(EC){
        errs() << "Could not open file: "<<EC.message();
        return;
    }
	llvm::WriteBitcodeToFile(*context.theModule, dest);
    cout<<"bitcode generated!"<<"\n";
    dest.flush();
    return ;
}

void ObjGen(CodeGenContext& context){
    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    //To specify the architecture that you want to target, we use a string called a “target triple”. This takes the form <arch><sub>-<vendor>-<sys>-<abi>
    auto targetTriple = sys::getDefaultTargetTriple();
    context.theModule->setTargetTriple(targetTriple);

    std::string error;
    auto Target = TargetRegistry::lookupTarget(targetTriple, error);

    if( !Target ){
        errs() << error;
        return;
    }

    //We’ll use the generic CPU without any additional features, options or relocation model.
    auto CPU = "generic";
    auto features = "";
    
    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TM = Target->createTargetMachine(targetTriple, CPU, features, opt, RM);

    //We’re now ready to configure our module, to specify the target and data layout. This isn’t strictly necessary, but the frontend performance guide recommends this. Optimizations benefit from knowing about the target and data layout.
    context.theModule->setDataLayout(TM->createDataLayout());
    //context.theModule->setTargetTriple(targetTriple);
    std::error_code EC;

    auto filename = "output.o";
    //File descriptor
    raw_fd_ostream dest(filename, EC, sys::fs::OF_None);

    if(EC){
        errs() << "Could not open file: "<<EC.message();
        return;
    }
    legacy::PassManager PM;
    auto FileType = CGFT_ObjectFile;
    if (TM->addPassesToEmitFile(PM, dest, nullptr, FileType)){
        errs() << "TargetMachine can't emit a file of this type";
        return ;
    }
    PM.run(*context.theModule);
    cout<<"objcode generated!"<<"\n";
    dest.flush();
    return ;
}
