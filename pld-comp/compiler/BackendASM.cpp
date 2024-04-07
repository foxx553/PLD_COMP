#include "BackendASM.hpp"

BackendASM::~BackendASM()
{
}

void BackendASM::begin()
{
    std::vector<Symbol*> symbols;
    for(auto [name, symbol]: ir.global_scope->get_symbols())
    {
        symbols.push_back(&symbol);
    }

    o << "\t.text" << std::endl;

    if(symbols.size() == 0)
    {
        o << "\t.globl main" << std::endl;
    }
    else
    {
        o << "\t.bss" << std::endl;
        o << "\t.globl\t" << symbols[0]->get_name() << std::endl;
        o << "\t.type\t" << symbols[0]->get_name() << ", @object" << std::endl;

        for(int i = 0; i < symbols.size(); ++i)
        {
            bool last = i + 1 == symbols.size();
            o << symbols[i]->get_name() << ":" << std::endl;
            o << "\t.zero\t4" << std::endl;
            o << "\t.text" << std::endl;
            o << "\t.globl\t" << (last ? "main" : symbols[i + 1]->get_name()) << std::endl;
            o << "\t.type\t" << (last ? "main" : symbols[i + 1]->get_name()) << ", " << (last ? "@function" : "@object") << std::endl;
        }
    }
}

void BackendASM::graph_begin(CFG* cfg)
{
    o << std::endl
      << cfg->get_name() << ':' << std::endl;

    o << "\t# prologue" << std::endl;
    o << "\tpushq %rbp" << std::endl;
    o << "\tmovq %rsp, %rbp" << std::endl;

    int size = ((cfg->get_symbol_offset() - 1) | 15) + 1;
    o << "\tsubq $" << size << ", %rsp" << std::endl;
}

void BackendASM::graph_end(CFG* cfg)
{
    o << "\t# epilogue" << std::endl;
    o << "\tmovq %rbp, %rsp" << std::endl;
    o << "\tpopq %rbp" << std::endl;
    o << "\tret" << std::endl;
}

void BackendASM::block_begin(BasicBlock* bb)
{
    o << "." << bb->get_name() << ":" << std::endl;
}

void BackendASM::block_jump_conditional(BasicBlock* bb)
{
    o << "\tcmpl "
      << "$0, -" << bb->test_symbol.get_offset() << "(%rbp)" << std::endl;
    o << "\tje ." << bb->exit_false->get_name() << std::endl;
    o << "\tjmp ." << bb->exit_true->get_name() << std::endl;
}

void BackendASM::block_jump_simple(BasicBlock* bb)
{
    o << "\tjmp ." << bb->exit_true->get_name() << std::endl;
}

void BackendASM::instruction_ldconst(IRInstr* instr)
{
    o << "\tmovq " << symbol(instr->get_param(1)) << ", " << symbol(instr->get_param(0)) << std::endl;
}

void BackendASM::instruction_copy(IRInstr* instr)
{
    o << "\tmovq " << symbol(instr->get_param(1)) << ", %rax" << std::endl;
    o << "\tmovq %rax, " << symbol(instr->get_param(0)) << std::endl;
}

void BackendASM::instruction_add(IRInstr* instr)
{
    o << "\tmovq " << symbol(instr->get_param(1)) << ", %rax" << std::endl;
    o << "\taddq " << symbol(instr->get_param(2)) << ", %rax" << std::endl;
    o << "\tmovq %rax, " << symbol(instr->get_param(0)) << std::endl;
}

void BackendASM::instruction_sub(IRInstr* instr)
{
    o << "\tmovq " << symbol(instr->get_param(1)) << ", %rax" << std::endl;
    o << "\tsubq " << symbol(instr->get_param(2)) << ", %rax" << std::endl;
    o << "\tmovq %rax, " << symbol(instr->get_param(0)) << std::endl;
}

void BackendASM::instruction_mul(IRInstr* instr)
{
    o << "\tmovq " << symbol(instr->get_param(1)) << ", %rax" << std::endl;
    o << "\tmulq " << symbol(instr->get_param(2)) << std::endl;
    o << "\tmovq %rax, " << symbol(instr->get_param(0)) << std::endl;
}

