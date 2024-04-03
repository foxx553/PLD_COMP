#include "BackendASM.hpp"

BackendASM::~BackendASM()
{
}

void BackendASM::begin()
{
    o << ".text" << std::endl;
    o << ".globl main" << std::endl;
}

void BackendASM::graph_begin(CFG* cfg)
{
    o << std::endl
      << cfg->get_name() << ':' << std::endl;

    o << "\t# prologue" << std::endl;
    o << "\tpushq %rbp" << std::endl;
    o << "\tmovq %rsp, %rbp" << std::endl;
}

void BackendASM::graph_end(CFG* cfg)
{
    o << "\t# epilogue" << std::endl;
    o << "\tpopq %rbp" << std::endl;
    o << "\tret" << std::endl;
}

void BackendASM::block_begin(BasicBlock* bb)
{
    o << "." << bb->get_name() << ":" << std::endl;
}

void BackendASM::block_jump_conditional(BasicBlock* bb)
{
    int index = bb->get_graph()->get_index(bb->test_var_name);
    o << "\tcmpl "
      << "$0, -" << index << "(%rbp)" << std::endl;
    o << "\tje ." << bb->exit_false->get_name() << std::endl;
    o << "\tjmp ." << bb->exit_true->get_name() << std::endl;
}

void BackendASM::block_jump_simple(BasicBlock* bb)
{
    o << "\tjmp ." << bb->exit_true->get_name() << std::endl;
}

void BackendASM::instruction_ldconst(IRInstr* instr)
{
    o << "\tmovl $" << instr->get_param(1) << ", -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_copy(IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_add(IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\taddl -" << instr->get_param(2) << "(%rbp), %eax" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_sub(IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tsubl -" << instr->get_param(2) << "(%rbp), %eax" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_mul(IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tmul -" << instr->get_param(2) << "(%rbp)" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_div(IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tmovl -" << instr->get_param(2) << "(%rbp), %ecx" << std::endl;
    o << "\tcltd\n\tidivl %ecx" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_ret(IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(0) << "(%rbp), %eax" << std::endl;
}

void BackendASM::instruction_rmem(IRInstr* instr)
{
    o << "\tmovq -" << instr->get_param(1) << "(%rbp), %rax" << std::endl;
    o << "\tmovl (%rax), %eax" << std::endl;
    o << "\t%eax, -" << instr->get_param(0) << ", (%rbp)" << std::endl;
}

void BackendASM::instruction_wmem(IRInstr* instr)
{

    o << "\tmovq -" << instr->get_param(1) << "(%rbp), %rax" << std::endl;
    o << "\tmovl" << instr->get_param(0) << "(%rbp), %edx" << std::endl;
    o << "\t%edx, (%rax)" << std::endl;
}

void BackendASM::instruction_call(IRInstr* instr)
{
    // TO-DO
    int nb_params = instr->get_params().size();

    for(int i = 0; i < nb_params; i++)
    {
        switch(i)
        {
        case 0:
            break;
        case 1:
            o << "\tmovl" << instr->get_param(0) << "(%rbp), %eax" << std::endl;
            o << "\tmovl"
              << "(%eax), %edi" << std::endl;
            break;
        case 2:
            o << "\tmovl" << instr->get_param(1) << "(%rbp), %esi" << std::endl;
            break;
        case 3:
            o << "\tmovl" << instr->get_param(2) << "(%rbp), %edx" << std::endl;
            break;
        case 4:
            o << "\tmovl" << instr->get_param(3) << "(%rbp), %ecx" << std::endl;
            break;
        case 5:
            o << "\tmovl" << instr->get_param(0) << "(%rbp), %r8d" << std::endl;
            break;
        case 6:
            o << "\tmovl" << instr->get_param(0) << "(%rbp), %r9d" << std::endl;
            break;
        }
    }

    o << "\tcall" << std::endl;
}

void BackendASM::instruction_cmp_eq(IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tcmpl -" << instr->get_param(2) << "(%rbp), %eax" << std::endl;
    o << "\tsete %al" << std::endl;
    o << "\tmovzbl %al, %eax" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}

void BackendASM::instruction_cmp_lt(IRInstr* instr)
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tcmpl -" << instr->get_param(2) << "(%rbp), %eax" << std::endl;
    o << "\tsetl %al" << std::endl;
    o << "\tmovzbl %al, %eax" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
}
