#include "Symbol.hpp"

#include <iostream>

Symbol::Symbol() : type(Type::INT_64), offset(0), nature(Nature::VARIABLE), length(0), value(0)
{
}

Symbol::Symbol(std::string name, Type type, int offset, int length, Nature nature, bool pointer) : name(name), type(type), offset(offset), nature(nature), length(length), pointer(pointer)
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

const bool Symbol::is_pointer() const
{
    return pointer;
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
        return 0;
    }
}

const Type Symbol::type_from_string(const std::string& type)
{
    if(type == "int")
    {
        return Type::INT_64;
    }
    else if(type == "char")
    {
        return Type::INT_8;
    }
    else if(type == "void")
    {
        return Type::VOID;
    }

    return Type::INT_64;
}

const std::string Symbol::type_to_string(Type type)
{
    switch(type)
    {
    case Type::INT_64:
        return "int_64";
    case Type::INT_32:
        return "int_32";
    case Type::INT_16:
        return "int_16";
    case Type::INT_8:
        return "int_8";
    default:
        return "void";
    }
}