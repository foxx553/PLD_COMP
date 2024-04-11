#include "IRInstr.hpp"

/**
 * Constructeur
 */

IRInstr::IRInstr(BasicBlock* block, Operation op, Type type, const std::vector<Symbol>& params) : block(block), op(op), type(type), params(params)
{
}

/**
 * Retourne le ième paramètre de l'instruction
 * Une instrcution contient un ensemble (ou vector) de paramètres (Symbol)
 * @param i le numero du paramètre que l'on veut retourner
 */
const Symbol& IRInstr::get_param(int i) const
{
    return params[i];
}

/**
 * Retourne l'opération(add,mul,mod...) fait par l'instruction
 */
const Operation IRInstr::get_operation() const
{
    return op;
}

/**
 * Retourne le vector de tous les paramètres
 */
const std::vector<Symbol>& IRInstr::get_params() const
{
    return params;
}

/* Retourne le type des opérandes */
const Type IRInstr::get_type() const
{
    return type;
}