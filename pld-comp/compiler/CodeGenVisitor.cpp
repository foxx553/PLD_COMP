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

    bool allVariablesUsed = true;
    for (auto it = symbolesUse.begin(); it != symbolesUse.end(); it++) {
        if (it -> second == false) {
            allVariablesUsed = false;
            break;
        }
    }

    if (!allVariablesUsed) {
        std::cout << "WARNING: Some declared variables were never initialized" << std::endl;
    }

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
    
    this->visitChildren(ctx);
    return 0;

}

antlrcpp::Any CodeGenVisitor::visitDeclaration(ifccParser::DeclarationContext *ctx) {
    
    antlr4::tree::TerminalNode* identifier = ctx->assignation() ? ctx->assignation()->IDENTIFIER(): ctx->IDENTIFIER();
    std::string dest = identifier->getText();

    if (!symboles.count(dest))
    {
        stack += 4;
        int index = stack;
        symboles[dest] = stack;
        symbolesUse[dest] = false;
    }

    if(ctx->assignation())
    {
        this->visit(ctx->assignation());
    }

    return 0;

}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) {

    this->visitChildren(ctx);
    return 0;

}

antlrcpp::Any CodeGenVisitor::visitAssignation(ifccParser::AssignationContext *ctx) {

    std::string dest = ctx->IDENTIFIER()->getText();

    if (!symboles.count(dest))
    {
        std::cout << "Variable non existante dans la table de symboles. \n";
    }
    else {
        symbolesUse[dest] = true;
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
        std::cout << "\tmovl -" << droite << "(%rbp), %ecx\n";
        std::cout << "\tcltd\n\tidivl %ecx\n";
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

antlrcpp::Any CodeGenVisitor::visitExprUnaire(ifccParser::ExprUnaireContext *ctx) {
    visitChildren(ctx);

    int term = inter.top();
    inter.pop();

    stack += 4;
    inter.push(stack);

    if(ctx->SUB()) {
        std::cout << "\tmovl $-1, %eax\n";
    }else {
        std::cout << "\tmovl $1, %eax\n";
    }
    
    std::cout << "\tmul -" << term << "(%rbp)\n";
    std::cout << "\tmovl %eax, -" << stack << "(%rbp)\n";

    return 0;
  }

  antlrcpp::Any CodeGenVisitor::visitCall_stmt(ifccParser::Call_stmtContext *ctx) {

    /* WORK IN PROGRESS
    this->visitChildren(ctx);
    */

    return 0;

  }


  antlrcpp::Any CodeGenVisitor::visitFunction_call(ifccParser::Function_callContext *ctx) {
    
    /* WORK IN PROGRESS
    this->visitChildren(ctx);

    std::string function_name = ctx->IDENTIFIER()->getText();

    for (int i = 0; i < ctx->expression().size(); i++) {
        
        int term = inter.top();
        inter.pop();

        std::cout << "\tmovl -" << term << "(%rbp), " << this->param_reg[i] << "\n";

    }

    std::cout << "\tcall " << function_name << "@PLT\n";
    std::cout << "\tmovl $0, %eax\n";
    std::cout << "\tleave\n";
    */

    return 0;

  }