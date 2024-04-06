#include "BasicBlock.hpp"
#include "CFG.hpp"
#include "IRInstr.hpp"

BasicBlock::BasicBlock(CFG* cfg) : exit_true(nullptr), exit_false(nullptr), cfg(cfg), name(cfg->block_name()), test_symbol()
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

void BasicBlock::add_instruction(Operation op, Type type, const std::vector<Symbol>& params)
{
    auto instruction = new IRInstr(this, op, type, params);
    instrs.push_back(instruction);
}