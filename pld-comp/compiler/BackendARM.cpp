#include "BackendARM.hpp"

BackendARM::~BackendARM()
{

}

void BackendARM::begin()
{
    o << "\t.section\t __TEXT,__text,regular,pure_instructions" << std::endl;
    o << "\t.build_version macos, 13, 3 sdk_version 13, 3" << std::endl;
    o << "\t.globl\t _main                           ; -- Begin function main" << std::endl;
    o << "\t.p2align\t  2";
    
}

void BackendARM::graph_begin(CFG* cfg)
{
    o << std::endl << "_" << cfg->get_name() << ":                                  ; @" << cfg->get_name() << std::endl;
    o << "\t.cfi_startproc" << std::endl;
    o << "; %bb.0:" << std::endl;
    o << "\tsub	sp, sp, #16" << std::endl;
    o << "\t.cfi_def_cfa_offset 16" << std::endl;
    o << "\tstr	wzr, [sp, #12]" << std::endl;
    o << "\tb .main_BB_0" << std::endl;
}

void BackendARM::graph_end(CFG* cfg)
{
    
    o << "\t# epilogue" << std::endl;
    o << "\tadd	sp, sp, #16" << std::endl;
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
    int index = bb->get_graph()->get_index(bb->test_var_name);
    o << "\tldr w" << index << ", [" << "sp, #" << index << "]" << std::endl;
    o << "\tcbnz w" << index << ", ." << bb->exit_true->get_name() << std::endl;
    o << "\tb ." << bb->exit_false->get_name() << std::endl;
}

void BackendARM::block_jump_simple(BasicBlock* bb)
{
    o << "\tb ." << bb->exit_true->get_name() << std::endl;
}

void BackendARM::instruction_ldconst(IRInstr* instr)
{
    o << "\tmov w" << instr->get_param(0) << ", #" << instr->get_param(1) << std::endl;
    // o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_copy(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_add(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tadd w" << instr->get_param(0) << ", w" << instr->get_param(0) << ", w" << instr->get_param(2) << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_sub(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tsub w" << instr->get_param(0) << ", w" << instr->get_param(0) << ", w" << instr->get_param(2) << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_mul(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tmul w" << instr->get_param(0) << ", w" << instr->get_param(0) << ", w" << instr->get_param(2) << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_div(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tsdiv w" << instr->get_param(0) << ", w" << instr->get_param(0) << ", w" << instr->get_param(2) << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_ret(IRInstr* instr)
{
    // o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_rmem(IRInstr* instr)
{
    o << "\tldr x" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tldr w" << instr->get_param(0) << ", [x" << instr->get_param(0) << "]" << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_wmem(IRInstr* instr)
{
    o << "\tldr x" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [x" << instr->get_param(0) << "]" << std::endl;
}

void BackendARM::instruction_call(IRInstr* instr)
{
    // TO-DO
    int nb_params = instr->get_params().size();

    for (int i = 0; i < nb_params; i++) {

        switch (i)
        {
            case 0:
                break;
            case 1:
                o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
                break;
            case 2:
                o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
                break;
            case 3:
                o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
                break;
            case 4:
                o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
                break;
            case 5:
                o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
                break;
            case 6:
                o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
                break;
        }
    }

}

void BackendARM::instruction_cmp_eq(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tcmp w" << instr->get_param(0) << ", w" << instr->get_param(2) << std::endl;
    o << "\tsete w" << instr->get_param(0) << std::endl;
    o << "\tmovz w" << instr->get_param(0) << ", #0" << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_cmp_lt(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tcmp w" << instr->get_param(0) << ", w" << instr->get_param(2) << std::endl;
    o << "\tsetl w" << instr->get_param(0) << std::endl;
    o << "\tmovz w" << instr->get_param(0) << ", #0" << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_cmp_lt_eq(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tcmp w" << instr->get_param(0) << ", w" << instr->get_param(2) << std::endl;
    o << "\tsetle w" << instr->get_param(0) << std::endl;
    o << "\tmovz w" << instr->get_param(0) << ", #0" << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_cmp_le(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tcmp w" << instr->get_param(0) << ", w" << instr->get_param(2) << std::endl;
    o << "\tsetg w" << instr->get_param(0) << std::endl;
    o << "\tmovz w" << instr->get_param(0) << ", #0" << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}

void BackendARM::instruction_cmp_le_eq(IRInstr* instr)
{
    o << "\tldr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
    o << "\tcmp w" << instr->get_param(0) << ", w" << instr->get_param(2) << std::endl;
    o << "\tsetge w" << instr->get_param(0) << std::endl;
    o << "\tmovz w" << instr->get_param(0) << ", #0" << std::endl;
    o << "\tstr w" << instr->get_param(0) << ", [sp, #" << instr->get_param(1) << "]" << std::endl;
}
