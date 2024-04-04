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

void BackendASM::instruction_copy(IRInstr* instr) //assignation
{
    o << "\tmovl -" << instr->get_param(1) << "(%rbp), %eax" << std::endl;
    o << "\tmovl %eax, -" << instr->get_param(0) << "(%rbp)" << std::endl;
    o << "movq $-" << instr->get_param(0) 
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
    std::string registres[] = {"edi","esi","edx","ecx","r8d","r9d"};

    for(int i = 1; i < nb_params; i++)
    {
        o << "\tmovl" << instr->get_param(i) << "(%rbp), %" << registres[i-1] << std::endl;
    }
    o << "\tcall" << instr->get_param(0) <<std::endl;
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
