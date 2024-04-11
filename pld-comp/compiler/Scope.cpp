#include "Scope.hpp"

#include <stdexcept>
#include <iostream>

Scope::Scope(Scope* parent) : parent(parent)
{
}

/**
 * Destructeur
 * Vérifie que toutes les variables déclarées dans le scope ont été utilisées
*/
Scope::~Scope()
{
    for(auto [name, used]: used_symbols)
    {
        if(used == false)
        {
            std::cerr << "Scope: variable " << name << " is not used" << std::endl;
        }
    }
}

const std::map<std::string, Symbol>& Scope::get_symbols() const
{
    return symbols;
}

const Symbol& Scope::add_symbol(const Symbol& symbol)
{
    if(symbols.count(symbol.get_name()) != 0)
    {
        throw std::invalid_argument("Scope::add_symbol: symbol already exists");
    }

    symbols[symbol.get_name()] = symbol;
    used_symbols[symbol.get_name()] = false;

    return symbol;
}

const Symbol& Scope::create_temp(int symbol_offset, int temp_offset, Type type, bool pointer)
{
    auto name = "tmp_" + std::to_string(temp_offset);
    symbols[name] = {name, type, symbol_offset, 1, Symbol::Nature::VARIABLE, pointer};

    return symbols[name];
}

const Symbol Scope::get_symbol(std::string name)
{
    const Symbol* symbol = nullptr;

    Scope* current = this;
    while(current != nullptr)
    {
        if(current->symbols.count(name) != 0)
        {
            symbol = &current->symbols.at(name);
            current->used_symbols[name] = true;
            break;
        }

        current = current->parent;
    }

    if(symbol == nullptr)
    {
        throw std::invalid_argument("Scope::get_symbol: symbol doesn't exist");
    }

    return *symbol;
}

Scope* Scope::get_parent()
{
    return parent;
}