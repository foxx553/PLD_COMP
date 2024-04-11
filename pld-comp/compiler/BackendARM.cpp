#include "BackendARM.hpp"

BackendARM::~BackendARM()
{
}

void BackendARM::begin()
{
    o << "\t.section\t __TEXT,__text,regular,pure_instructions" << std::endl;
    o << "\t.build_version macos, 13, 3 sdk_version 13, 3" << std::endl;
    o << "\t.globl\t _main ; -- Begin function main" << std::endl;
    o << "\t.p2align\t  2";
}

void BackendARM::graph_begin(CFG* cfg)
{
    o << std::endl
      << "_" << cfg->get_name() << ": ; @" << cfg->get_name() << std::endl;
    o << "\t.cfi_startproc" << std::endl;
    o << "; %bb.0:" << std::endl;
    o << "\tsub	sp, sp, #32" << std::endl;
    o << "\tstp	x29, x30, [sp, #504]" << std::endl;
    o << "\tadd	x29, sp, #504" << std::endl;
    o << "\tstr	w8, [sp, #4]" << std::endl;
    o << "\tstur    wzr, [x29, #-4]" << std::endl;

    o << "\tb .main_BB_0" << std::endl;
}

void BackendARM::graph_end(CFG* cfg)
{
    o << "\t; epilogue" << std::endl;
    o << "\tldp x29, x30, [sp, #504]" << std::endl;
    o << "\tadd	sp, sp, #32" << std::endl;
    o << "\tret" << std::endl;
    o << "\t.cfi_endproc" << std::endl;
    o << "\t\t\t\t\t\t\t\t\t\t; -- End function" << std::endl;
}

void BackendARM::block_begin(BasicBlock* bb)
{
    o << "." << bb->get_name() << ":" << std::endl;
}

void BackendARM::block_jump_conditional(BasicBlock* bb)
{
    o << "\tcbnz w8, ." << bb->exit_false->get_name() << std::endl;
    o << "\tb ." << bb->exit_true->get_name() << std::endl;
}

void BackendARM::block_jump_simple(BasicBlock* bb)
{
    o << "\tb ." << bb->exit_true->get_name() << std::endl;
}

void BackendARM::instruction_ldconst(IRInstr* instr)
{
    o << "\tmov w8, #" << symbol(instr->get_param(1)) << std::endl;
    o << "\tstr w8, [sp, #" << symbol(instr->get_param(0)) << "]" << std::endl;
}

void BackendARM::instruction_copy(IRInstr* instr)
{
    o << "\tldr w8, [sp, #" << symbol(instr->get_param(1)) << "]" << std::endl;
    o << "\tstr w8, [sp, #" << symbol(instr->get_param(0)) << "]" << std::endl;
}

void BackendARM::instruction_add(IRInstr* instr)
{
    o << "\tldr w8, [sp, #" << symbol(instr->get_param(1)) << "]" << std::endl;
    o << "\tldr w2, [sp, #" << symbol(instr->get_param(2)) << "]" << std::endl;
    o << "\tadd w8, w8, w2" << std::endl;
    o << "\tstr w8, [sp, #" << symbol(instr->get_param(0)) << "]" << std::endl;
}

void BackendARM::instruction_sub(IRInstr* instr)
{
    o << "\tldr w8, [sp, #" << symbol(instr->get_param(1)) << "]" << std::endl;
    o << "\tldr w2, [sp, #" << symbol(instr->get_param(2)) << "]" << std::endl;
    o << "\tsub w8, w8, w2" << std::endl;
    o << "\tstr w8, [sp, #" << symbol(instr->get_param(0)) << "]" << std::endl;
}

void BackendARM::instruction_mul(IRInstr* instr)
{
    o << "\tldr w8, [sp, #" << symbol(instr->get_param(1)) << "]" << std::endl;
    o << "\tldr w2, [sp, #" << symbol(instr->get_param(2)) << "]" << std::endl;
    o << "\tmul w8, w8, w2" << std::endl;
    o << "\tstr w8, [sp, #" << symbol(instr->get_param(0)) << "]" << std::endl;
}

