#include "Backend.hpp"

/**
 * Constructeur
 */

Backend::Backend(IR& ir, std::ostream& o) : ir(ir), o(o)
{
}

/**
 * Destructeur
 */

Backend::~Backend()
{
}

void Backend::generate()
{
    begin();

    for(auto cfg: ir.graphs)
    {
        graph(cfg);
    }
}


/**
 * Parcours le graphe CFG, à chaque BasciBlock, fait un appel à la méthode Backend::block, pour traduire les instructions en asssembleur
 * @param cfg le graphe à traduire en assembleur
 */

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


/**
 * Permet l'ecriture en code assembleur de chaque instructions d'un BasicBlock
 * @param bb le BasicBlock qui contient les instructions à traduire en assembleur
 */

void Backend::block(BasicBlock* bb)
{
    block_begin(bb);

    for(const auto& instr: bb->instrs)
    {
        instruction(instr);
    }
}


/**
 * Permet l'ecriture d'une instruction en code assembleur
 * @param instr l'instructions à traduire en assembleur
 * La classe IRInstr contient un attribut Operation
 * C'est sur la base du type d'opération que la traduction en assembleur se fait
 */

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
    case Operation::mod:
        instruction_mod(instr);
        break;
    case Operation::ret:
        instruction_ret(instr);
        break;
    case Operation::bxor:
        instruction_bxor(instr);
        break;
    case Operation::bor:
        instruction_bor(instr);
        break;
    case Operation::band:
        instruction_band(instr);
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
    }
}