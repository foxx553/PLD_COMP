#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

#include <map>

class CodeGenVisitor : public ifccBaseVisitor
{
public:
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext *ctx) override;
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override ;
    virtual antlrcpp::Any visitExprConstante(ifccParser::ExprConstanteContext *ctx) override;
    virtual antlrcpp::Any visitExprAddSub(ifccParser::ExprAddSubContext *ctx) override;
    virtual antlrcpp::Any visitExprVariable(ifccParser::ExprVariableContext *ctx) override;
    virtual antlrcpp::Any visitExprMultDiv(ifccParser::ExprMultDivContext *ctx) override ;
    virtual antlrcpp::Any visitExprUnaire(ifccParser::ExprUnaireContext *ctx) override ;

private:
    std::map<std::string, int> symboles;
    int stack = 0;
    std::stack<int> inter;
};
