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

struct IR
{
    std::vector<CFG*> graphs;
    Scope*            global_scope;
};

class CFG
{
public:
    CFG(const std::string& name, Type type);
    virtual ~CFG();

    const std::string&  get_name() const;
    int                 next_symbol_offset(Type type, int length = 1);
    int                 get_symbol_offset() const;
    const Symbol&       create_temp(Scope* scope, Type type, bool pointer = false);
    const Symbol&       create_temp(Scope* scope, const Symbol& symbol);
    void                add_param(Symbol param);
    std::vector<Symbol> get_params() const;
    const Type          get_type() const;

    static bool is_standard_function(const std::string& name, int& params);

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
    Type                     type;          ///< return type
};
