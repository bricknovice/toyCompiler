#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "iostream"
using namespace llvm;
using namespace std;
namespace{
    //Implements what the pass does
    void visitor(Function& F){
        errs()<<"(Analyzer)Hello from: "<<F.getName()<<"\n";
        errs()<<"(Analyzer)Number of arguments: " << F.arg_size() << "\n";
    }


    struct Analyzer : PassInfoMixin<Analyzer>{
        //Main entry point, takes IR unit to run the pass on (&F) and the
        // corresponding pass manager (to be queried if need be)
        PreservedAnalyses run(Function& F, FunctionAnalysisManager &){
            visitor(F);
            return PreservedAnalyses::all();
        }
    };
}

//PM Registration
llvm::PassPluginLibraryInfo getAnalyzerPluginInfo(){
    return{LLVM_PLUGIN_API_VERSION, "Analyzer", LLVM_VERSION_STRING,
            //Callback function, use in pass pipeline.
            //Pass pipeline is a list of pass you want to run. It is something you will specify on the command line when running opt.
            //This tells the passbuilder when it pass in the passpipeline, use this function.
            //It check the name of the pass going to be passed, if it exist then we're going to use it.
            [](PassBuilder &PB){
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, FunctionPassManager &FPM, 
                    ArrayRef<PassBuilder::PipelineElement>){
                        if(Name == "analyzer"){
                            FPM.addPass(Analyzer());
                            return true;
                        }
                        return false;
                    });
            }};
}
// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize HelloWorld when added to the pass pipeline on the
// command line, i.e. via '-passes='analyzer'
// PassPluginLibraryInfo() contains api version, plugin name, version of plugin and callback for registering plugin passes with passbuilder.
extern "C" LLVM_ATTRIBUTE_WEAK::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo(){
    return getAnalyzerPluginInfo();
}
