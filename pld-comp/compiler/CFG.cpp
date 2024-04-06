#include "CFG.hpp"
#include "BasicBlock.hpp"

CFG::CFG(const std::string& name, Type type) : name(name), type(type), block_offset(0), symbol_offset(0), temp_offset(0)
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

std::string CFG::block_name()
{
    return name + "_BB_" + std::to_string(block_offset++);
}

const Symbol& CFG::create_temp(Scope* scope, Type type)
{
    symbol_offset += Symbol::get_type_size(type);
    return scope->create_temp(symbol_offset, temp_offset++, type);
}

int CFG::next_symbol_offset(Type type, int length)
{
    symbol_offset += Symbol::get_type_size(type) * length;
    return symbol_offset;
}

int CFG::get_symbol_offset() const
{
    return symbol_offset;
}

void CFG::add_param(Symbol param)
{
    params.push_back(param);
}

std::vector<Symbol> CFG::get_params() const
{
    return params;
}

const Type CFG::get_type() const
{
    return type;
}

bool CFG::is_standard_function(const std::string& name, int& params)
{
    std::array<std::pair<std::string, int>, 2> functions = {std::make_pair("putchar", 1), std::make_pair("getchar", 0)};

    for(auto [label, count]: functions)
    {
        if(label == name)
        {
            params = count;
            return true;
        }
    }

    return false;
}