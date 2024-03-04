#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
    std::cout << ".globl main\n";
    std::cout << "\tmain: \n";

    std::cout << "\n\t# prologue\n";
    std::cout << "\tpushq %rbp \n";
    std::cout << "\tmovq %rsp, %rbp \n";

    std::cout << "\n\t# body\n";

    this->visit(ctx->instruction());

    std::cout << "\n\t# epilogue\n";
    std::cout << "\tpopq %rbp\n";

    std::cout << "\tret\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    int retval = stoi(ctx->CONST()->getText());

    std::cout << "\tmovl $" << retval << ", %eax\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffect_stmt(ifccParser::Affect_stmtContext *ctx)
{
    std::string dest = ctx->IDENTIFIER()[0]->getText();

    if (!symboles.count(dest))
    {
        stack += 4;
        int index = stack;
        symboles[dest] = stack;
    }

    if (ctx->CONST())
    {
        int value = std::stoi(ctx->CONST()->getText());
        std::cout << "\tmovl $" << value << ", -" << symboles[dest] << "(%rbp)\n";
    }
    else if (ctx->IDENTIFIER()[1])
    {
        std::string source = ctx->IDENTIFIER()[1]->getText();

        if (symboles.count(source))
        {
            std::cout << "\tmovl -" << symboles[source] << "(%rbp), %eax\n";
            std::cout << "\tmovl %eax, -" << symboles[dest] << "(%rbp)\n";
        }
        else
        {
            std::cerr << "ERREUR !!!\n";
        }
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitInstruction(ifccParser::InstructionContext *ctx)
{
    this->visitChildren(ctx);
    return 0;
}