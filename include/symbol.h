#pragma once

#include <optional>
#include <string>

#include "parser.hxx"
#include "ast.h"

namespace symbol {

using DeclInfo = AST::Name;

class Compiler {
    using parser_t = yy::parser;
public:
    Compiler(std::string filename,
            int debug_level = 0):
        debug_level{debug_level},
        filename{std::move(filename)},
        yylloc{&this->filename, 0, 0},
        parser{yylval, yylloc, *this}
    {}

    int compile();

    void add_symbol(DeclInfo);
    void open_scope();
    void close_scope();

    bool has_symbol(const DeclInfo&);

    std::optional<AST::Node> lookup(const DeclInfo&, const std::string& trailer = {});

    void import_module(const std::string&);
    void import_module(const std::vector<std::string>& path);

private:
    int debug_level{0};
    std::string filename;

    parser_t::semantic_type yylval;
    parser_t::location_type yylloc;
    parser_t parser;
};


class undeclared_variable: std::exception {
public:
    undeclared_variable(const DeclInfo& s):
        what_(s.to_string())
    {}

    const char* what() const noexcept override{
        return what_.c_str();
    }

private:
    std::string what_;
};


class import_error: std::exception {
public:
    import_error(const std::string& filename, const std::string desc):
        what_(std::string{"module '"} + filename + "' " + desc)
        {}

    const char* what() const noexcept override {
        return what_.c_str();
    }

private:
    std::string what_;
};


}
