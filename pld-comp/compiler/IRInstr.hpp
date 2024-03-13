#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

class BasicBlock;
class CFG;
class DefFonction;

enum class Type 
{
    INT_64,
    INT_32,
    INT_16
};

//! The class for one 3-address instruction
class IRInstr {
 
   public:
	/** The instructions themselves -- feel free to subclass instead */
	enum class Operation 
    {
		ldconst,    // a = 5
		copy,       // a = b
		add,        // x + y
		sub,        // x - y
		mul,        // x * y
		div,        // x / y
        rmem,       // a = *b
        wmem,       // *b = a
		call,       // fct()
		cmp_eq,     // a == b
		cmp_lt,     // a < b
		cmp_le      // a <= b
	};

	/**  constructor */
	IRInstr(BasicBlock* bb_, Operation op, Type t, std::vector<std::string> params);
	
	/** Actual code generation */
	void gen_asm(std::ostream &o); /**< x86 assembly code generation for this IR instruction */
	
 private:
	BasicBlock* bb; /**< The BB this instruction belongs to, which provides a pointer to the CFG this instruction belong to */
	Operation op;
	Type t;
	std::vector<std::string> params; /**< For 3-op instrs: d, x, y; for ldconst: d, c;  For call: label, d, params;  for wmem and rmem: choose yourself */
	// if you subclass IRInstr, each IRInstr subclass has its parameters and the previous (very important) comment becomes useless: it would be a better design. 
};