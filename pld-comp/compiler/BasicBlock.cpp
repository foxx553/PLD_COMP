#include "BasicBlock.hpp"

BasicBlock::BasicBlock(CFG* cfg, std::string entry_label)
{

}

void BasicBlock::gen_asm(std::ostream &o)
{
    //To do 

}

void BasicBlock::add_IRInstr(IRInstr::Operation op, Type t, std::vector<std::string> params)
{
    IRInstr* instr = new IRInstr(this,op,t,params);
    instrs.push_back(instr);

}