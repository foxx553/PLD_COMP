#include "Backend.hpp"

BackendASM::~BackendASM()
{

}

void BackendASM::begin(std::ostream& o)
{
    o << ".text" << std::endl;
    o << ".globl main" << std::endl;
}

void BackendASM::graph_begin(std::ostream& o, CFG* cfg)
{
    o << std::endl << cfg->get_name() << ':' << std::endl;

    o << "\t# prologue" << std::endl;
    o << "\tpushq %rbp" << std::endl;
    o << "\tmovq %rsp, %rbp" << std::endl;
}

void BackendASM::graph_end(std::ostream& o, CFG* cfg)
{
    o << "\t# epilogue" << std::endl;
    o << "\tpopq %rbp" << std::endl;
    o << "\tret" << std::endl;
}

void BackendASM::block_begin(std::ostream& o, BasicBlock* bb)
{
    o << "." << bb->get_name() << ":" << std::endl;
}

void BackendASM::block_jump_conditional(std::ostream& o, BasicBlock* bb)
{
    int index = bb->get_graph()->get_index(bb->test_var_name);
    o << "\tcmpl -" << "$0, -" << index << "(%rbp)" << std::endl;
    o << "\tje ." << bb->exit_true->get_name() << std::endl;
    o << "\tjmp ." << bb->exit_false->get_name() << std::endl;
}

void BackendASM::block_jump_simple(std::ostream& o, BasicBlock* bb)
{
    o << "\tjmp ." << bb->exit_true->get_name() << std::endl;
}

void BackendASM::instruction_ldconst(std::ostream& o, IRInstr* instr)
{
    o << "\tmovl $" << instr->get_param(1) << ", -" << instr->get_param(0)<< "(%rbp)" << std::endl;
}

void BackendASM::instruction_copy(std::ostream& o, IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_add(std::ostream& o, IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\taddl -" << instr->get_param(2) << "(%rbp), %eax" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_sub(std::ostream& o, IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tsubl -" << instr->get_param(2) << "(%rbp), %eax" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_mul(std::ostream& o, IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tmul -" << instr->get_param(2) << "(%rbp)" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_div(std::ostream& o, IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tmovl -" << instr->get_param(2) << "(%rbp), %ecx" << std::endl;
    o << "\tcltd\n\tidivl %ecx" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_ret(std::ostream& o, IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(0) << "(%rbp), %eax" << std::endl;
}

void BackendASM::instruction_rmem(std::ostream& o, IRInstr* instr)
{
    // TO-DO
}

void BackendASM::instruction_wmem(std::ostream& o, IRInstr* instr)
{
    // TO-DO
}

void BackendASM::instruction_call(std::ostream& o, IRInstr* instr)
{
    // TO-DO
}

void BackendASM::instruction_cmp_eq(std::ostream& o, IRInstr* instr)
{
    // TO-DO
}

void BackendASM::instruction_cmp_lt(std::ostream& o, IRInstr* instr)
{
    // TO-DO
}

void BackendASM::instruction_cmp_le(std::ostream& o, IRInstr* instr)
{
    // TO-DO
}
