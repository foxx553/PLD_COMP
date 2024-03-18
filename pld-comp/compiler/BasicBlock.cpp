#include "BasicBlock.hpp"
#include "IRInstr.hpp"

BasicBlock::BasicBlock(CFG* cfg, std::string entry_label) : exit_true(nullptr), exit_false(nullptr), label(entry_label), cfg(cfg)
{

}

BasicBlock::~BasicBlock()
{
    for(auto instr: instrs)
    {
        delete instr;
    }
}

void BasicBlock::gen_asm(std::ostream &o)
{
    for (int i = 0; i < instrs.size(); i++) 
    {
        instrs[i]->gen_asm(o);
    }
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, std::vector<std::string> params)
{
    IRInstr* instr = new IRInstr(this, op, t, params);
    instrs.push_back(instr);
}