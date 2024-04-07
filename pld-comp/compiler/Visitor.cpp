#include "Visitor.hpp"

#include <iostream>

Visitor::Visitor() : current_scope(nullptr)
{
}

IR Visitor::get_ir()
{
    return {graphs, scopes.front()};
}

Visitor::~Visitor()
{
    for(auto graph: graphs)
    {
        delete graph;
    }

    for(auto scope: scopes)
    {
        delete scope;
    }
}

antlrcpp::Any Visitor::visitProg(ifccParser::ProgContext* ctx)
{
    open_scope();
    this->visitChildren(ctx);
    close_scope();

    return 0;
}

antlrcpp::Any Visitor::visitFunction_global(ifccParser::Function_globalContext* ctx)
{
    auto type = Symbol::type_from_string(ctx->return_type()->getText());
    graphs.push_back(new CFG(ctx->IDENTIFIER()->getText(), type));

    open_scope();

    auto  graph = graphs.back();
    auto* block = graph->current_bb;

    for(int i = 0; i < ctx->declaration().size(); ++i)
    {
        auto declaration = ctx->declaration()[i];

        auto type = Type::INT_64;
        auto length = declaration->array_length() ? reduce_constant(declaration->array_length()->constant()).get_value() : 1;
        auto offset = graph->next_symbol_offset(type, length);

        const auto& symbol = current_scope->add_symbol(offset, declaration->IDENTIFIER()->getText(), type, length);

        if(i < 6)
        {
            block->add_instruction(Operation::copy, Type::INT_64, {symbol, {"param", Symbol::Nature::REGISTER, i}});
        }

        graph->add_param(symbol);
    }

    this->visit(ctx->block());

    graph->current_bb->exit_true = graph->end_bb;

    close_scope();

    return 0;
}

antlrcpp::Any Visitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx)
{
    this->visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto dest = pop_symbol();

    block->add_instruction(Operation::ret, Type::INT_64, {dest});

    // exit
    block->exit_true = graph->end_bb;

    auto garbage = new BasicBlock(graph);
    graph->add_block(garbage);
    graph->current_bb = garbage;

    return 0;
}

antlrcpp::Any Visitor::visitDeclaration_global(ifccParser::Declaration_globalContext* ctx)
{
    auto type = Symbol::type_from_string(ctx->TYPE()->getText());

    if(type == Type::VOID)
    {
        throw std::invalid_argument("Visitor::visitDeclaration_global: can't have void variable");
    }

    for(auto declaration: ctx->declaration())
    {
        auto length = declaration->array_length() ? reduce_constant(declaration->array_length()->constant()).get_value() : 1;
        current_scope->add_symbol({declaration->IDENTIFIER()->getText(), type, 0, length, Symbol::Nature::GLOBAL});
    }

    return 0;
}

antlrcpp::Any Visitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext* ctx)
{
    auto type = Symbol::type_from_string(ctx->TYPE()->getText());

    if(type == Type::VOID)
    {
        throw std::invalid_argument("Visitor::visitDeclaration_stmt: can't have void variable");
    }

    for(auto declaration: ctx->declaration())
    {
        auto* graph = graphs.back();
        auto* block = graph->current_bb;

        auto length = declaration->array_length() ? reduce_constant(declaration->array_length()->constant()).get_value() : 1;
        auto offset = graph->next_symbol_offset(type, length);

        const auto& symbol = current_scope->add_symbol(offset, declaration->IDENTIFIER()->getText(), type, length);

        if(declaration->expression())
        {
            this->visit(declaration->expression());
            auto expression = pop_symbol();

            block = graph->current_bb; // get out block after visiting
            block->add_instruction(Operation::copy, Type::INT_64, {symbol, expression});
        }
    }

    return 0;
}

antlrcpp::Any Visitor::visitLvalue(ifccParser::LvalueContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto identifier = current_scope->get_symbol(ctx->IDENTIFIER()->getText());

    if(identifier.get_nature() == Symbol::Nature::GLOBAL)
    {
        symbols.push(identifier);
        return 0;
    }

    auto dest = graph->create_temp(current_scope, Type::INT_64);
    block->add_instruction(Operation::ldconst, Type::INT_64, {dest, {-identifier.get_offset()}});
    block->add_instruction(Operation::add, Type::INT_64, {dest, {"bp", Symbol::Nature::REGISTER}, dest});

    if(ctx->array_index())
    {
        this->visit(ctx->array_index());
        auto index = pop_symbol();

        block = graph->current_bb; // get out block after visiting
        block->add_instruction(Operation::add, Type::INT_64, {dest, index, dest});
    }

    symbols.push(dest);

    return 0;
}

