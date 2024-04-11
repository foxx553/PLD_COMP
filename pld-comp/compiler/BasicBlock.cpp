#include "BasicBlock.hpp"
#include "CFG.hpp"
#include "IRInstr.hpp"

/**
 * Constructeur
 */
BasicBlock::BasicBlock(CFG* cfg) : exit_true(nullptr), exit_false(nullptr), cfg(cfg), name(cfg->block_name()), test_symbol()
{
}

/**
 * Destructeur
 * Supprime toutes les instructions du BasicBlock
 */
BasicBlock::~BasicBlock()
{
    for(auto instr: instrs)
    {
        delete instr;
    }
}

/**
 * Renvoie l'etiquette du BasicBlock (name = etiquette)
 */
const std::string& BasicBlock::get_name() const
{
    return name;
}

/**
 * Renvoie le graphe CFG dans lequel est le BasicBlock
 */
const CFG* BasicBlock::get_graph() const
{
    return cfg;
}

/**
 * Renvoie le vector d'instructions du BasicBlock
 */
const std::vector<IRInstr*>& BasicBlock::get_instructions()
{
    return instrs;
}

/**
 * Permet d'ajouter une nouvelle instrcutions dans le BasicBlock
 * @param op : l'OPÉRATION (add, mul, div ...)
 * @param type : le type d'opération (INT64, INT32, ...)
 * @param params : la liste des paramètres de l'instruction
 */
void BasicBlock::add_instruction(Operation op, Type type, const std::vector<Symbol>& params)
{
    auto instruction = new IRInstr(this, op, type, params);
    instrs.push_back(instruction);
}

void BasicBlock::set_test(Symbol test_symbol)
{
    this->test_symbol = test_symbol;
}

Symbol BasicBlock::get_test() const
{
    return test_symbol;
}