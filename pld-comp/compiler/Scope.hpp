#pragma once

#include "Symbol.hpp"
#include <map>

class Scope
{
public:
    Scope(Scope* parent);
    ~Scope();

    const Symbol& add_symbol(int offset, std::string name, Type t, int length = 1);
    const Symbol& add_symbol(const Symbol& symbol);
    const Symbol  get_symbol(std::string name);
    const Symbol& create_temp(int symbol_offset, int temp_offset, Type type);
    Scope*        get_parent();

private:
    Scope*                        parent;       ///< parent scope
    std::map<std::string, Symbol> symbols;      ///< symbol table
    std::map<std::string, bool>   used_symbols; ///< used symbols
};