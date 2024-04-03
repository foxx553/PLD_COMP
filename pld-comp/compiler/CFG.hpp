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

    const std::string&              get_name() const;
    const std::vector<BasicBlock*>& get_blocks() const;
    const Type                      get_type(const std::string& name) const;
    const int                       get_index(const std::string& name) const;

    void        add_bb(BasicBlock* bb);
    std::string IR_reg_to_asm(std::string reg); /**< helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24 */

    // symbol table methods
    void                         add_to_symbol_table(std::string name, Type t);
    std::tuple<std::string, int> create_new_tempvar(Type t);
    int                          get_var_index(std::string name);
    Type                         get_var_type(std::string name);

    // basic block management
    std::string new_BB_name();
    BasicBlock* current_bb;
    BasicBlock* begin_bb;
    BasicBlock* end_bb;

protected:
    std::map<std::string, Type> SymbolType;          /**< part of the symbol table  */
    std::map<std::string, int>  SymbolIndex;         /**< part of the symbol table  */
    int                         nextFreeSymbolIndex; /**< to allocate new symbols in the symbol table */
    int                         nextBBnumber;        /**< just for naming */
    int                         nextTempIndex;

    std::vector<BasicBlock*> blocks; /**< all the basic blocks of this CFG*/
    std::string              name;
};
