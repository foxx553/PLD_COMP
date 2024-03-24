#include "IRInstr.hpp"

IRInstr::IRInstr(BasicBlock* bb_, Operation op, Type t, std::vector<std::string> params)
{
    this->params = params;
    this->op = op;
    this->bb = bb_;
    this->t = t;
}

IRInstr::IRInstr(Operation op, Type t, std::vector<std::string> params)
{
    this->params = params;
    this->op = op;
    this->t = t;
}

const std::string& IRInstr::get_param(int i) const
{
    return params[i];
}

const Operation IRInstr::get_operation() const
{
    return op;
}