antlrcpp::Any Visitor::visitBlock_stmt(ifccParser::Block_stmtContext* ctx)
{
    open_scope();
    this->visitChildren(ctx);
    close_scope();

    return 0;
}

antlrcpp::Any Visitor::visitAssignment(ifccParser::AssignmentContext* ctx)
{
    this->visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto right = pop_symbol();
    auto left = pop_symbol();

    if(left.get_nature() == Symbol::Nature::GLOBAL)
    {
        block->add_instruction(Operation::copy, Type::INT_64, {left, right});
        return 0;
    }

    block->add_instruction(Operation::wmem, Type::INT_64, {left, right});

    symbols.push(left);

    return 0;
}

antlrcpp::Any Visitor::visitExprAssignment(ifccParser::ExprAssignmentContext* ctx)
{
    this->visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto address = pop_symbol();

    auto dest = graph->create_temp(current_scope, Type::INT_64);
    block->add_instruction(Operation::rmem, Type::INT_64, {dest, address});

    symbols.push(dest);

    return 0;
}

antlrcpp::Any Visitor::visitExprConstante(ifccParser::ExprConstanteContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto dest = graph->create_temp(current_scope, Type::INT_64);

    block->add_instruction(Operation::ldconst, Type::INT_64, {dest, reduce_constant(ctx->constant())});

    symbols.push(dest);

    return 0;
}

antlrcpp::Any Visitor::visitExprLvalue(ifccParser::ExprLvalueContext* ctx)
{
    visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto value = pop_symbol();

    if(value.get_nature() == Symbol::Nature::GLOBAL)
    {
        symbols.push(value);
        return 0;
    }

    auto dest = graph->create_temp(current_scope, Type::INT_64);
    block->add_instruction(Operation::rmem, Type::INT_64, {dest, value});
    symbols.push(dest);

    return 0;
}

antlrcpp::Any Visitor::visitExprAddSub(ifccParser::ExprAddSubContext* ctx)
{
    visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto droite = pop_symbol();
    auto gauche = pop_symbol();

    auto dest = graph->create_temp(current_scope, Type::INT_64);

    block->add_instruction(ctx->ADD() ? Operation::add : Operation::sub, Type::INT_64, {dest, gauche, droite});

    symbols.push(dest);

    return 0;
}

antlrcpp::Any Visitor::visitExprProduit(ifccParser::ExprProduitContext* ctx)
{
    visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto droite = pop_symbol();
    auto gauche = pop_symbol();

    auto dest = graph->create_temp(current_scope, Type::INT_64);

    auto operation = ctx->MUL() ? Operation::mul : (ctx->DIV() ? Operation::div : Operation::mod);
    block->add_instruction(operation, Type::INT_64, {dest, gauche, droite});

    symbols.push(dest);

    return 0;
}

antlrcpp::Any Visitor::visitExprUnaire(ifccParser::ExprUnaireContext* ctx)
{
    visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto facteur = graph->create_temp(current_scope, Type::INT_64);
    block->add_instruction(Operation::ldconst, Type::INT_64, {facteur, {ctx->ADD() ? 1 : -1}});

    auto source = pop_symbol();

    auto dest = graph->create_temp(current_scope, Type::INT_64);
    block->add_instruction(Operation::mul, Type::INT_64, {dest, source, facteur});

    symbols.push(dest);

    return 0;
}

antlrcpp::Any Visitor::visitExprFunction(ifccParser::ExprFunctionContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;
    auto  name = ctx->function_call()->IDENTIFIER()->getText();

    auto target = find_graph(name);

    int target_params;
    if(!CFG::is_standard_function(name, target_params))
    {
        if(!target)
        {
            throw std::invalid_argument("Visitor::visitExprFunction: target function " + name + " doesn't exist");
        }

        if(target->get_type() == Type::VOID)
        {
            throw std::invalid_argument("Visitor::visitExprFunction: target function " + name + " returns void");
        }
    }

    this->visitChildren(ctx);

    return 0;
}

