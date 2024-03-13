#include "CFG.hpp"

CFG::CFG(DefFonction* ast)
{
    
}

void CFG::add_bb(BasicBlock* bb)
{
    this->bbs.push_back(bb);
}

void CFG::gen_asm(std::ostream& o)
{
    // Todo : team asm

}

std::string CFG::IR_reg_to_asm(std::string reg)
{
    // Todo : team asm

}

void CFG::gen_asm_prologue(std::ostream& o)
{
    // Todo : team asm

}

void CFG::gen_asm_epilogue(std::ostream& o)
{
    // Todo : team asm

}

void CFG::add_to_symbol_table(std::string name, Type t)
{
    if(SymbolType.count(name) == 0 && SymbolIndex.count(name) == 0 ){
        
        SymbolType[name] = t;
        if(t == Type::INT_64){
            nextFreeSymbolIndex += 8;
            SymbolIndex[name] = nextFreeSymbolIndex;
        }
    } else {

        std::cout << "Error adding symbol already present" << std::endl;
    }
    
}

std::string CFG::create_new_tempvar(Type t)
{   
    add_to_symbol_table("tmp"+std::to_string(nextTempIndex++),t);
    
}

int CFG::get_var_index(std::string name)
{
    if(SymbolType.count(name) == 0 && SymbolIndex.count(name) == 0 ){

        std::cout << "Error symbol does not present" << std::endl;
        return -1;

    } else {

       return SymbolIndex[name];
    }

}

Type CFG::get_var_type(std::string name)
{
    if(SymbolType.count(name) == 0 && SymbolIndex.count(name) == 0 ){

        std::cout << "Error symbol does not present" << std::endl;
        
        // to do : throw an exception
        
            
    } else {

       return SymbolType[name];
    }

}

std::string CFG::new_BB_name()
{

    return "BB"+std::to_string(nextBBnumber++);
}
