#pragma once

#include <ostream>
#include "CFG.hpp"

class Backend 
{
public:
    Backend(IR& ir, std::ostream& o);
    virtual ~Backend();

    void generate();

protected:
    virtual void graph(CFG* cfg);
    virtual void block(BasicBlock* bb);
    virtual void instruction(IRInstr* instr);

    virtual void begin() = 0;
    virtual void graph_begin(CFG* cfg) = 0;
    virtual void graph_end(CFG* cfg) = 0;
    virtual void block_jump_conditional(BasicBlock* bb) = 0;
    virtual void block_jump_simple(BasicBlock* bb) = 0;
    virtual void block_begin(BasicBlock* bb) = 0;
    virtual void instruction_ldconst(IRInstr* instr) = 0;
    virtual void instruction_copy(IRInstr* instr) = 0;
    virtual void instruction_add(IRInstr* instr) = 0;
    virtual void instruction_sub(IRInstr* instr) = 0;
    virtual void instruction_mul(IRInstr* instr) = 0;
    virtual void instruction_div(IRInstr* instr) = 0;
    virtual void instruction_ret(IRInstr* instr) = 0;
    virtual void instruction_rmem(IRInstr* instr) = 0;
    virtual void instruction_wmem(IRInstr* instr) = 0;
    virtual void instruction_call(IRInstr* instr) = 0;
    virtual void instruction_cmp_eq(IRInstr* instr) = 0;
    virtual void instruction_cmp_lt(IRInstr* instr) = 0;
    virtual void instruction_cmp_le(IRInstr* instr) = 0;

    IR& ir;
    std::ostream& o;
};