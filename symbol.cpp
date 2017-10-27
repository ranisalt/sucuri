#include "symbol.h"

#include <iostream>
#include <iterator>
#include <algorithm>

using namespace AST;

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
    std::cout << "opening scope" << std::endl;
    scope_decls.emplace_back(std::vector<DeclInfo>{});
}

void symbol::close_scope() {
    std::cout << "closing scope\n";
    scope_decls.pop_back();
}

bool symbol::has_symbol(const symbol_t& s) {
    std::cout << "\033[1;33mchecking existence of " << s.to_string() << "\033[0m\n";
    for (auto i = scope_decls.rbegin(); i != scope_decls.rend(); ++i) {
        const auto& v = *i;
        if (std::find(begin(v), end(v), s) != std::end(v)) {
            std::cout << "\033[1;32mexists!\033[0m\n";
            return true;
        }
    }
    std::cout << "\033[1;31mdoesn't exists...\033[0m\n";
    return false;
}
