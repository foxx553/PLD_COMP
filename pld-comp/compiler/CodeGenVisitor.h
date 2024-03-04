#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

#include <map>

class CodeGenVisitor : public ifccBaseVisitor
{
public:
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
    virtual antlrcpp::Any visitAffect_stmt(ifccParser::Affect_stmtContext *ctx) override;
    virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext *ctx) override;
private:
    std::map<std::string, int> symboles;
    int stack = 0;
};
