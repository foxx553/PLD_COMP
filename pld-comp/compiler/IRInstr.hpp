#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Symbol.hpp"
#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

class BasicBlock;

/** The instructions themselves -- feel free to subclass instead */
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

//! The class for one 3-address instruction
class IRInstr
{

public:
    IRInstr(BasicBlock* block, Operation op, Type type, const std::vector<Symbol>& params);

    const Symbol&              get_param(int i) const;
    const std::vector<Symbol>& get_params() const;
    const Operation            get_operation() const;
    const Type                 get_type() const;

private:
    BasicBlock*         block;  ///< Associated block
    Operation           op;     ///< Operation
    Type                type;   ///< Operand types
    std::vector<Symbol> params; ///< Parameters

    /**< For 3-op instrs: d, x, y; for ldconst: d, c;  For call: label, d, params;  for wmem and rmem: choose yourself */
    // if you subclass IRInstr, each IRInstr subclass has its parameters and the previous (very important) comment becomes useless: it would be a better design.
};