void BackendARM::instruction_div(IRInstr* instr)
{
    o << "\tldr w8, [sp, #" << symbol(instr->get_param(1)) << "]" << std::endl;
    o << "\tldr w2, [sp, #" << symbol(instr->get_param(2)) << "]" << std::endl;
    o << "\tsdiv w8, w8, w2" << std::endl;
    o << "\tstr w8, [sp, #" << symbol(instr->get_param(0)) << "]" << std::endl;
}

void BackendARM::instruction_mod(IRInstr* instr)
{
    o << "\tldr w8, [sp, #" << symbol(instr->get_param(1)) << "]" << std::endl;
    o << "\tldr w2, [sp, #" << symbol(instr->get_param(2)) << "]" << std::endl;
    o << "\tsdiv w9, w8, w2" << std::endl;
    o << "\tmul w9, w9, w2" << std::endl;
    o << "\tsubs w8, w8, w9" << std::endl;
    o << "\tstr w8, [sp, #" << symbol(instr->get_param(0)) << "]" << std::endl;
}

void BackendARM::instruction_ret(IRInstr* instr)
{
    o << "\tldr w0, [sp, #" << symbol(instr->get_param(0)) << "]" << std::endl;
}

void BackendARM::instruction_rmem(IRInstr* instr)
{

    o << "\tadd x8, sp, #" << symbol(instr->get_param(0)) << std::endl;
}

void BackendARM::instruction_wmem(IRInstr* instr)
{
    o << "\tadd x8, sp, #" << symbol(instr->get_param(0)) << std::endl;
    o << "\tstr x8, [sp, #" << symbol(instr->get_param(1)) << "]" << std::endl;
}

void BackendARM::instruction_call(IRInstr* instr)
{
    int         nb_params = instr->get_params().size();
    std::string registres[] = {"w8", "w2", "w3", "w4"};

    for(int i = 1; i < nb_params && i < 4; i++)
    {
        o << "\tldr " << registres[i - 1] << ", [sp, #" << symbol(instr->get_param(i)) << "]" << std::endl;
    }
    o << "\tbl _" << (instr->get_param(0)).get_name() << std::endl;
}

void BackendARM::instruction_cmp_lt(IRInstr* instr)
{
    o << "\tldr w8, [sp, #" << symbol(instr->get_param(1)) << "]" << std::endl;
    o << "\tldr w9, [sp, #" << symbol(instr->get_param(2)) << "]" << std::endl;
    o << "\tsubs w8, w8, w9" << std::endl;
    o << "\tcset w8, ge" << std::endl;
}

void BackendARM::instruction_cmp_eq(IRInstr* instr)
{
    o << "\tldr w8, [sp, #" << symbol(instr->get_param(1)) << "]" << std::endl;
    o << "\tldr w9, [sp, #" << symbol(instr->get_param(2)) << "]" << std::endl;
    o << "\tsubs w8, w8, w9" << std::endl;
    o << "\tcset w8, ne" << std::endl;
}

std::string BackendARM::symbol(const Symbol& symbol)
{
    if(symbol.get_nature() == Symbol::Nature::CONSTANT)
    {
        return "#" + std::to_string(symbol.get_value());
    }
    else if(symbol.get_nature() == Symbol::Nature::REGISTER)
    {
        if(symbol.get_name() == "bp")
        {
            return "%rbp";
        }
        else if(symbol.get_offset() != -1) // function parameter
        {
            std::string params[] = {"w8", "w2", "w3", "w4", "w5", "w6"};
            return params[symbol.get_offset()];
        }
    }
    else if(symbol.get_nature() == Symbol::Nature::VARIABLE)
    {
        return "-" + std::to_string(symbol.get_offset()) + "(%rbp)";
    }

    return symbol.get_name();
}
