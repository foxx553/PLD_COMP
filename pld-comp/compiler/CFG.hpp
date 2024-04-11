#pragma once

#include "BasicBlock.hpp"
#include "IRInstr.hpp"
#include "Scope.hpp"

/**
 * Une IR est un ensemble de CFG et un scope global
 */
struct IR
{
    std::vector<CFG*> graphs;
    Scope*            global_scope;
};

/**
 * CFG : Control Flow Graph
 * Un CFG est un ensemble de BasicBlock
 * Représente une fonction
 */
class CFG
{
public:
    CFG(const std::string& name, Type type);
    virtual ~CFG();

    const std::string&  get_name() const;                                           ///< get the name of the CFG
    int                 next_symbol_offset(Type type, int length = 1);              ///< add the size of a symbol to the symbol offset
    int                 get_symbol_offset() const;                                  ///< get the symbol offset
    const Symbol&       create_temp(Scope* scope, Type type, bool pointer = false); ///< create a temporary symbol
    const Symbol&       create_temp(Scope* scope, const Symbol& symbol);            ///< create a temporary symbol
    void                add_param(Symbol param);                                    ///< add a param to the CFG
    std::vector<Symbol> get_params() const;                                         ///< get the params of the CFG
    const Type          get_type() const;                                           ///< get the return type of the CFG
    static bool         is_standard_function(const std::string& name, int& params); ///< check if a function is a standard function

    /* Gestions des BasicBlock */
    const std::vector<BasicBlock*>& get_blocks() const;        ///< get the basic blocks
    void                            add_block(BasicBlock* bb); ///< add a basic block
    std::string                     block_name();              ///< generate a new block name
    BasicBlock*                     current_bb;                ///< current basic block
    BasicBlock*                     begin_bb;                  ///< begin basic block
    BasicBlock*                     end_bb;                    ///< end basic block

protected:
    std::vector<BasicBlock*> blocks;        ///< basic blocks
    std::string              name;          ///< name of the graph
    std::vector<Symbol>      params;        ///< params
    int                      block_offset;  ///< basic block offset : for name
    int                      symbol_offset; ///< symbol offset : for index
    int                      temp_offset;   ///< tmp offset : for name
    Type                     type;          ///< return type
};
