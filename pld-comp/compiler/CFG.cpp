#include "CFG.hpp"
#include "BasicBlock.hpp"

CFG::CFG(const std::string& name) : name(name), nextFreeSymbolIndex(0), nextBBnumber(0), nextTempIndex(0)
{
    begin_bb = new BasicBlock(this, name + new_BB_name());
    end_bb = new BasicBlock(this, name + new_BB_name());
    begin_bb->exit_true = end_bb;

    current_bb = begin_bb;

    blocks.push_back(begin_bb);
    blocks.push_back(end_bb);
}

CFG::~CFG()
{
    for(auto bb: blocks)
    {
        delete bb;
    }
}

const std::string& CFG::get_name() const
{
    return name;
}

const std::vector<BasicBlock*>& CFG::get_blocks() const
{
    return blocks;
}

const int CFG::get_index(const std::string& name) const
{
    if(SymbolType.count(name) == 0 && SymbolIndex.count(name) == 0)
    {

        std::cerr << "Error symbol does not present" << std::endl;
        return -1;
    }
    else
    {
        return SymbolIndex.at(name);
    }
}

const Type CFG::get_type(const std::string& name) const
{
    if(SymbolType.count(name) == 0 && SymbolIndex.count(name) == 0)
    {
        std::cout << "Error symbol does not present" << std::endl;
        // to do : throw an exception
    }
    else
    {
        return SymbolType.at(name);
    }

    return Type::INT_64;
}

void CFG::add_bb(BasicBlock* bb)
{
    this->blocks.push_back(bb);
}

std::string CFG::IR_reg_to_asm(std::string reg)
{
    // Todo : team asm :
    /**< helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24 */
    std::string formatted = "-" + reg + "(%rbp)";
    return formatted;
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