antlrcpp::Any Visitor::visitFunction_call(ifccParser::Function_callContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;
    auto  name = ctx->IDENTIFIER()->getText();

    auto target = find_graph(name);

    int target_params;

    if(target)
    {
        target_params = target->get_params().size();
    }
    else if(!CFG::is_standard_function(name, target_params))
    {
        throw std::invalid_argument("Visitor::visitExprFunction: target function " + name + " doesn't exist");
    }

    auto                dest = graph->create_temp(current_scope, Type::INT_64);
    std::vector<Symbol> parameters{{name, Symbol::Nature::NAME}, dest};

    for(int i = 0; i < ctx->expression().size(); i++)
    {
        this->visit(ctx->expression()[i]);
        parameters.push_back(pop_symbol());
    }

    if(ctx->expression().size() != target_params)
    {
        throw std::invalid_argument("Visitor::visitExprFunction: target function " + name + " has " + std::to_string(target->get_params().size()) + " parameters (instead of " + std::to_string(target_params) + ")");
    }

    block = graph->current_bb; // get out block after visiting
    block->add_instruction(Operation::call, Type::INT_64, parameters);
    symbols.push(dest);

    return 0;
}

antlrcpp::Any Visitor::visitLoop_stmt(ifccParser::Loop_stmtContext* ctx)
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating bb after the 'while'
    auto out_bb = new BasicBlock(graph);
    graph->add_block(out_bb);

    // 'while' condition
    auto condition_bb = new BasicBlock(graph);
    graph->add_block(condition_bb);

    // Register loop
    loops.push(std::make_pair(condition_bb, out_bb));

    // Branching the previous block to the 'while' condition
    block->exit_true = condition_bb;

    // Visiting the condition expression
    graph->current_bb = condition_bb;
    this->visit(ctx->expression());
    condition_bb = graph->current_bb; // on récupère le "bloc out" de l'expression

    condition_bb->test_symbol = pop_symbol();

    // 'while' block
    auto block_bb = new BasicBlock(graph);
    graph->add_block(block_bb);

    // Branching the condition to the beginning of the block
    condition_bb->exit_true = block_bb;

    // Branching the 'while' block to the condition bb
    graph->current_bb = block_bb;
    this->visit(ctx->block());
    block_bb = graph->current_bb; // on récupère le "bloc out" du bloc
    block_bb->exit_true = condition_bb;

    // bb linking
    condition_bb->exit_false = out_bb;

    // Setting current bb to the out bb
    graph->current_bb = out_bb;

    // Unregister loop
    loops.pop();

    return 0;
}

antlrcpp::Any Visitor::visitCondition_stmt(ifccParser::Condition_stmtContext* ctx)
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating bb after all the conditions
    auto out_bb = new BasicBlock(graph);
    graph->add_block(out_bb);

    BasicBlock* last_condition_bb = nullptr;

    // Following blocks
    for(int i = 0; i < ctx->expression().size(); i++)
    {
        auto condition_bb = new BasicBlock(graph);
        graph->add_block(condition_bb);

        // Branching the first bb
        if(i == 0)
        {
            block->exit_true = condition_bb;
        }

        graph->current_bb = condition_bb;
        this->visit(ctx->expression()[i]);
        condition_bb = graph->current_bb; // on récupère le "bloc out" de l'expression

        condition_bb->test_symbol = pop_symbol();

        if(last_condition_bb)
        {
            last_condition_bb->exit_false = condition_bb;
        }

        // 'if' block
        auto block_bb = new BasicBlock(graph);
        graph->add_block(block_bb);
        block_bb->exit_true = out_bb;

        // bb linking
        condition_bb->exit_true = block_bb;
        condition_bb->exit_false = out_bb;

        graph->current_bb = block_bb;
        this->visit(ctx->block()[i]);
        block_bb = graph->current_bb; // on récupère le "bloc out" du bloc

        // last condition bb
        last_condition_bb = condition_bb;
    }

    // There's an 'else' block
    if(ctx->block().size() > ctx->expression().size())
    {
        auto else_bb = new BasicBlock(graph);
        graph->add_block(else_bb);

        last_condition_bb->exit_false = else_bb;

        graph->current_bb = else_bb;
        this->visit(ctx->block()[ctx->expression().size()]);
        else_bb = graph->current_bb; // on récupère le "bloc out" du bloc

        else_bb->exit_true = out_bb;
    }

    // Setting current bb to the out bb
    graph->current_bb = out_bb;

    return 0;
}

