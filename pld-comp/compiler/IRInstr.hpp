#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

class BasicBlock;
class CFG;

enum class Type
{
    INT_64,
    INT_32,
    INT_16,
    INT_8
};

/** The instructions themselves -- feel free to subclass instead */
enum class Operation
{
    ldconst, // a = 5
    copy,    // a = b
    add,     // x + y
    sub,     // x - y
    mul,     // x * y
    div,     // x / y
    rmem,    // a = *b
    wmem,    // *b = a
    call,    // fct()
    cmp_eq,  // a == b
    cmp_lt,  // a < b
    ret      // return a
};

//! The class for one 3-address instruction
class IRInstr
{

public:
    /**  constructor */
    IRInstr(BasicBlock* bb_, Operation op, Type t, std::vector<std::string> params);
    IRInstr(Operation op, Type t, std::vector<std::string> params);

    const std::string&              get_param(int i) const;
    const std::vector<std::string>& get_params() const;
    const Operation                 get_operation() const;
    const Type                      get_type() const;

    static int get_type_size(Type t)
    {
        switch(t)
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

private:
    BasicBlock*              bb; /**< The BB this instruction belongs to, which provides a pointer to the CFG this instruction belong to */
    Operation                op;
    Type                     t;
    std::vector<std::string> params; /**< For 3-op instrs: d, x, y; for ldconst: d, c;  For call: label, d, params;  for wmem and rmem: choose yourself */
                                     // if you subclass IRInstr, each IRInstr subclass has its parameters and the previous (very important) comment becomes useless: it would be a better design.
};