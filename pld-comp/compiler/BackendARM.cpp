#include "BackendARM.hpp"

BackendARM::~BackendARM()
{

}

void BackendARM::begin()
{
    o << ".text" << std::endl;
    o << ".global main" << std::endl;
}

void BackendARM::graph_begin(CFG* cfg)
{
    o << std::endl << cfg->get_name() << ':' << std::endl;

    o << "\t@ prologue" << std::endl;
    o << "\tpush {fp, lr}" << std::endl;
    o << "\tmov fp, sp" << std::endl;
}

void BackendARM::graph_end(CFG* cfg)
{
    o << "\t@ epilogue" << std::endl;
    o << "\tpop {fp, pc}" << std::endl;
}

void BackendARM::block_begin(BasicBlock* bb)
{
    o << "." << bb->get_name() << ":" << std::endl;
}

void BackendARM::block_jump_conditional(BasicBlock* bb)
{
    int index = bb->get_graph()->get_index(bb->test_var_name);
    o << "\tldr r1, [fp, #" << index << "]" << std::endl;
    o << "\tcmp r1, #0" << std::endl;
    o << "\tbeq ." << bb->exit_true->get_name() << std::endl;
    o << "\tb ." << bb->exit_false->get_name() << std::endl;
}

void BackendARM::block_jump_simple(BasicBlock* bb)
{
    o << "\tb ." << bb->exit_true->get_name() << std::endl;
}

void BackendARM::instruction_ldconst(IRInstr* instr)
{
   /* o << "\tmovw r0, #" << (instr->get_param(1) & 0xFFFF) << std::endl;
    o << "\tmovt r0, #" << (instr->get_param(1) >> 16) << std::endl;
    o << "\tstr r0, [fp, #-"<< instr->get_param(0) << "]" << std::endl;
    */
}

void BackendARM::instruction_copy(IRInstr* instr)
{
    o << "\tldr r1, [fp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tstr r1, [fp, #-"<< instr->get_param(0) << "]" << std::endl;
}

void BackendARM::instruction_add(IRInstr* instr)
{
    o << "\tldr r1, [fp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tldr r2, [fp, #" << instr->get_param(2) << "]" << std::endl;
    o << "\tadd r0, r1, r2" << std::endl;
    o << "\tstr r0, [fp, #-"<< instr->get_param(0) << "]" << std::endl;
}

void BackendARM::instruction_sub(IRInstr* instr)
{
    o << "\tldr r1, [fp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tldr r2, [fp, #" << instr->get_param(2) << "]" << std::endl;
    o << "\tsub r0, r1, r2" << std::endl;
    o << "\tstr r0, [fp, #-"<< instr->get_param(0) << "]" << std::endl;
}

void BackendARM::instruction_mul(IRInstr* instr)
{
    o << "\tldr r1, [fp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tldr r2, [fp, #" << instr->get_param(2) << "]" << std::endl;
    o << "\tmul r0, r1, r2" << std::endl;
    o << "\tstr r0, [fp, #-"<< instr->get_param(0) << "]" << std::endl;
}

void BackendARM::instruction_div(IRInstr* instr)
{
    // Not working now 

    o << "\tldr r1, [fp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tldr r2, [fp, #" << instr->get_param(2) << "]" << std::endl;
    o << "\tbl division_function" << std::endl;
    o << "\tstr r0, [fp, #-"<< instr->get_param(0) << "]" << std::endl;
}

void BackendARM::instruction_ret(IRInstr* instr)
{
    o << "\tldr r0, [fp, #" << instr->get_param(0) << "]" << std::endl;
}

void BackendARM::instruction_rmem(IRInstr* instr)
{
    // TO-DO
}

void BackendARM::instruction_wmem(IRInstr* instr)
{
    // TO-DO
}

void BackendARM::instruction_call(IRInstr* instr)
{
    // TO-DO
}

void BackendARM::instruction_cmp_eq(IRInstr* instr)
{
    // TO-DO
}

void BackendARM::instruction_cmp_lt(IRInstr* instr)
{
    // TO-DO
}

void BackendARM::instruction_cmp_le(IRInstr* instr)
{
    // TO-DO
}