antlrcpp::Any Visitor::visitExprAnd(ifccParser::ExprAndContext* ctx)
{
    // Get results of left and right
    visitChildren(ctx);
    reduce_and();

    return 0;
}

antlrcpp::Any Visitor::visitExprOr(ifccParser::ExprOrContext* ctx)
{
    // Get results of left and right
    visitChildren(ctx);
    reduce_or();

    return 0;
}

antlrcpp::Any Visitor::visitExprNot(ifccParser::ExprNotContext* ctx)
{
    // Get value
    visitChildren(ctx);
    reduce_not();

    return 0;
}

antlrcpp::Any Visitor::visitExprCmp(ifccParser::ExprCmpContext* ctx)
{
    visitChildren(ctx);

    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Destination variable
    auto dest = graph->create_temp(current_scope, Type::INT_64);

    // Get results of left and right
    auto droite = pop_symbol();
    auto gauche = pop_symbol();

    std::vector<Symbol> cmp_params = {dest, gauche, droite};

    if(ctx->EQ())
    {
        block->add_instruction(Operation::cmp_eq, Type::INT_64, cmp_params);
        symbols.push(dest);
    }
    else if(ctx->LT())
    {
        block->add_instruction(Operation::cmp_lt, Type::INT_64, cmp_params);
        symbols.push(dest);
    }
    else if(ctx->NE())
    {
        block->add_instruction(Operation::cmp_eq, Type::INT_64, cmp_params);
        symbols.push(dest);
        reduce_not();
    }
    else if(ctx->GE())
    {
        block->add_instruction(Operation::cmp_lt, Type::INT_64, cmp_params);
        symbols.push(dest);
        reduce_not();
    }
    else if(ctx->LE())
    {
        block->add_instruction(Operation::cmp_eq, Type::INT_64, cmp_params);
        symbols.push(dest);

        auto second_dest = graph->create_temp(current_scope, Type::INT_64);
        block->add_instruction(Operation::cmp_lt, Type::INT_64, {second_dest, gauche, droite});
        symbols.push(second_dest);

        reduce_or();
    }
    else if(ctx->GT())
    {
        block->add_instruction(Operation::cmp_eq, Type::INT_64, cmp_params);
        symbols.push(dest);

        auto second_dest = graph->create_temp(current_scope, Type::INT_64);
        block->add_instruction(Operation::cmp_lt, Type::INT_64, {second_dest, gauche, droite});
        symbols.push(second_dest);

        reduce_or();
        reduce_not();
    }

    return 0;
}

void Visitor::reduce_and()
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating out block
    auto out_bb = new BasicBlock(graph);
    graph->add_block(out_bb);

    // Get values
    auto droite = pop_symbol();
    auto gauche = pop_symbol();

    // Destination variable
    auto dest = graph->create_temp(current_scope, Type::INT_64);

    // True / false blocks
    auto true_bb = new BasicBlock(graph);
    graph->add_block(true_bb);
    true_bb->add_instruction(Operation::ldconst, Type::INT_64, {dest, {1}});
    true_bb->exit_true = out_bb;

    auto false_bb = new BasicBlock(graph);
    graph->add_block(false_bb);
    false_bb->add_instruction(Operation::ldconst, Type::INT_64, {dest, {0}});
    false_bb->exit_true = out_bb;

    // Conditions
    auto droite_bb = new BasicBlock(graph);
    graph->add_block(droite_bb);
    droite_bb->test_symbol = droite;
    droite_bb->exit_true = true_bb;
    droite_bb->exit_false = false_bb;

    auto gauche_bb = new BasicBlock(graph);
    graph->add_block(gauche_bb);
    gauche_bb->test_symbol = gauche;
    gauche_bb->exit_true = droite_bb;
    gauche_bb->exit_false = false_bb;

    symbols.push(dest);

    // Init
    block->exit_true = gauche_bb;
    graph->current_bb = out_bb;
}

