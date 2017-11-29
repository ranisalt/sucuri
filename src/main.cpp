#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <iostream>
#include <string>

#include "symbol.h"

using namespace llvm;

int main(int argc, char** argv)
{
  int debug_level = 0;

  std::string filename{"stdin"};
  for (auto i = 0; i < argc; ++i) {
      if (argv[i] == std::string{"-d"}) {
          try {
              debug_level = std::stoi(argv[i + 1]);
          } catch (const std::invalid_argument& e) {
              std::cerr << "error: debug level must be integer (e.g. -d 0)\n";
              return 1;
          }
      }
  }

  if (argc > 1) {
      filename = argv[argc - 1];
  }

  auto compiler = symbol::Compiler{filename, debug_level};
  auto ret = compiler.compile();

  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();

  auto target_triple = sys::getDefaultTargetTriple();

  std::string error;
  auto target = TargetRegistry::lookupTarget(target_triple, error);
  if (not target) {
    std::cerr << error << std::endl;
  }

  auto reloc_model = Optional<Reloc::Model>();
  auto target_machine = target->createTargetMachine(
      target_triple, "generic", "", {}, reloc_model
  );

  std::error_code ec;
  raw_fd_ostream output{"/dev/stdout", ec, sys::fs::OpenFlags::F_None};
  if (ec) {
    std::cerr << "[Error] Could not open file: " << ec.message() << std::endl;
    return 1;
  }

  legacy::PassManager pass;
  if (target_machine->addPassesToEmitFile(pass, output, {})) {
    std::cerr << "[Error] Target machine can't emit file type." << std::endl;
    return 1;
  }

  auto& module = compiler.module();
  module.setDataLayout(target_machine->createDataLayout());
  pass.run(module);
  output.flush();
  /* output << compiler.program->to_llvm(); */
  return ret;
}
