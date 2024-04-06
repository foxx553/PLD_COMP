#pragma once

#include "BasicBlock.hpp"
#include "IRInstr.hpp"

#include <tuple>

typedef std::vector<CFG*> IR;

/** The class for the control flow graph, also includes the symbol table */

/* A few important comments:
     The entry block is the one with the same label as the AST function name.
       (it could be the first of bbs, or it could be defined by an attribute value)
     The exit block is the one with both exit pointers equal to nullptr.
     (again it could be identified in a more explicit way)

 */
class CFG
{
public:
    CFG(const std::string& name);
    virtual ~CFG();

    const std::string& get_name() const;

    // symbol table methods
    const Symbol& add_symbol(std::string name, Type t, int length = 1);
    const Symbol& add_symbol(const Symbol& symbol);
    const Symbol& create_temp(Type type);
    const Symbol  get_symbol(std::string name) const;
    int           get_total_size();

    // basic block management
    const std::vector<BasicBlock*>& get_blocks() const;
    void                            add_block(BasicBlock* bb);
    std::string                     block_name();
    BasicBlock*                     current_bb;
    BasicBlock*                     begin_bb;
    BasicBlock*                     end_bb;

protected:
    std::vector<BasicBlock*>      blocks;        ///< basic blocks
    std::map<std::string, Symbol> symbols;       ///< symbol table
    std::string                   name;          ///< name of the graph
    int                           symbol_offset; ///< symbol offset : for index
    int                           block_offset;  ///< basic block offset : for name
    int                           temp_offset;   ///< tmp offset : for name
};
