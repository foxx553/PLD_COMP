#include "Scope.hpp"
#include <stdexcept>

Scope::Scope(Scope* parent) : parent(parent)
{
}

const Symbol& Scope::add_symbol(int offset, std::string name, Type type, int length)
{
    if(symbols.count(name) != 0)
    {
        throw std::invalid_argument("Scope::add_symbol: symbol already exists");
    }

    symbols[name] = {name, type, offset, length};

    return symbols[name];
}

const Symbol& Scope::add_symbol(const Symbol& symbol)
{
    if(symbols.count(symbol.get_name()) != 0)
    {
        throw std::invalid_argument("Scope::add_symbol: symbol already exists");
    }

    symbols[symbol.get_name()] = symbol;
    return symbol;
}

const Symbol& Scope::create_temp(int symbol_offset, int temp_offset, Type type)
{
    auto name = "tmp_" + std::to_string(temp_offset);
    return add_symbol(symbol_offset, name, type);
}

const Symbol Scope::get_symbol(std::string name) const
{
    const Symbol* symbol = nullptr;

    const Scope* current = this;
    while(current != nullptr)
    {
        if(current->symbols.count(name) != 0)
        {
            symbol = &current->symbols.at(name);
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