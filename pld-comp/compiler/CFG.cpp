#include "CFG.hpp"
#include "BasicBlock.hpp"

CFG::CFG(const std::string& name) : name(name), nextFreeSymbolIndex(0), nextBBnumber(0), nextTempIndex(0)
{
    begin_bb = new BasicBlock(this, name + new_BB_name());
    end_bb = new BasicBlock(this, name + new_BB_name());
    begin_bb->exit_true = end_bb;

    current_bb = begin_bb;

    bbs.push_back(begin_bb);
    bbs.push_back(end_bb);
}

CFG::~CFG()
{
    for(auto bb: bbs)
    {
        delete bb;
    }
}

void CFG::add_bb(BasicBlock* bb)
{
    this->bbs.push_back(bb);
}

void CFG::gen_asm(std::ostream& o)
{
    o << name << ':' << std::endl;

    gen_asm_prologue(o);

    for(int i = 0; i < bbs.size(); i++)
    {
        o << "." << bbs[i]->label << ":" << std::endl;
        bbs[i]->gen_asm(o);

        if(bbs[i]->exit_false && bbs[i]->exit_true)
        {
            // si 2 flèches de sortie : Test à faire pour décider du saut
            // Récupérer la variable du test: 2 solutions: derniere tmp: celle qui contient le résultat de la comparaison (marche pas tjrs)
            // test_var_name; puis récupérer avec map<std::string, int> SymbolIndex;

            int indexTest = SymbolIndex[bbs[i]->test_var_name];
            o << "\tcmpl -"
              << "$0, -" << indexTest << "(%rbp)" << std::endl;
            o << "\tje ." << bbs[i]->exit_true->label << std::endl;
            o << "\tjmp ." << bbs[i]->exit_false->label << std::endl;
        }
        else if(!(bbs[i]->exit_false) && bbs[i]->exit_true)
        {
            // si exit_false est nul, une seule sortie
            o << "\tjmp ." << bbs[i]->exit_true->label << std::endl;
        }
        else if(!(bbs[i]->exit_true))
        {
            gen_asm_epilogue(o);
        }
    }
}

std::string CFG::IR_reg_to_asm(std::string reg)
{
    // Todo : team asm :
    /**< helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24 */
    std::string formatted = "-" + reg + "(%rbp)";
    return formatted;
}

void CFG::gen_asm_prologue(std::ostream& o)
{
    o << "\t# prologue\n";
    o << "\tpushq %rbp \n";
    o << "\tmovq %rsp, %rbp \n";
}

void CFG::gen_asm_epilogue(std::ostream& o)
{
    o << "\t# epilogue\n";
    o << "\tpopq %rbp\n";
    o << "\tret\n";
}

void CFG::add_to_symbol_table(std::string name, Type t)
{
    if(SymbolType.count(name) == 0 && SymbolIndex.count(name) == 0)
    {

        SymbolType[name] = t;
        if(t == Type::INT_64)
        {
            nextFreeSymbolIndex += 8;
            SymbolIndex[name] = nextFreeSymbolIndex;
        }
    }
    else
    {

        std::cout << "Error adding symbol already present" << std::endl;
    }
}

std::string CFG::create_new_tempvar(Type t)
{
    auto name = "tmp" + std::to_string(nextTempIndex++);
    add_to_symbol_table(name, t);
    return name;
}

int CFG::get_var_index(std::string name)
{
    if(SymbolType.count(name) == 0 && SymbolIndex.count(name) == 0)
    {

        std::cerr << "Error symbol does not present" << std::endl;
        return -1;
    }
    else
    {

        return SymbolIndex[name];
    }
}

Type CFG::get_var_type(std::string name)
{
    if(SymbolType.count(name) == 0 && SymbolIndex.count(name) == 0)
    {
        std::cout << "Error symbol does not present" << std::endl;
        // to do : throw an exception
    }
    else
    {
        return SymbolType[name];
    }

    return Type::INT_64;
}

std::string CFG::new_BB_name()
{
    return "BB" + std::to_string(nextBBnumber++);
}
