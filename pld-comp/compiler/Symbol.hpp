#pragma once

#include <string>

/**
 * Type de variable et de fonction
 */
enum class Type
{
    INT_64 = 0,
    INT_32,
    INT_16,
    INT_8,
    VOID
};

class CFG;

/**
 * Symbol
 * Représente un symbole (variable, registre, constante, nom)
 */
class Symbol
{
public:
    /**
     * Nature du symbole
     */
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

    const Type         get_type() const;                                                                                     ///< get the type of the symbol
    const std::string& get_name() const;                                                                                     ///< get the name of the symbol
    const int          get_offset() const;                                                                                   ///< get the offset of the symbol
    const int          get_size() const;                                                                                     ///< get the size of the symbol
    const int          get_value() const;                                                                                    ///< get the value of the symbol
    const Nature       get_nature() const;                                                                                   ///< get the nature of the symbol
    const bool         is_pointer() const;                                                                                   ///< true if the symbol is a pointer

    static const int         get_type_size(Type t);                                                                          ///< get the size of a type
    static const Type        type_from_string(const std::string& type);                                                      ///< convert a string to a type
    static const std::string type_to_string(Type type);                                                                      ///< convert a type to a string

private:
    Type        type;    ///< type of the symbol
    std::string name;    ///< name of the symbol
    int         offset;  ///< offset of the symbol
    int         length;  ///< length of the symbol
    int         value;   ///< value of the symbol
    Nature      nature;  ///< variable, register, constant, global, name
    bool        pointer; ///< true if the symbol is a pointer
};