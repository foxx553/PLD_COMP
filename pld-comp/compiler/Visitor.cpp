#include "Visitor.hpp"

#include <iostream>


/**
 * Constructeur
 */

Visitor::Visitor() : current_scope(nullptr)
{
}


/**
 * Destructeur
 */

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


/**
 * TO DO Fatih
*/
IR Visitor::get_ir()
{
    return {graphs, scopes.front()};
}


/**
 * C'est le Visiteur de notre programme
 * Ce visiteur va permetre de parcourir le main et les différentes fonctions
*/

antlrcpp::Any Visitor::visitProg(ifccParser::ProgContext* ctx)
{
    open_scope();
    this->visitChildren(ctx);
    close_scope();

    return 0;
}



/**
 * C'est le Visiteur pour le main et les fonctions
 * Crée un nouveau graphe(CFG) à partir du nom et du type de retour de la fonction
 *TO DO Fatih 
*/

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

        auto type = Symbol::type_from_string(ctx->TYPE()[i]->getText());
        auto length = declaration->array_length() ? reduce_constant(declaration->array_length()->constant()).get_value() : 1;

        /*Si un tableau est déclaré, sa taille ne doit pas etre inferieure à 1*/
        if(length < 1)
        {
            throw std::invalid_argument("Visitor::visitDeclaration_stmt: array length can't be less than 1");
        }

        auto offset = graph->next_symbol_offset(type, length);

        const auto& symbol = current_scope->add_symbol({declaration->IDENTIFIER()->getText(), type, offset, length, Symbol::Nature::VARIABLE, !!declaration->MUL() || declaration->array_length()});

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



/**
 * C'est le Visiteur du return
 * 1- Ajoute l'instruction "ret" au current BasicBlock
 * 2- Relis la sortie du current BasicBlock au BasicBlock de fin du graphe
*/

antlrcpp::Any Visitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx)
{
    this->visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto dest = pop_symbol();

    /*La fonction VOID ne contient pas de return (sinon déclenche un Warning)*/
    if(graph->get_type() == Type::VOID)                                       
    {
        std::cerr << "warning: 'return' in function '" << graph->get_name() << "' returning void" << std::endl;
    }

    block->add_instruction(Operation::ret, Type::INT_64, {dest});

    // exit
    block->exit_true = graph->end_bb;                                          

    auto garbage = new BasicBlock(graph);
    graph->add_block(garbage);
    graph->current_bb = garbage;

    return 0;
}



/**
 * C'est le Visiteur pour les délarations des varaibles globales
 * TO DO Fatih
*/

antlrcpp::Any Visitor::visitDeclaration_global(ifccParser::Declaration_globalContext* ctx)
{
    auto type = Symbol::type_from_string(ctx->TYPE()->getText());

    /*Les varaibles de type void ne sont pas acceptées*/
    if(type == Type::VOID)
    {
        throw std::invalid_argument("Visitor::visitDeclaration_global: can't have void variable");
    }

    for(auto declaration: ctx->declaration())
    {
        auto length = declaration->array_length() ? reduce_constant(declaration->array_length()->constant()).get_value() : 1;

        /*Si un tableau est déclaré, sa taille ne doit pas etre inferieure à 1*/
        if(length < 1)
        {
            throw std::invalid_argument("Visitor::visitDeclaration_stmt: array length can't be less than 1");
        }

        current_scope->add_symbol({declaration->IDENTIFIER()->getText(), type, 0, length, Symbol::Nature::GLOBAL, !!declaration->MUL() || declaration->array_length()});
    }

    return 0;
}



/**
 * C'est le Visiteur pour les délarations des varaibles 
 * TO DO Fatih
*/

antlrcpp::Any Visitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext* ctx)
{
    auto type = Symbol::type_from_string(ctx->TYPE()->getText());

    /*Les varaibles de type void ne sont pas acceptées*/
    if(type == Type::VOID)
    {
        throw std::invalid_argument("Visitor::visitDeclaration_stmt: can't have void variable");
    }

    for(auto declaration: ctx->declaration())
    {
        auto* graph = graphs.back();
        auto* block = graph->current_bb;

        auto length = declaration->array_length() ? reduce_constant(declaration->array_length()->constant()).get_value() : 1;

        /*Si un tableau est déclaré, sa taille ne doit pas etre inferieure à 1*/
        if(length < 1)
        {
            throw std::invalid_argument("Visitor::visitDeclaration_stmt: array length can't be less than 1");
        }

        auto offset = graph->next_symbol_offset(type, length);

        if(declaration->array_length())
        {
            const auto& symbol = current_scope->add_symbol({declaration->IDENTIFIER()->getText(), Type::INT_64, graph->next_symbol_offset(Type::INT_64, 1), length, Symbol::Nature::VARIABLE, true});
            block->add_instruction(Operation::ldconst, Type::INT_64, {symbol, {-offset}});
            block->add_instruction(Operation::add, Type::INT_64, {symbol, {"bp", Symbol::Nature::REGISTER}, symbol});
        }
        else
        {
            const auto& symbol = current_scope->add_symbol({declaration->IDENTIFIER()->getText(), type, offset, length, Symbol::Nature::VARIABLE, !!declaration->MUL()});

            if(declaration->expression())
            {
                this->visit(declaration->expression());
                auto expression = pop_symbol();

                block = graph->current_bb; // get out block after visiting

                if(symbol.get_type() != expression.get_type())
                {
                    throw std::invalid_argument("Visitor::visitAssignment: can't assign type " + Symbol::type_to_string(symbol.get_type()) + " to " + Symbol::type_to_string(expression.get_type()));
                }

                block->add_instruction(Operation::copy, Type::INT_64, {symbol, expression});
            }
        }
    }

    return 0;
}



