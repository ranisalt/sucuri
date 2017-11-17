#include <iostream>
#include <string>

#include "symbol.h"

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

  return symbol::Compiler{filename, debug_level}.compile();
}