void BackendASM::instruction_div(IRInstr* instr)
{
    o << "\tmovl " << symbol(instr->get_param(1)) << ", %eax" << std::endl;
    o << "\tmovl " << symbol(instr->get_param(2)) << ", %ecx" << std::endl;
    o << "\tcltd\n\tidivl %ecx" << std::endl;
    o << "\tmovl %eax, " << symbol(instr->get_param(0)) << std::endl;
}

void BackendASM::instruction_mod(IRInstr* instr)
{
    o << "\tmovl " << symbol(instr->get_param(1)) << ", %eax" << std::endl;
    o << "\tmovl " << symbol(instr->get_param(2)) << ", %ecx" << std::endl;
    o << "\tcltd\n\tidivl %ecx" << std::endl;
    o << "\tmovl %edx, " << symbol(instr->get_param(0)) << std::endl;
}

void BackendASM::instruction_ret(IRInstr* instr)
{
    o << "\tmovq " << symbol(instr->get_param(0)) << ", %rax" << std::endl;
}

void BackendASM::instruction_rmem(IRInstr* instr)
{
    o << "\tmovq " << symbol(instr->get_param(1)) << ", %rax" << std::endl;
    o << "\tmovq (%rax), %r10" << std::endl;
    o << "\tmovq %r10, " << symbol(instr->get_param(0)) << std::endl;
}

void BackendASM::instruction_wmem(IRInstr* instr)
{
    o << "\tmovq " << symbol(instr->get_param(0)) << ", %rax" << std::endl;
    o << "\tmovq " << symbol(instr->get_param(1)) << ", %r10" << std::endl;
    o << "\tmovq %r10, (%rax)" << std::endl;
}

void BackendASM::instruction_call(IRInstr* instr)
{
    std::string registres[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

    for(int i = 2; i < instr->get_params().size(); i++)
    {
        o << "\tmovq " << symbol(instr->get_param(i)) << ", %" << registres[i - 2] << std::endl;
    }

    o << "\tcall " << instr->get_param(0).get_name() << std::endl;
    o << "\tmovq %rax, " << symbol(instr->get_param(1)) << std::endl;
}

void BackendASM::instruction_cmp_eq(IRInstr* instr)
{
    o << "\tmovl " << symbol(instr->get_param(1)) << ", %eax" << std::endl;
    o << "\tcmpl " << symbol(instr->get_param(2)) << ", %eax" << std::endl;
    o << "\tsete %al" << std::endl;
    o << "\tmovzbl %al, %eax" << std::endl;
    o << "\tmovl %eax, " << symbol(instr->get_param(0)) << std::endl;
}

void BackendASM::instruction_cmp_lt(IRInstr* instr)
{
    o << "\tmovl " << symbol(instr->get_param(1)) << ", %eax" << std::endl;
    o << "\tcmpl " << symbol(instr->get_param(2)) << ", %eax" << std::endl;
    o << "\tsetl %al" << std::endl;
    o << "\tmovzbl %al, %eax" << std::endl;
    o << "\tmovl %eax, " << symbol(instr->get_param(0)) << std::endl;
}

std::string BackendASM::symbol(const Symbol& symbol)
{
    if(symbol.get_nature() == Symbol::Nature::CONSTANT)
    {
        return "$" + std::to_string(symbol.get_value());
    }
    else if(symbol.get_nature() == Symbol::Nature::REGISTER)
    {
        if(symbol.get_name() == "bp")
        {
            return "%rbp";
        }
        else if(symbol.get_name() == "param") // function parameter
        {
            std::string params[] = {"rdi", "rsi", "rdx", "rcx", "r8d", "r9d"};
            return "%" + params[symbol.get_offset()];
        }
    }
    else if(symbol.get_nature() == Symbol::Nature::VARIABLE)
    {
        return "-" + std::to_string(symbol.get_offset()) + "(%rbp)";
    }
    else if(symbol.get_nature() == Symbol::Nature::GLOBAL)
    {
        return symbol.get_name() + "(%rip)";
    }

    return symbol.get_name();
}