#include "Backend.hpp"

Backend::Backend(IR& ir): ir(ir) 
{

}

Backend::~Backend()
{

}

void Backend::generate(std::ostream& o)
{
    begin(o);

    for(auto cfg: ir)
    {
        graph(o, cfg);
    }
}

void Backend::graph(std::ostream& o, CFG* cfg)
{
    graph_begin(o, cfg);

    for(const auto& bb: cfg->get_blocks())
    {
        block(o, bb);

        if(!bb->exit_true) // pas de suite
        {
            graph_end(o, cfg);
        }
        else if(!bb->exit_false) // suite simple
        {
            block_jump_simple(o, bb);
        }
        else // suite conditionnelle
        {
            block_jump_conditional(o, bb);
        }
    }
}

void Backend::block(std::ostream& o, BasicBlock* bb)
{
    block_begin(o, bb);

    for(const auto& instr: bb->instrs)
    {
        instruction(o, instr);
    }
}

void Backend::instruction(std::ostream& o, IRInstr* instr)
{
    switch(instr->get_operation())
    {
    case Operation::ldconst:
        instruction_ldconst(o, instr);
        break;
    case Operation::copy:
        instruction_copy(o, instr);
        break;
    case Operation::add:
        instruction_add(o, instr);
        break;
    case Operation::sub:
        instruction_sub(o, instr);
        break;
    case Operation::mul:
        instruction_mul(o, instr);
        break;
    case Operation::div:
        instruction_div(o, instr);
        break;
    case Operation::ret:
        instruction_ret(o, instr);
        break;
    case Operation::rmem:
        instruction_rmem(o, instr);
        break;
    case Operation::wmem:
        instruction_wmem(o, instr);
        break;
    case Operation::call:
        instruction_call(o, instr);
        break;
    case Operation::cmp_eq:
        instruction_cmp_eq(o, instr);
        break;
    case Operation::cmp_lt:
        instruction_cmp_lt(o, instr);
        break;
    case Operation::cmp_le:
        instruction_cmp_le(o, instr);
        break;
    }
}