/**
 * C'est le Visiteur pour les affectations à une Lvalue quelconque
 * TO DO Fatih
*/

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

    auto dest = graph->create_temp(current_scope, identifier);
    block->add_instruction(Operation::ldconst, Type::INT_64, {dest, {-identifier.get_offset()}});
    block->add_instruction(Operation::add, Type::INT_64, {dest, {"bp", Symbol::Nature::REGISTER}, dest});

    if(ctx->array_index())
    {
        this->visit(ctx->array_index());
        auto index = pop_symbol();
        block = graph->current_bb; // get out block after visiting

        block->add_instruction(Operation::rmem, Type::INT_64, {dest, dest});
        block->add_instruction(Operation::mul, Type::INT_64, {index, {Symbol::get_type_size(identifier.get_type())}, index});
        block->add_instruction(Operation::add, Type::INT_64, {dest, index, dest});
    }

    if(ctx->MUL())
    {
        if(!identifier.is_pointer())
        {
            throw std::invalid_argument("Visitor::visitExprLvalue: cannot dereference variable " + identifier.get_name() + " which is not a pointer type");
        }

        block->add_instruction(Operation::rmem, Type::INT_64, {dest, dest});
    }

    symbols.push(dest);

    return 0;
}



/**
 * C'est le Visiteur pour d'un Bloc
 * Oe le Scope
 * TO DO Fatih
*/

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

    if(right.get_type() != left.get_type())
    {
        throw std::invalid_argument("Visitor::visitAssignment: can't assign type " + Symbol::type_to_string(right.get_type()) + " to " + Symbol::type_to_string(left.get_type()));
    }

    if(left.get_nature() == Symbol::Nature::GLOBAL)
    {
        block->add_instruction(Operation::copy, Type::INT_64, {left, right});
        return 0;
    }

    block->add_instruction(Operation::wmem, Type::INT_64, {left, right});

    symbols.push(left);

    return 0;
}


/**
 * C'est le Visiteur pour les affectations des expressions
 * TO DO Fatih
*/

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

    if(ctx->REF() || value.get_nature() == Symbol::Nature::GLOBAL)
    {
        symbols.push(value);
    }
    else
    {
        auto dest = graph->create_temp(current_scope, Type::INT_64);
        block->add_instruction(Operation::rmem, Type::INT_64, {dest, value});
        symbols.push(dest);
    }

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

