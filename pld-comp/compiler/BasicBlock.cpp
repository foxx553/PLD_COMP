#include "BasicBlock.hpp"
#include "IRInstr.hpp"

BasicBlock::BasicBlock(CFG* cfg, std::string entry_label) : exit_true(nullptr), exit_false(nullptr), name(entry_label), cfg(cfg)
{
}

BasicBlock::~BasicBlock()
{
    for(auto instr: instrs)
    {
        delete instr;
    }
}

const std::string& BasicBlock::get_name() const
{
    return name;
}

const CFG* BasicBlock::get_graph() const 
{
    return cfg;
}

const std::vector<IRInstr*> BasicBlock::get_instructions() const
{
    return instrs;
}

void BasicBlock::add_IRInstr(Operation op, Type t, std::vector<std::string> params)
{
    IRInstr* instr = new IRInstr(this, op, t, params);
    instrs.push_back(instr);
}