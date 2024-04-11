#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Symbol.hpp"
#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

class BasicBlock;

enum class Operation
{
    ldconst, // a = 5
    copy,    // a = b
    add,     // x + y
    sub,     // x - y
    bxor,    // x ^ y
    band,    // x & y
    bor,     // x | y
    mul,     // x * y
    div,     // x / y
    mod,     // x % y
    rmem,    // a = *b
    wmem,    // *b = a
    call,    // fct()
    cmp_eq,  // a == b
    cmp_lt,  // a < b
    ret      // return a
};

/**
 * Class IRInstr, qui représente une instruction en code intermédiaire
 */
class IRInstr
{

public:
    IRInstr(BasicBlock* block, Operation op, Type type, const std::vector<Symbol>& params);

    const Symbol&              get_param(int i) const; // Gets the ith parameter
    const std::vector<Symbol>& get_params() const;     // Gets all parameters
    const Operation            get_operation() const;  // Gets the operation type
    const Type                 get_type() const;       // Gets the result type

private:
    BasicBlock*         block;  ///< Associated block
    Operation           op;     ///< Operation
    Type                type;   ///< Operand types
    std::vector<Symbol> params; ///< Parameters
};