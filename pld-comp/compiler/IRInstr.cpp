#include "IRInstr.hpp"

IRInstr::IRInstr(BasicBlock* bb_, Operation op, Type t, std::vector<std::string> params) 
{
    this->params=params;
    this->op=op;
    this->bb=bb_;
    this->t=t;
}

IRInstr::IRInstr(Operation op, Type t, std::vector<std::string> params) 
{
    this->params=params;
    this->op=op;
    this->t=t;
}

void IRInstr::gen_asm(std::ostream &o)
{
    switch (op) {
    case Operation::ldconst:
        gen_ldconst(o);
        break;
    case Operation::copy:
        gen_copy(o);
        break;
    case Operation::add:
        gen_add(o);
        break;
    case Operation::sub:
        gen_sub(o);
        break;
    case Operation::mul:
        gen_mul(o);
        break;
    case Operation::div:
        gen_div(o);
        break;
    case Operation::rmem:
        //pas encore à faire
        break;
    case Operation::wmem:
        //pas encore à faire
        break;
    case Operation::call:
        //pas encore faite
        break;
    case Operation::cmp_eq:
        //pas encore faite
        break;
    case Operation::cmp_lt:
        //pas encore faite
        break;
    case Operation::cmp_le:
        //pas encore faite
        break;

    }
}

void IRInstr::gen_ldconst(std::ostream &o)
{
    o<< "\tmovl $" << params[1] << ", -" << params[0] << "(%rbp)\n";
}

void IRInstr::gen_copy(std::ostream &o)
{
    o<< "\tmovl -" << params[1] << "(%rbp), %eax\n";
    o<< "\tmovl %eax, -" << params[0] << "(%rbp)\n";
}

void IRInstr::gen_add(std::ostream &o)
{
    o << "\tmovl -" << params[1] << "(%rbp), %eax\n";
    o << "\taddl -" << params[2] << "(%rbp), %eax\n";
    o << "\tmovl %eax, -" << params[0] << "(%rbp)\n";
}

void IRInstr::gen_sub(std::ostream &o)
{
    o << "\tmovl -" << params[1] << "(%rbp), %eax\n";
    o << "\tsubl -" << params[2] << "(%rbp), %eax\n";
    o << "\tmovl %eax, -" << params[0] << "(%rbp)\n";
}

void IRInstr::gen_mul(std::ostream &o)
{
    //mul var1 var2 var3 : var1 = var2*var3;
    o << "\tmovl -" << params[1] << "(%rbp), %eax\n";
    o << "\tmul -" << params[2] << "(%rbp)\n";
    o << "\tmovl %eax, -" << params[0] << "(%rbp)\n";
}

void IRInstr::gen_div(std::ostream &o)
{
    //div var1 var2 var3 : var1 = var2/var3;
    o << "\tmovl -" << params[1] << "(%rbp), %eax\n";
    o << "\tmovl -" << params[2] << "(%rbp), %ecx\n";
    o << "\tcltd\n\tidivl %ecx\n";
    o << "\tmovl %eax, -" << params[0] << "(%rbp)\n";
}