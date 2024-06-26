#include "CFG.hpp"
#include "BasicBlock.hpp"

/**
 * Constructeur
 */
CFG::CFG(const std::string& name, Type type) : name(name), type(type), block_offset(0), symbol_offset(0), temp_offset(0)
{
    begin_bb = new BasicBlock(this);
    end_bb = new BasicBlock(this);

    current_bb = begin_bb;

    blocks.push_back(begin_bb);
    blocks.push_back(end_bb);
}

/**
 * Destructeur
 * Supprime tous les BasicBlocks du CFG
 */
CFG::~CFG()
{
    for(auto bb: blocks)
    {
        delete bb;
    }
}

/**
 * Renvoie l'etiquette du CFG (name = etiquette)
 * Par exemple, pour le main du programme, name = 'main'
 * Pour une fonction, name = nom de la fonction
 */
const std::string& CFG::get_name() const
{
    return name;
}

/**
 * Renvoie le vector des BasicBLocks d'un CFG
 * Un CFG est un vector de BasicBlocks
 */
const std::vector<BasicBlock*>& CFG::get_blocks() const
{
    return blocks;
}

/**
 * Ajoute un BasicBlock à un CFG
 * @param bb : le BasicBlock à ajouter
 */
void CFG::add_block(BasicBlock* bb)
{
    this->blocks.push_back(bb);
}

/**
 * Génère un nom pour un nouveau BasicBlock à ajouter
 * Par exemple, dans le CFG du main, on souhaite rajouter un troisième bloc,
 * alors la méthode génére l'étiquette "main_BB_3"
 */
std::string CFG::block_name()
{
    return name + "_BB_" + std::to_string(block_offset++);
}

/**
 * Génère un symbol temporaire pour un CFG
 *
 * @param scope : le scope dans lequel le symbol temporaire est créé
 * @param type : le type du symbol temporaire
 * @param pointer : si le symbol temporaire est un pointeur
 */
const Symbol& CFG::create_temp(Scope* scope, Type type, bool pointer)
{
    symbol_offset += Symbol::get_type_size(type);
    return scope->create_temp(symbol_offset, temp_offset++, type, pointer);
}

/**
 * Génère un symbol temporaire pour un CFG
 * @param scope : le scope dans lequel le symbol temporaire est créé
 * @param symbol : le symbol temporaire
 */
const Symbol& CFG::create_temp(Scope* scope, const Symbol& symbol)
{
    symbol_offset += Symbol::get_type_size(symbol.get_type());
    return scope->create_temp(symbol_offset, temp_offset++, symbol.get_type(), symbol.is_pointer());
}

/**
 * Ajoute la taille d'un symbol au symbol offset et renvoie la nouvelle valeur
 *
 * @param type : le type du symbol
 * @param length : la taille du symbol
 */
int CFG::next_symbol_offset(Type type, int length)
{
    symbol_offset += Symbol::get_type_size(type) * length;
    return symbol_offset;
}

/**
 * Renvoie l'offset du symbol
 */
int CFG::get_symbol_offset() const
{
    return symbol_offset;
}

/**
 * Ajoute un paramètre à un CFG pour les fonctions
 */
void CFG::add_param(Symbol param)
{
    params.push_back(param);
}

/**
 * Renvoie les paramètres d'un CFG pour les fonctions
 */
std::vector<Symbol> CFG::get_params() const
{
    return params;
}

/**
 * Renvoie le type de retour attendu par le CFG
 * Soit "int", soit "void"
 */
const Type CFG::get_type() const
{
    return type;
}

/**
 * Indique si une fonction est une fonction standard
 * Permets de savoir si une fonction est une fonction standard comme putchar ou getchar
 *
 * @param name : le nom de la fonction
 * @param params : le nombre de paramètres de la fonction
 */
bool CFG::is_standard_function(const std::string& name, int& params)
{
    std::array<std::pair<std::string, int>, 2> functions = {std::make_pair("putchar", 1), std::make_pair("getchar", 0)};

    for(auto [label, count]: functions)
    {
        if(label == name)
        {
            params = count;
            return true;
        }
    }

    return false;
}