antlrcpp::Any Visitor::visitExprBitwiseOperation(ifccParser::ExprBitwiseOperationContext* ctx)
{
    visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto droite = pop_symbol();
    auto gauche = pop_symbol();

    auto dest = graph->create_temp(current_scope, Type::INT_64);

    block->add_instruction(ctx->BAND() ? Operation::band : (ctx->BXOR() ? Operation::bxor : Operation::bor), Type::INT_64, {dest, gauche, droite});

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



/**
 * Retrouve un graphe CFG à partir de son nom(étiquette)
 * @param name : le nom du graphe que l'on cherche
*/

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



/**
 * C'est le Visiteur pour l'appel des fonctions
*/

antlrcpp::Any Visitor::visitExprFunction(ifccParser::ExprFunctionContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;
    auto  name = ctx->function_call()->IDENTIFIER()->getText();

    /*A partir du nom de la fonction, on recherche le graphe CFG correspondant*/
    auto target = find_graph(name);                                             

    int target_params;
    if(!CFG::is_standard_function(name, target_params))
    {
        /*Vérifie si la fonction existe */
        if(!target)
        {
            throw std::invalid_argument("Visitor::visitExprFunction: target function " + name + " doesn't exist");
        }

        /*La fonction void ne doit pas etre appelée dans une affectation*/
        if(target->get_type() == Type::VOID)
        {
            throw std::invalid_argument("Visitor::visitExprFunction: target function " + name + " returns void");
        }
    }

    this->visitChildren(ctx);

    return 0;
}



/**
 * C'est le Visiteur pour les appels de fonctions
 * 
*/

antlrcpp::Any Visitor::visitFunction_call(ifccParser::Function_callContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;
    auto  name = ctx->IDENTIFIER()->getText();

    /*A partir du nom de la fonction, on recherche le graphe CFG correspondant*/
    auto target = find_graph(name);

    int target_params;

    /*Vérifie si la fonction existe */
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



/**
 * C'est le Visiteur pour la boucle
 * On crée un BasicBlock pour la condition de la boucle et on empile le BasicBlock dans loops
 * La pile loops nous permet d'implémenter le break et le continue
 * On visite l'intérieur de la boucle (pour créer d'autres BasicBlock)
 * A la fin, on crée un BasicBlock pour la sortie de la boucle et on dépile loops
*/

antlrcpp::Any Visitor::visitLoop_stmt(ifccParser::Loop_stmtContext* ctx)
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating bb after the 'while'
    auto out_bb = new BasicBlock(graph);
    graph->add_block(out_bb);

    // 'while' condition
    auto condition_begin_bb = new BasicBlock(graph);
    graph->add_block(condition_begin_bb);

    // Enregistre la loop et sa sortie(exit_false)
    loops.push(std::make_pair(condition_begin_bb, out_bb));

    // Branching the previous block to the 'while' condition
    block->exit_true = condition_begin_bb;

    // Visiting the condition expression
    graph->current_bb = condition_begin_bb;
    this->visit(ctx->expression());
    auto condition_end_bb = graph->current_bb; // on récupère le "bloc out" de l'expression

    condition_end_bb->test_symbol = pop_symbol();

    // 'while' block
    auto block_bb = new BasicBlock(graph);
    graph->add_block(block_bb);

    // Branching the condition to the beginning of the block
    condition_end_bb->exit_true = block_bb;
    condition_end_bb->exit_false = out_bb;

    // Branching the 'while' block to the condition bb
    graph->current_bb = block_bb;
    this->visit(ctx->block());
    block_bb = graph->current_bb; // on récupère le "bloc out" du bloc
    block_bb->exit_true = condition_begin_bb;

    // Setting current bb to the out bb
    graph->current_bb = out_bb;

    // Unregister loop
    loops.pop();

    return 0;
}



/**
 * C'est le Visiteur pour l'instruction break
 * Quand on entre dans une boucle, le BasicBlock "condition" de la boucle est empilé dans la pile loops
 * Si un break est rencontré, on fait un saut vers le exit_false de la tete de pile
*/

antlrcpp::Any Visitor::visitBreak_stmt(ifccParser::Break_stmtContext* ctx)
{
    //La pile loops est vide, alors le break n'est pas un une boucle (erreur)
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



/**
 * C'est le Visiteur pour l'instruction continue
 * Quand on entre dans une boucle, le BasicBlock "condition" de la boucle est empilé dans la pile loops
 * Si un continue est rencontré, on fait un saut vers la tete de pile
*/

antlrcpp::Any Visitor::visitContinue_stmt(ifccParser::Continue_stmtContext* ctx)
{
    //La pile loops est vide, alors le continue n'est pas une boucle (erreur)
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



/**
 * C'est le Visiteur pour les conditions de if/boucle
 * TO DO Fatih
*/

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


/**
 * TO DO Fatih
*/

antlrcpp::Any Visitor::visitExprAnd(ifccParser::ExprAndContext* ctx)
{
    // Get results of left and right
    visitChildren(ctx);
    reduce_and();

    return 0;
}



/**
 * TO DO Fatih
*/

antlrcpp::Any Visitor::visitExprOr(ifccParser::ExprOrContext* ctx)
{
    // Get results of left and right
    visitChildren(ctx);
    reduce_or();

    return 0;
}



/**
 * TO DO Fatih
*/

antlrcpp::Any Visitor::visitExprNot(ifccParser::ExprNotContext* ctx)
{
    // Get value
    visitChildren(ctx);
    reduce_not();

    return 0;
}



/**
 * TO DO Fatih
*/

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



/**
 * TO DO Fatih
*/

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



/**
 * TO DO Fatih
*/

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
    gauche_bb->test_symbol = gauche;
    gauche_bb->exit_true = true_bb;
    gauche_bb->exit_false = droite_bb;

    symbols.push(dest);

    // Init
    block->exit_true = gauche_bb;
    graph->current_bb = out_bb;
}



/**
 * TO DO Fatih
*/

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



/**
 * TO DO Fatih
*/

const Symbol& Visitor::pop_symbol()
{
    auto*       graph = graphs.back();
    const auto& value = symbols.top();
    symbols.pop();
    return value;
}



/**
 * TO DO Fatih
*/

void Visitor::open_scope()
{
    auto* scope = new Scope(current_scope);
    scopes.push_back(scope);
    current_scope = scope;
}



/**
 * TO DO Fatih
*/

void Visitor::close_scope()
{
    current_scope = current_scope->get_parent();
}



/**
 * TO DO Fatih
*/

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