#include "Backend.hpp"

Backend::Backend(IR& ir, std::ostream& o): ir(ir), o(o) 
{

}

Backend::~Backend()
{

}

void Backend::generate()
{
    begin();

    for(auto cfg: ir)
    {
        graph(cfg);
    }
}

void Backend::graph(CFG* cfg)
{
    graph_begin(cfg);

    for(const auto& bb: cfg->get_blocks())
    {
        block(bb);

        if(!bb->exit_true) // pas de suite
        {
            graph_end(cfg);
        }
        else if(!bb->exit_false) // suite simple
        {
            block_jump_simple(bb);
        }
        else // suite conditionnelle
        {
            block_jump_conditional(bb);
        }
    }
}

void Backend::block(BasicBlock* bb)
{
    block_begin(bb);

    for(const auto& instr: bb->instrs)
    {
        instruction(instr);
    }
}

void Backend::instruction(IRInstr* instr)
{
    switch(instr->get_operation())
    {
    case Operation::ldconst:
        instruction_ldconst(instr);
        break;
    case Operation::copy:
        instruction_copy(instr);
        break;
    case Operation::add:
        instruction_add(instr);
        break;
    case Operation::sub:
        instruction_sub(instr);
        break;
    case Operation::mul:
        instruction_mul(instr);
        break;
    case Operation::div:
        instruction_div(instr);
        break;
    case Operation::ret:
        instruction_ret(instr);
        break;
    case Operation::rmem:
        instruction_rmem(instr);
        break;
    case Operation::wmem:
        instruction_wmem(instr);
        break;
    case Operation::call:
        instruction_call(instr);
        break;
    case Operation::cmp_eq:
        instruction_cmp_eq(instr);
        break;
    case Operation::cmp_lt:
        instruction_cmp_lt(instr);
        break;
    case Operation::cmp_le:
        instruction_cmp_le(instr);
        break;
    case Operation::cmp_lt_eq:
        instruction_cmp_lt_eq(instr);
        break;
    case Operation::cmp_le_eq:
        instruction_cmp_le_eq(instr);
        break;
    }
}