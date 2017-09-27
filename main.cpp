#include "parser.hxx"

int main(int argc, char** argv)
{
  int debug_level = argc > 1 ? std::atoi(argv[1]) : 0;
  yy::parser::semantic_type yylval;
  yy::parser::location_type yylloc;
  yy::parser p{yylval, yylloc};
  p.set_debug_level(debug_level);
  int res = p.parse();
  return res;
}
