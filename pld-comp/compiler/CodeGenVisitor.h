#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

#include <map>
#include "CFG.hpp"

class CodeGenVisitor : public ifccBaseVisitor
{
public:
    virtual ~CodeGenVisitor();

    void gen_asm(std::ostream& o);
    
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
    virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext *ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    virtual antlrcpp::Any visitDeclare_stmt(ifccParser::Declare_stmtContext *ctx) override;
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override ;
    virtual antlrcpp::Any visitAssignation(ifccParser::AssignationContext *ctx) override ;
    virtual antlrcpp::Any visitExprConstante(ifccParser::ExprConstanteContext *ctx) override;
    virtual antlrcpp::Any visitExprAddSub(ifccParser::ExprAddSubContext *ctx) override;
    virtual antlrcpp::Any visitExprVariable(ifccParser::ExprVariableContext *ctx) override;
    virtual antlrcpp::Any visitExprMultDiv(ifccParser::ExprMultDivContext *ctx) override ;
    virtual antlrcpp::Any visitExprUnaire(ifccParser::ExprUnaireContext *ctx) override ;
    virtual antlrcpp::Any visitCall_stmt(ifccParser::Call_stmtContext *ctx) override ;
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext *ctx) override ;
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext *ctx) override ;

private:
    std::map<std::string, int> symboles;
    std::map<std::string, bool> symbolesUse;
    int stack = 0;
    std::vector<std::string> param_reg = {"%edi", "%esi", "%edx", "%ecx", "%e8", "%e9"};

    std::stack<std::string> expressions;

    std::vector<CFG*> graphs;
};
