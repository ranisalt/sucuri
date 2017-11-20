#include "symbol.h"

#include <experimental/filesystem>

#include <algorithm>
#include <iostream>
#include <iterator>

#include "scanner.hxx"
#include "parser.hxx"
#include "utils.h"

namespace {

using namespace std::string_literals;
using namespace std::experimental::filesystem;

using namespace AST;
using namespace utils;

/* using DeclInfo = std::pair<Name, parser::location_type>; */
using symbol::DeclInfo;
std::vector<std::vector<DeclInfo>> scope_decls = {{}};

std::optional<AST::Node> lookup(
        const DeclInfo& node,
        const std::string& trailer) {
    for (auto i = scope_decls.rbegin(); i != scope_decls.rend(); ++i) {
        const auto& v = *i;
        auto it = std::find(v.begin(), v.end(), node);
        if (it != v.end()) {
            return *it;
        }
    }
    return {};
}

}

namespace symbol {

std::optional<AST::Node> Compiler::lookup(const DeclInfo& node, const std::string& trailer) {
    return ::lookup(node, trailer);
}

/**
 * Basics
 */

int Compiler::compile() {
    auto toplevel = std::fopen(filename.c_str(), "r");
    yyset_in(toplevel);

    parser.set_debug_level(debug_level);
    auto res = parser.parse();
    std::fclose(toplevel);
    return res;
}

/**
 * Symbol table handling
 */
void Compiler::add_symbol(DeclInfo s) {
    std::cout << "\tdeclaring " << s.to_string();
    scope_decls.back().push_back(s);
    std::cout << " (";
    for (const auto& s: scope_decls.back()) {
        std::cout << s.to_string() << ", ";
    }
    std::cout << ")\n";
}

void Compiler::open_scope() {
    scope_decls.emplace_back(std::vector<DeclInfo>{});
    std::cout << " >>> " << scope_decls.size() << "\n";
}

void Compiler::close_scope() {
    std::cout << " <<< " << scope_decls.size() << "\n";
    scope_decls.pop_back();
}

bool Compiler::has_symbol(const DeclInfo& s) {
    return lookup(s).has_value();
}


/**
 * Modules
 */

void Compiler::import_module(const std::vector<std::string>& path) {
    const auto filename = join("/"s, path);
    import_module(filename);
}

void Compiler::import_module(const std::string& module_name) {
    const auto filename = module_name + ".scr";
    std::cout << "importing " << filename << "\n";
    if (not exists(filename)) {
        throw import_error(filename, "not found");
    }
    if (is_directory(filename)) {
        throw import_error(filename, "is a directory");
    }

    auto module = std::fopen(filename.c_str(), "r");
    //auto _yyloc = p.yylloc;
    yyset_in(module);
    std::fclose(module);
}

}
