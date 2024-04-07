#pragma once

#include <string>

enum class Type
{
    INT_64 = 0,
    INT_32,
    INT_16,
    INT_8,
    VOID
};

class CFG;

class Symbol
{
public:
    enum class Nature
    {
        VARIABLE,
        REGISTER,
        CONSTANT,
        GLOBAL,
        NAME
    };

    Symbol();
    Symbol(std::string name, Type type, int offset, int length = 1, Nature nature = Nature::VARIABLE, bool pointer = false); // variable constructor
    Symbol(std::string name, Nature nature, int offset = 0);                                                                 // name / register constructor
    Symbol(int value, Type type = Type::INT_64);                                                                             // constant constructor

    const Type         get_type() const;
    const std::string& get_name() const;
    const int          get_offset() const;
    const int          get_size() const;
    const int          get_value() const;
    const Nature       get_nature() const;
    const bool         is_pointer() const;

    static const int         get_type_size(Type t);
    static const Type        type_from_string(const std::string& type);
    static const std::string type_to_string(Type type);

private:
    Type        type;
    std::string name;
    int         offset;
    int         length;
    int         value;
    Nature      nature;
    bool        pointer;
};