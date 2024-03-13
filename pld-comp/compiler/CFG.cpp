#include "CFG.hpp"

CFG::CFG(DefFonction* ast)
{
    
}

void CFG::add_bb(BasicBlock* bb)
{

}

void CFG::gen_asm(std::ostream& o)
{

}

std::string CFG::IR_reg_to_asm(std::string reg)
{

}

void CFG::gen_asm_prologue(std::ostream& o)
{

}

void CFG::gen_asm_epilogue(std::ostream& o)
{

}

void CFG::add_to_symbol_table(std::string name, Type t)
{

}

std::string CFG::create_new_tempvar(Type t)
{

}

int CFG::get_var_index(std::string name)
{

}

Type CFG::get_var_type(std::string name)
{

}

std::string CFG::new_BB_name()
{

}
