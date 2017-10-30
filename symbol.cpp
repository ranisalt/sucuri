#include "symbol.h"

#include <experimental/filesystem>

#include <iostream>
#include <iterator>
#include <algorithm>

#include "utils.h"

using namespace std::string_literals;
using namespace std::experimental::filesystem;

using namespace AST;
using namespace utils;

/* using DeclInfo = std::pair<Name, parser::location_type>; */
using DeclInfo = Name;
std::vector<std::vector<DeclInfo>> scope_decls = { {} };

void symbol::add_symbol(symbol_t s) {
    std::cout << "\tdeclaring " << s.to_string();
    scope_decls.back().push_back(s);
    std::cout << " (";
    for (const auto& s: scope_decls.back()) {
        std::cout << s.to_string() << ", ";
    }
    std::cout << ")\n";
}

void symbol::open_scope() {
    scope_decls.emplace_back(std::vector<DeclInfo>{});
    std::cout << " >>> " << scope_decls.size() << "\n";
}

void symbol::close_scope() {
    std::cout << " <<< " << scope_decls.size() << "\n";
    scope_decls.pop_back();
}

bool symbol::has_symbol(const symbol_t& s) {
    for (auto i = scope_decls.rbegin(); i != scope_decls.rend(); ++i) {
        const auto& v = *i;
        if (std::find(begin(v), end(v), s) != std::end(v)) {
            return true;
        }
    }
    return false;
}

void symbol::import_module(const std::string& filename) {
    std::cout << "importing " << filename << "\n";
    if (not exists(filename)) {
        throw import_error(filename, "not found");
    }
    if (is_directory(filename)) {
        throw import_error(filename, "is a directory");
    }
}

void symbol::import_module(const std::vector<std::string>& path) {
    const auto filename = join("/"s, path);
    import_module(filename);
}

void symbol::import_module(const std::vector<Identifier>& path) {
    const auto filename = join("/"s, path);
    import_module(filename);
}
