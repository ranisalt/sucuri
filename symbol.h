#pragma once

#include "ast.h"

namespace symbol {

using symbol_t = AST::Name;

void add_symbol(symbol_t);
void open_scope();
void close_scope();
bool has_symbol(const symbol_t&);

class undeclared_variable: std::runtime_error {
public:
    undeclared_variable(const symbol_t& s):
        std::runtime_error(s.to_string())
    {}
};

}
