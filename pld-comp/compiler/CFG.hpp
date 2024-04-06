#pragma once

#include "BasicBlock.hpp"
#include "IRInstr.hpp"
#include "Scope.hpp"

/** The class for the control flow graph, also includes the symbol table */

/* A few important comments:
     The entry block is the one with the same label as the AST function name.
       (it could be the first of bbs, or it could be defined by an attribute value)
     The exit block is the one with both exit pointers equal to nullptr.
     (again it could be identified in a more explicit way)

 */

typedef std::vector<CFG*> IR;

class CFG
{
public:
    CFG(const std::string& name);
    virtual ~CFG();

    const std::string&  get_name() const;
    int                 next_symbol_offset(Type type, int length = 1);
    int                 get_symbol_offset() const;
    const Symbol&       create_temp(Scope* scope, Type type);
    void                add_param(Symbol param);
    std::vector<Symbol> get_params();

    // basic block management
    const std::vector<BasicBlock*>& get_blocks() const;
    void                            add_block(BasicBlock* bb);
    std::string                     block_name();
    BasicBlock*                     current_bb;
    BasicBlock*                     begin_bb;
    BasicBlock*                     end_bb;

protected:
    std::vector<BasicBlock*> blocks;        ///< basic blocks
    std::string              name;          ///< name of the graph
    std::vector<Symbol>      params;        ///< params
    int                      block_offset;  ///< basic block offset : for name
    int                      symbol_offset; ///< symbol offset : for index
    int                      temp_offset;   ///< tmp offset : for name
};
