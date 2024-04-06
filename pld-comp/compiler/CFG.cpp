#include "CFG.hpp"
#include "BasicBlock.hpp"

CFG::CFG(const std::string& name) : name(name), symbol_offset(0), block_offset(0), temp_offset(0)
{
    begin_bb = new BasicBlock(this);
    end_bb = new BasicBlock(this);

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

void CFG::add_block(BasicBlock* bb)
{
    this->blocks.push_back(bb);
}

const Symbol& CFG::add_symbol(std::string name, Type type, int length)
{
    if(symbols.count(name) != 0)
    {
        throw std::invalid_argument("CFG::add_symbol: symbol already exists");
    }

    symbol_offset += Symbol::get_type_size(type) * length;
    symbols[name] = {name, type, symbol_offset, length};

    return symbols[name];
}

const Symbol& CFG::add_symbol(const Symbol& symbol)
{
    if(symbols.count(symbol.get_name()) != 0)
    {
        throw std::invalid_argument("CFG::add_symbol: symbol already exists");
    }

    symbols[symbol.get_name()] = symbol;
    return symbol;
}

const Symbol& CFG::create_temp(Type type)
{
    auto name = "tmp" + std::to_string(temp_offset++);
    return add_symbol(name, type);
}

const Symbol CFG::get_symbol(std::string name) const
{
    if(symbols.count(name) == 0)
    {
        throw std::invalid_argument("CFG::get_symbol: symbol doesn't exist");
    }

    return symbols.at(name);
}

std::string CFG::block_name()
{
    return name + "_BB_" + std::to_string(block_offset++);
}

int CFG::get_total_size()
{
    int total = 0;

    for(auto [name, symbol]: symbols)
    {
        total += symbol.get_size();
    }

    return total;
}