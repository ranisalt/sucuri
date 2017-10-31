#include "scanner.hxx"
#include "parser.hxx"

#include <cstdlib>

int main(int argc, char** argv)
{
  int debug_level = argc > 1 ? std::atoi(argv[1]) : 0;

  std::string filename{"stdin"};
  if (argc > 2) {
      filename = argv[2];
  }

  yy::parser::semantic_type yylval;
  yy::parser::location_type yylloc(&filename, 0, 0);
  yy::parser p{yylval, yylloc};
  yyset_in(std::fopen(filename.c_str(), "r"));

  p.set_debug_level(debug_level);

  return p.parse();
}