void Visitor::reduce_or()
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating out block
    auto out_bb = new BasicBlock(graph);
    graph->add_block(out_bb);

    // Get values
    auto droite = pop_symbol();
    auto gauche = pop_symbol();

    // Destination variable
    auto dest = graph->create_temp(current_scope, Type::INT_64);

    // True / false blocks
    auto true_bb = new BasicBlock(graph);
    graph->add_block(true_bb);
    true_bb->add_instruction(Operation::ldconst, Type::INT_64, {dest, {1}});
    true_bb->exit_true = out_bb;

    auto false_bb = new BasicBlock(graph);
    graph->add_block(false_bb);
    false_bb->add_instruction(Operation::ldconst, Type::INT_64, {dest, {0}});
    false_bb->exit_true = out_bb;

    // Conditions
    auto droite_bb = new BasicBlock(graph);
    graph->add_block(droite_bb);
    droite_bb->test_symbol = droite;
    droite_bb->exit_true = true_bb;
    droite_bb->exit_false = false_bb;

    auto gauche_bb = new BasicBlock(graph);
    graph->add_block(gauche_bb);
    block->test_symbol = gauche;
    block->exit_true = true_bb;
    block->exit_false = droite_bb;

    symbols.push(dest);

    // Init
    block->exit_true = gauche_bb;
    graph->current_bb = out_bb;
}

void Visitor::reduce_not()
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating out block
    auto out_bb = new BasicBlock(graph);
    graph->add_block(out_bb);

    // Get value
    auto value = pop_symbol();

    // Destination variable
    auto dest = graph->create_temp(current_scope, Type::INT_64);

    // True / false blocks
    auto true_bb = new BasicBlock(graph);
    graph->add_block(true_bb);
    true_bb->add_instruction(Operation::ldconst, Type::INT_64, {dest, {1}});
    true_bb->exit_true = out_bb;

    auto false_bb = new BasicBlock(graph);
    graph->add_block(false_bb);
    false_bb->add_instruction(Operation::ldconst, Type::INT_64, {dest, {0}});
    false_bb->exit_true = out_bb;

    // Condition
    auto not_bb = new BasicBlock(graph);
    graph->add_block(not_bb);
    not_bb->test_symbol = value;
    not_bb->exit_true = false_bb;
    not_bb->exit_false = true_bb;

    symbols.push(dest);

    // Init
    block->exit_true = not_bb;
    graph->current_bb = out_bb;
}

const Symbol& Visitor::pop_symbol()
{
    auto*       graph = graphs.back();
    const auto& value = symbols.top();
    symbols.pop();
    return value;
}

void Visitor::open_scope()
{
    auto* scope = new Scope(current_scope);
    scopes.push_back(scope);
    current_scope = scope;
}

void Visitor::close_scope()
{
    current_scope = current_scope->get_parent();
}

antlrcpp::Any Visitor::visitBreak_stmt(ifccParser::Break_stmtContext* ctx)
{
    if(loops.empty())
    {
        throw std::invalid_argument("Visitor::Break_stmtContext: no loop to break");
    }

    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // exit
    block->exit_true = loops.top().second;

    auto garbage = new BasicBlock(graph);
    graph->add_block(garbage);
    graph->current_bb = garbage;

    return 0;
}

antlrcpp::Any Visitor::visitContinue_stmt(ifccParser::Continue_stmtContext* ctx)
{
    if(loops.empty())
    {
        throw std::invalid_argument("Visitor::visitContinue_stmt: no loop to continue");
    }

    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // exit
    block->exit_true = loops.top().first;

    auto garbage = new BasicBlock(graph);
    graph->add_block(garbage);
    graph->current_bb = garbage;

    return 0;
}

CFG* Visitor::find_graph(const std::string& name) const
{
    for(auto graph: graphs)
    {
        if(graph->get_name() == name)
        {
            return graph;
        }
    }

    return nullptr;
}

Symbol Visitor::reduce_constant(ifccParser::ConstantContext* ctx)
{
    if(ctx->NUMERIC())
    {
        return {std::stoi(ctx->getText())};
    }
    else if(ctx->LITERAL())
    {
        std::string                literal = ctx->getText();
        const std::map<char, char> escapes = {{'\\', '\\'}, {'n', '\n'}, {'t', '\t'}, {'0', '\0'}};

        if(literal.size() == 3)
        {
            return {literal[1]};
        }
        else
        {
            return {escapes.count(literal[2]) ? escapes.at(literal[2]) : literal[2]};
        }
    }

    return {0};
}