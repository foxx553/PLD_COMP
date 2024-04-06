#include "Symbol.hpp"

#include <iostream>

Symbol::Symbol() : type(Type::INT_64), offset(0), nature(Nature::VARIABLE), length(0), value(0)
{
}

Symbol::Symbol(std::string name, Type type, int offset, int length) : name(name), type(type), offset(offset), nature(Nature::VARIABLE), length(length)
{
}

Symbol::Symbol(std::string name, Nature nature, int offset) : name(name), type(Type::INT_64), value(0), offset(offset), nature(nature), length(0)
{
}

Symbol::Symbol(int value, Type type) : type(type), value(value), offset(0), nature(Nature::CONSTANT), length(0)
{
}

const Type Symbol::get_type() const
{
    return type;
}

const std::string& Symbol::get_name() const
{
    return name;
}

const int Symbol::get_offset() const
{
    return offset;
}

const int Symbol::get_value() const
{
    return value;
}

const int Symbol::get_size() const
{
    return get_type_size(type) * length;
}

const Symbol::Nature Symbol::get_nature() const
{
    return nature;
}

const int Symbol::get_type_size(Type type)
{
    switch(type)
    {
    case Type::INT_64:
        return 8;
    case Type::INT_32:
        return 4;
    case Type::INT_16:
        return 2;
    case Type::INT_8:
        return 1;
    default:
        return 1;
    }
}