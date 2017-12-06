#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <iostream>
#include <string>

#include "parser.hxx"

using namespace llvm;

int main(int argc, char** argv)
{
  int debug_level = 0;

  std::string filename{"stdin"};
  std::string output_filename{"/dev/stdout"};
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
      output_filename = argv[argc - 1];
      std::cout << output_filename << '\n';
      --argc;
  }

/*   if (argc > 1) { */
/*       filename = argv[argc - 1]; */
/*   } */

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
  module->setDataLayout(target_machine->createDataLayout());

  std::error_code ec;
  raw_fd_ostream output{output_filename, ec, sys::fs::OpenFlags::F_None};
  if (ec) {
    std::cerr << "[Error] Could not open file: " << ec.message() << std::endl;
    return 1;
  }

  legacy::PassManager pass;
  if (target_machine->addPassesToEmitFile(pass, output, TargetMachine::CGFT_ObjectFile)) {
    std::cerr << "[Error] Target machine can't emit file type." << std::endl;
    return 1;
  }

  yy::parser::semantic_type yylval;
  yy::parser::location_type yylloc{&filename, 0, 0};
  yy::parser parser{yylval, yylloc};

  auto ret = parser.parse();
  if (ret != 0) {
    return ret;
  }

  /* pass.run(*module); */
  /* module->print(output, nullptr); */
  /* output.flush(); */

  return ret;
}
