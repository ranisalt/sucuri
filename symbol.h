#pragma once

#include <string>

#include "ast.h"

namespace symbol {

using symbol_t = AST::Name;

void add_symbol(symbol_t);
void open_scope();
void close_scope();
bool has_symbol(const symbol_t&);
void import_module(const std::string&);
void import_module(const std::vector<std::string>& path);
void import_module(const std::vector<AST::Identifier>& path);

class undeclared_variable: std::runtime_error {
public:
    undeclared_variable(const symbol_t& s):
        std::runtime_error(s.to_string())
    {}
};

class import_error: std::runtime_error {
public:
    import_error(const std::string& filename, const std::string desc):
        std::runtime_error(std::string{"module '"} + filename + "' " + desc)
    {}

    const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};

}
