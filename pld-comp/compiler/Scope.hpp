#pragma once

#include "Symbol.hpp"
#include <map>

/**
 * Class Scope pour la gestion des scopes
 * Permet de gérer les scopes imbriqués
 */
class Scope
{
public:
    Scope(Scope* parent);
    ~Scope();

    const std::map<std::string, Symbol>& get_symbols() const;                                                              ///< get the symbol table
    const Symbol&                        add_symbol(const Symbol& symbol);                                                 ///< add a symbol
    const Symbol                         get_symbol(std::string name);                                                     ///< get a symbol by name
    const Symbol&                        create_temp(int symbol_offset, int temp_offset, Type type, bool pointer = false); ///< create a temporary symbol
    Scope*                               get_parent();                                                                     ///< get the parent scope

private:
    Scope*                        parent;       ///< parent scope
    std::map<std::string, Symbol> symbols;      ///< symbol table
    std::map<std::string, bool>   used_symbols; ///< used symbols
};