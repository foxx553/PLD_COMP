#pragma once

#include "Backend.hpp"

class BackendARM : public Backend
{
public:
    using Backend::Backend;
    virtual ~BackendARM();

private:
    void begin();
    void graph_begin(CFG* cfg);
    void graph_end(CFG* cfg);
    void block_jump_conditional(BasicBlock* bb);
    void block_jump_simple(BasicBlock* bb);
    void block_begin(BasicBlock* bb);    
    void instruction_ldconst(IRInstr* instr);
    void instruction_copy(IRInstr* instr);
    void instruction_add(IRInstr* instr);
    void instruction_sub(IRInstr* instr);
    void instruction_mul(IRInstr* instr);
    void instruction_div(IRInstr* instr);
    void instruction_ret(IRInstr* instr);
    void instruction_rmem(IRInstr* instr);
    void instruction_wmem(IRInstr* instr);
    void instruction_call(IRInstr* instr);
    void instruction_cmp_eq(IRInstr* instr);
    void instruction_cmp_lt(IRInstr* instr);
};