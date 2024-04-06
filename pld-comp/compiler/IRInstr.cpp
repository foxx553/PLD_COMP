#include "IRInstr.hpp"

IRInstr::IRInstr(BasicBlock* block, Operation op, Type type, const std::vector<Symbol>& params) : block(block), op(op), type(type), params(params)
{
}

const Symbol& IRInstr::get_param(int i) const
{
    return params[i];
}

const Operation IRInstr::get_operation() const
{
    return op;
}

const std::vector<Symbol>& IRInstr::get_params() const
{
    return params;
}

const Type IRInstr::get_type() const
{
    return type;
}