#pragma once

#include <ostream>
#include "CFG.hpp"

class Backend 
{
public:
    Backend(IR& ir);
    virtual ~Backend();

    void generate(std::ostream& o);

protected:
    virtual void graph(std::ostream& o, CFG* cfg);
    virtual void block(std::ostream& o, BasicBlock* bb);
    virtual void instruction(std::ostream& o, IRInstr* instr);

    virtual void begin(std::ostream& o) = 0;
    virtual void graph_begin(std::ostream& o, CFG* cfg) = 0;
    virtual void graph_end(std::ostream& o, CFG* cfg) = 0;
    virtual void block_jump_conditional(std::ostream& o, BasicBlock* bb) = 0;
    virtual void block_jump_simple(std::ostream& o, BasicBlock* bb) = 0;
    virtual void block_begin(std::ostream& o, BasicBlock* bb) = 0;
    virtual void instruction_ldconst(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_copy(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_add(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_sub(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_mul(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_div(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_ret(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_rmem(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_wmem(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_call(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_cmp_eq(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_cmp_lt(std::ostream& o, IRInstr* instr) = 0;
    virtual void instruction_cmp_le(std::ostream& o, IRInstr* instr) = 0;

    IR& ir;
};

class BackendASM : public Backend
{
public:
    using Backend::Backend;
    virtual ~BackendASM();

private:
    void begin(std::ostream& o);
    void graph_begin(std::ostream& o, CFG* cfg);
    void graph_end(std::ostream& o, CFG* cfg);
    void block_jump_conditional(std::ostream& o, BasicBlock* bb);
    void block_jump_simple(std::ostream& o, BasicBlock* bb);
    void block_begin(std::ostream& o, BasicBlock* bb);    
    void instruction_ldconst(std::ostream& o, IRInstr* instr);
    void instruction_copy(std::ostream& o, IRInstr* instr);
    void instruction_add(std::ostream& o, IRInstr* instr);
    void instruction_sub(std::ostream& o, IRInstr* instr);
    void instruction_mul(std::ostream& o, IRInstr* instr);
    void instruction_div(std::ostream& o, IRInstr* instr);
    void instruction_ret(std::ostream& o, IRInstr* instr);
    void instruction_rmem(std::ostream& o, IRInstr* instr);
    void instruction_wmem(std::ostream& o, IRInstr* instr);
    void instruction_call(std::ostream& o, IRInstr* instr);
    void instruction_cmp_eq(std::ostream& o, IRInstr* instr);
    void instruction_cmp_lt(std::ostream& o, IRInstr* instr);
    void instruction_cmp_le(std::ostream& o, IRInstr* instr);
};