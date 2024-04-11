#pragma once

#include "IRInstr.hpp"


class BasicBlock
{
public:
    BasicBlock(CFG* cfg);
    virtual ~BasicBlock();

    const std::string& get_name() const;
    const CFG*         get_graph() const;

    const std::vector<IRInstr*> get_instructions() const;
    void                        add_instruction(Operation op, Type type, const std::vector<Symbol>& params);
    /*Peut faut il les passer en attribut prive ?*/


    BasicBlock* exit_true;              /**< pointer to the next basic block, true branch. If nullptr, return from procedure */
    BasicBlock* exit_false;             /**< pointer to the next basic block, false branch. If null_ptr, the basic block ends with an unconditional jump */

    std::vector<IRInstr*> instrs;       /** < the instructions themselves. */
    Symbol                test_symbol;  /** < when generating IR code for an if(expr) or while(expr) etc, store here the name of the variable that holds the value of expr */

private:
    std::string name;                   /**Nom ou étiquette du BasicBlock*/
    CFG*        cfg;                    /*Graphe CFG dans lequel est contenu le BasicBlock*/
};
