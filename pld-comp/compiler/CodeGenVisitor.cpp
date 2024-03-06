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
    this->visitChildren(ctx);

    int source = inter.top();
    inter.pop();
    
    std::cout << "\tmovl -" << source << "(%rbp), %eax\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext *ctx)
{
    antlr4::tree::TerminalNode* identifier = ctx->assign_stmt() ? ctx->assign_stmt()->IDENTIFIER(): ctx->IDENTIFIER();
    std::string dest = identifier->getText();

    if (!symboles.count(dest))
    {
        stack += 4;
        int index = stack;
        symboles[dest] = stack;
    }

    if(ctx->assign_stmt())
    {
        this->visit(ctx->assign_stmt());
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) {

   std::string dest = ctx->IDENTIFIER()->getText();

    if (!symboles.count(dest))
    {
        std::cout << "Variable non existante dans la table de symboles. \n";
    }
   
    this->visitChildren(ctx);

    int source = inter.top();
    inter.pop();

    std::cout << "\tmovl -" << source << "(%rbp), %eax\n";
    std::cout << "\tmovl %eax, -" << symboles[dest] << "(%rbp)\n";

    return 0;
}

 antlrcpp::Any CodeGenVisitor::visitExprConstante(ifccParser::ExprConstanteContext *ctx) {
    int value = std::stoi(ctx->CONST()->getText());
    stack += 4;
    inter.push(stack);
    std::cout << "\tmovl $" << value << ", -" << stack << "(%rbp)\n";
    return visitChildren(ctx);
  }

antlrcpp::Any CodeGenVisitor::visitExprAddSub(ifccParser::ExprAddSubContext *ctx)   
{
    visitChildren(ctx);

    int droite = inter.top();
    inter.pop();
    int gauche = inter.top();
    inter.pop();

    stack += 4;
    inter.push(stack);

    std::cout << "\tmovl -" << gauche << "(%rbp), %eax\n";

    if(ctx->ADD()) {
        std::cout << "\taddl -" << droite << "(%rbp), %eax\n";
    }else{
        std::cout << "\tsubl -" << droite << "(%rbp), %eax\n";
    }

    std::cout << "\tmovl %eax, -" << stack << "(%rbp)\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprMultDiv(ifccParser::ExprMultDivContext *ctx) {
    visitChildren(ctx);

    int droite = inter.top();
    inter.pop();
    int gauche = inter.top();
    inter.pop();

    stack += 4;
    inter.push(stack);

    std::cout << "\tmovl -" << gauche << "(%rbp), %eax\n";

    if(ctx->MULT()) {
        std::cout << "\tmul -" << droite << "(%rbp)\n";
    }else {
        std::cout << "\tdiv -" << droite << "(%rbp)\n";
    }

    std::cout << "\tmovl %eax, -" << stack << "(%rbp)\n";

    return 0;
  }

antlrcpp::Any CodeGenVisitor::visitExprVariable(ifccParser::ExprVariableContext *ctx)
{
    std::string id = ctx->IDENTIFIER()->getText();

    if(symboles.count(id)) {
        inter.push(symboles[id]);
    }
    else
    {
        std::cerr << "ERREUR !!!!\n";
    }

    return 0;
}
