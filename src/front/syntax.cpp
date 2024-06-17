#include "front/syntax.h"
#include <iostream>
#include <cassert>

using frontend::Parser;

// #define DEBUG_PARSER
#define TODO assert(0 && "todo")
#define saveChildrenNum int curChildrenNums = res->children.size()
#define saveIndex int lastIndex = index

/**
 * *文法规则：
 * 1 编译单元 CompUnit -> (Decl | FuncDef) [CompUnit]
 *
 * 2 声明 Decl -> ConstDecl | VarDecl
 * 3 常量声明 ConstDecl -> 'const' BType ConstDef { ',' ConstDef } ';'
 * 4 基本类型 BType -> 'int' | 'float'
 * 5 常量定义 ConstDef -> Ident { '[' ConstExp ']' } '=' ConstInitVal
 * 6 常量初值 ConstInitVal -> ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
 * 7 变量声明 VarDecl -> BType VarDef { ',' VarDef } ';'
 * 8 变量定义 VarDef -> Ident { '[' ConstExp ']' } [ '=' InitVal ]
 * 9 变量初值 InitVal -> Exp | '{' [ InitVal { ',' InitVal } ] '}'
 *
 * 10函数定义 FuncDef -> FuncType Ident '(' [FuncFParams] ')' Block
 * 11函数类型 FuncType -> 'void' | 'int' | 'float'
 * 12函数形参 FuncFParam -> BType Ident [ '[' ']' { '[' Exp ']' } ]
 * 13函数形参表 FuncFParams -> FuncFParam { ',' FuncFParam }
 *
 * 14语句块 Block -> '{' { BlockItem } '}'
 * 15语句块项 BlockItem -> Decl | Stmt
 * 16语句 Stmt -> LVal '=' Exp ';' | Block | 'if' '(' Cond ')' Stmt [ 'else' Stmt ] | 'while' '(' Cond ')' Stmt | 'break' ';' | 'continue' ';' | 'return' [Exp] ';' | [Exp] ';'
 *
 * 17表达式 Exp -> AddExp
 * 18条件表达式 Cond -> LOrExp
 * 19左值表达式 LVal -> Ident { '[' Exp ']' }
 * 20数值 Number -> IntConst | floatConst
 * 21基本表达式 PrimaryExp -> '(' Exp ')' | LVal | Number
 * 22一元表达式 UnaryExp -> PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
 * 23单目运算符 UnaryOp -> '+' | '-' | '!'
 * 24函数实参表 FuncRParams -> Exp { ',' Exp }
 * 25乘除模表达式 MulExp -> UnaryExp { ( '*' | '/' | '%' ) UnaryExp }
 * 26加减表达式 AddExp -> MulExp { ( '+' | '-' ) MulExp }
 * 27关系表达式 RelExp -> AddExp { ( '<' | '>' | '<=' | '>=' ) AddExp }
 * 28相等性表达式 EqExp -> RelExp { ( '==' | '!=' ) RelExp }
 * 29逻辑与表达式 LAndExp -> EqExp [ '&&' LAndExp ]
 * 30逻辑或表达式 LOrExp -> LAndExp [ '||' LOrExp ]
 * 31常量表达式 ConstExp -> AddExp
 */

Parser::Parser(const std::vector<frontend::Token> &tokens) : index(0), token_stream(tokens) {}

Parser::~Parser() {}

frontend::CompUnit *Parser::get_abstract_syntax_tree()
{
    // TODO; lab1todo9
    return parseCompUnit(nullptr); // 构造根节点
}

void Parser::log(AstNode *node)
{
#ifdef DEBUG_PARSER
    std::cout << "in parse" << toString(node->type) << ", cur_token_type::" << toString(token_stream[index].type) << ", token_val::" << token_stream[index].value << '\n';
#endif
}

// TODO; lab1todo10
/**
 * @brief 1 编译单元 CompUnit -> (Decl | FuncDef) [CompUnit]
 * @param root
 * @return frontend::CompUnit*
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
frontend::CompUnit *Parser::parseCompUnit(AstNode *root)
{
    CompUnit *res = new CompUnit(root); // 构造根节点

    bool isDecl = false;    // 标记Decl分支
    bool isFuncDef = false; // 标记FuncDef分支
    saveIndex;              // 记录当前index
    saveChildrenNum;        // 记录进入分支前的children数量

    if (matchDecl().count(token_stream[index].type)) // 匹配Decl
    {
        isDecl = parseDecl(res);
        if (!isDecl)
            undo(lastIndex, res, curChildrenNums);
    }

    if (!isDecl && matchFuncDef().count(token_stream[index].type)) // 匹配FuncDef
        isFuncDef = parseFuncDef(res);

    if (!isDecl && !isFuncDef) // 两个分支都不匹配报错，只有这里是直接报错
        assert(0 && "error in parseCompUnit");

    if (matchCompUnit().count(token_stream[index].type)) // 匹配CompUnit
        parseCompUnit(res);

    return res;
}

/**
 * @brief 2 声明 Decl -> ConstDecl | VarDecl
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseDecl(AstNode *root)
{
    Decl *res = new Decl(root);
    bool isConstDecl = false;
    bool isVarDecl = false;
    if (matchConstDecl().count(token_stream[index].type)) // 匹配ConstDecl
        isConstDecl = parseConstDecl(res);
    else if (matchVarDecl().count(token_stream[index].type)) // 匹配VarDecl
        isVarDecl = parseVarDecl(res);
    if (!isConstDecl && !isVarDecl)
        return false;
    return true;
}

/**
 * @brief 3 常量声明 ConstDecl -> 'const' BType ConstDef { ',' ConstDef } ';'
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseConstDecl(AstNode *root)
{
    ConstDecl *res = new ConstDecl(root);
    new Term(token_stream[index++], res);
    parseBType(res);                                               // 匹配BType
    parseConstDef(res);                                            // 匹配ConstDef
    while (token_stream[index].type == frontend::TokenType::COMMA) // 匹配{ ',' ConstDef }
    {
        new Term(token_stream[index++], res);
        parseConstDef(res);
    }
    new Term(token_stream[index++], res);
    return true;
}

/**
 * @brief 4 基本类型 BType -> 'int' | 'float'
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseBType(AstNode *root)
{
    BType *res = new BType(root);
    if (token_stream[index].type == frontend::TokenType::INTTK) // 匹配'int'
        new Term(token_stream[index++], res);
    else if (token_stream[index].type == frontend::TokenType::FLOATTK) // 匹配'float'
        new Term(token_stream[index++], res);
    return true;
}

/**
 * @brief 5 常量定义 ConstDef -> Ident { '[' ConstExp ']' } '=' ConstInitVal
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseConstDef(AstNode *root)
{
    ConstDef *res = new ConstDef(root);
    new Term(token_stream[index++], res);
    while (token_stream[index].type == frontend::TokenType::LBRACK) // 匹配{ '[' ConstExp ']' }
    {
        new Term(token_stream[index++], res);
        parseConstExp(res);
        new Term(token_stream[index++], res);
    }
    new Term(token_stream[index++], res);
    parseConstInitVal(res); // 匹配ConstInitVal
    return true;
}

/**
 * @brief 6 常量初值 ConstInitVal -> ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseConstInitVal(AstNode *root)
{
    ConstInitVal *res = new ConstInitVal(root);
    if (matchConstExp().count(token_stream[index].type)) // 匹配ConstExp
        parseConstExp(res);
    if (token_stream[index].type == frontend::TokenType::LBRACE) // 匹配'{' [ ConstInitVal { ',' ConstInitVal } ] '}'
    {
        new Term(token_stream[index++], res);
        if (matchConstInitVal().count(token_stream[index].type))
        {
            parseConstInitVal(res);
            while (token_stream[index].type == frontend::TokenType::COMMA)
            {
                new Term(token_stream[index++], res);
                parseConstInitVal(res);
            }
        }
        new Term(token_stream[index++], res);
    }

    return true;
}

/**
 * @brief 7 变量声明 VarDecl -> BType VarDef { ',' VarDef } ';'
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseVarDecl(AstNode *root)
{
    VarDecl *res = new VarDecl(root);
    parseBType(res);                                               // 匹配BType
    parseVarDef(res);                                              // 匹配VarDef
    while (token_stream[index].type == frontend::TokenType::COMMA) // 匹配{ ',' VarDef }
    {
        new Term(token_stream[index++], res);
        parseVarDef(res);
    }
    if (token_stream[index].type != frontend::TokenType::SEMICN) // 匹配';'
        return false;
    new Term(token_stream[index++], res);

    return true;
}

/**
 * @brief 8 变量定义 VarDef -> Ident { '[' ConstExp ']' } [ '=' InitVal ]
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseVarDef(AstNode *root)
{
    VarDef *res = new VarDef(root);
    new Term(token_stream[index++], res);
    while (token_stream[index].type == frontend::TokenType::LBRACK) // 匹配{ '[' ConstExp ']' }
    {
        new Term(token_stream[index++], res);
        parseConstExp(res);
        new Term(token_stream[index++], res);
    }
    if (token_stream[index].type == frontend::TokenType::ASSIGN) // 匹配[ '=' InitVal ]
    {
        new Term(token_stream[index++], res);
        parseInitVal(res);
    }
    return true;
}

/**
 * @brief 9 变量初值 InitVal -> Exp | '{' [ InitVal { ',' InitVal } ] '}'
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseInitVal(AstNode *root)
{
    InitVal *res = new InitVal(root);

    if (matchExp().count(token_stream[index].type)) // 匹配Exp
        parseExp(res);
    if (token_stream[index].type == frontend::TokenType::LBRACE) // 匹配'{' [ InitVal { ',' InitVal } ] '}'
    {
        new Term(token_stream[index++], res);
        if (matchInitVal().count(token_stream[index].type))
        {
            parseInitVal(res);
            while (token_stream[index].type == frontend::TokenType::COMMA)
            {
                new Term(token_stream[index++], res);
                parseInitVal(res);
            }
        }
        new Term(token_stream[index++], res);
    }
    return true;
}

/**
 * @brief 10函数定义 FuncDef -> FuncType Ident '(' [FuncFParams] ')' Block
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseFuncDef(AstNode *root)
{
    FuncDef *res = new FuncDef(root);
    parseFuncType(res); // 匹配FuncType
    new Term(token_stream[index++], res);
    new Term(token_stream[index++], res);
    if (matchFuncFParams().count(token_stream[index].type)) // 匹配[FuncFParams]
        parseFuncFParams(res);
    new Term(token_stream[index++], res);
    parseBlock(res); // 匹配Block
    return true;
}

/**
 * @brief 11函数类型 FuncType -> 'void' | 'int' | 'float'
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseFuncType(AstNode *root)
{
    FuncType *res = new FuncType(root);

    if (token_stream[index].type == frontend::TokenType::VOIDTK)
        new Term(token_stream[index++], res);
    else if (token_stream[index].type == frontend::TokenType::INTTK)
        new Term(token_stream[index++], res);
    else if (token_stream[index].type == frontend::TokenType::FLOATTK)
        new Term(token_stream[index++], res);

    return true;
}

/**
 * @brief 12函数形参 FuncFParam -> BType Ident [ '[' ']' { '[' Exp ']' } ]
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseFuncFParam(AstNode *root)
{
    FuncFParam *res = new FuncFParam(root);
    parseBType(res); // 匹配BType
    new Term(token_stream[index++], res);
    if (token_stream[index].type == frontend::TokenType::LBRACK) // 匹配[ '[' ']' { '[' Exp ']' } ]
    {
        new Term(token_stream[index++], res);
        new Term(token_stream[index++], res);
        while (token_stream[index].type == frontend::TokenType::LBRACK)
        {
            new Term(token_stream[index++], res);
            parseExp(res);
            new Term(token_stream[index++], res);
        }
    }
    return true;
}

/**
 * @brief 13函数形参表 FuncFParams -> FuncFParam { ',' FuncFParam }
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseFuncFParams(AstNode *root)
{
    FuncFParams *res = new FuncFParams(root);
    parseFuncFParam(res);                                          // 匹配FuncFParam
    while (token_stream[index].type == frontend::TokenType::COMMA) // 匹配{ ',' FuncFParam }
    {
        new Term(token_stream[index++], res);
        parseFuncFParam(res);
    }
    return true;
}

/**
 * @brief 14语句块 Block -> '{' { BlockItem } '}'
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseBlock(AstNode *root)
{
    Block *res = new Block(root);
    new Term(token_stream[index++], res);
    while (matchBlockItem().count(token_stream[index].type)) // 匹配{ BlockItem }
        parseBlockItem(res);
    new Term(token_stream[index++], res);
    return true;
}

// 15语句块项 BlockItem -> Decl | Stmt
bool Parser::parseBlockItem(AstNode *root)
{
    BlockItem *res = new BlockItem(root);
    if (matchDecl().count(token_stream[index].type)) // 匹配Decl
        parseDecl(res);
    else if (matchStmt().count(token_stream[index].type)) // 匹配Stmt
        parseStmt(res);
    return true;
}

/**
 * @brief 16语句 Stmt -> LVal '=' Exp ';' | Block | 'if' '(' Cond ')' Stmt [ 'else' Stmt ] | 'while' '(' Cond ')' Stmt | 'break' ';' | 'continue' ';' | 'return' [Exp] ';' | [Exp] ';'
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseStmt(AstNode *root)
{
    Stmt *res = new Stmt(root);
    saveIndex;
    saveChildrenNum;

    if (matchLVal().count(token_stream[index].type)) // 匹配LVal '=' Exp ';'
    {
        parseLVal(res);
        if (token_stream[index].type != frontend::TokenType::ASSIGN)
        {
            undo(lastIndex, res, curChildrenNums); // 不可删除
            goto BlockCase;
        }
        new Term(token_stream[index++], res);
        parseExp(res);
        new Term(token_stream[index++], res);
        return true;
    }

BlockCase:
    if (matchBlock().count(token_stream[index].type)) // 匹配Block
        parseBlock(res);
    else if (token_stream[index].type == frontend::TokenType::IFTK) // 匹配'if' '(' Cond ')' Stmt [ 'else' Stmt ]
    {
        new Term(token_stream[index++], res);
        new Term(token_stream[index++], res);
        parseCond(res);
        new Term(token_stream[index++], res);
        parseStmt(res);
        if (token_stream[index].type == frontend::TokenType::ELSETK)
        {
            new Term(token_stream[index++], res);
            parseStmt(res);
        }
    }
    else if (token_stream[index].type == frontend::TokenType::WHILETK) // 匹配'while' '(' Cond ')' Stmt
    {
        new Term(token_stream[index++], res);
        new Term(token_stream[index++], res);
        parseCond(res);
        new Term(token_stream[index++], res);
        parseStmt(res);
    }
    else if (token_stream[index].type == frontend::TokenType::BREAKTK) // 匹配'break' ';'
    {
        new Term(token_stream[index++], res);
        new Term(token_stream[index++], res);
    }
    else if (token_stream[index].type == frontend::TokenType::CONTINUETK) // 匹配'continue' ';'
    {
        new Term(token_stream[index++], res);
        new Term(token_stream[index++], res);
    }
    else if (token_stream[index].type == frontend::TokenType::RETURNTK) // 匹配'return' [Exp] ';'
    {
        new Term(token_stream[index++], res);

        if (matchExp().count(token_stream[index].type))
            parseExp(res);

        new Term(token_stream[index++], res);
    }
    else if (matchExp().count(token_stream[index].type)) // 匹配[Exp] ';'
    {
        parseExp(res);
        new Term(token_stream[index++], res);
    }
    else if (token_stream[index].type == frontend::TokenType::SEMICN) // 匹配';'
        new Term(token_stream[index++], res);

    return true;
}

/**
 * @brief 17表达式 Exp -> AddExp
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseExp(AstNode *root)
{
    Exp *res = new Exp(root);
    parseAddExp(res);
    return true;
}

/**
 * @brief 18条件表达式 Cond -> LOrExp
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseCond(AstNode *root)
{
    Cond *res = new Cond(root);
    parseLOrExp(res);
    return true;
}

/**
 * @brief 19左值表达式 LVal -> Ident { '[' Exp ']' }
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseLVal(AstNode *root)
{
    LVal *res = new LVal(root);
    new Term(token_stream[index++], res);                           // 匹配Ident
    while (token_stream[index].type == frontend::TokenType::LBRACK) // 匹配{ '[' Exp ']' }
    {
        new Term(token_stream[index++], res);
        parseExp(res);
        new Term(token_stream[index++], res);
    }
    return true;
}

/**
 * @brief 20数值 Number -> IntConst | floatConst
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseNumber(AstNode *root)
{
    Number *res = new Number(root);
    new Term(token_stream[index++], res);
    return true;
}

/**
 * @brief 21基本表达式 PrimaryExp -> '(' Exp ')' | LVal | Number
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parsePrimaryExp(AstNode *root)
{
    PrimaryExp *res = new PrimaryExp(root);
    if (token_stream[index].type == frontend::TokenType::LPARENT) // 匹配'(' Exp ')'
    {
        new Term(token_stream[index++], res);
        parseExp(res);
        new Term(token_stream[index++], res); // ')'
    }
    else if (matchLVal().count(token_stream[index].type)) // 匹配LVal
        parseLVal(res);
    else if (matchNumber().count(token_stream[index].type)) // 匹配Number
        parseNumber(res);
    return true;
}

/**
 * @brief 22一元表达式 UnaryExp -> PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseUnaryExp(AstNode *root)
{
    //? 这个表达式存在问题，比如测试点9中的b=func(a);
    //? 执行到这一步的时候，需要匹配func字段
    //? 如果按照正常顺序匹配，是会先与PrimaryExp匹配
    //? 此时会按照下列顺序匹配：
    //? PrimaryExp -> '(' Exp ')' | LVal | Number
    //? LVal -> Ident { '[' Exp ']' }
    //? 这时func匹配Ident。但是这个匹配不是我们需要的
    //? 所以需要修改匹配的顺序，先进行Ident '(' [FuncRParams] ')'匹配
    //? 然后就可以解决这一类问题
    UnaryExp *res = new UnaryExp(root);
    saveIndex;
    saveChildrenNum;

    if (token_stream[index].type == frontend::TokenType::IDENFR) // 匹配Ident '(' [FuncRParams] ')'
    {
        new Term(token_stream[index++], res);
        if (token_stream[index].type != frontend::TokenType::LPARENT)
        {
            undo(lastIndex, res, curChildrenNums); // 不可删除
            goto PrimaryExpCase;
        }
        new Term(token_stream[index++], res);
        if (matchFuncRParams().count(token_stream[index].type))
            parseFuncRParams(res);
        new Term(token_stream[index++], res);
        return true;
    }

PrimaryExpCase:
    if (matchPrimaryExp().count(token_stream[index].type)) // 匹配PrimaryExp
    {
        parsePrimaryExp(res);
        return true;
    }

    if (matchUnaryOp().count(token_stream[index].type)) // 匹配UnaryOp UnaryExp
    {
        parseUnaryOp(res);
        parseUnaryExp(res);
    }

    return true;
}

/**
 * @brief 23单目运算符 UnaryOp -> '+' | '-' | '!'
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseUnaryOp(AstNode *root)
{
    UnaryOp *res = new UnaryOp(root);

    if (token_stream[index].type != frontend::TokenType::PLUS && token_stream[index].type != frontend::TokenType::MINU && token_stream[index].type != frontend::TokenType::NOT)
        return false;
    new Term(token_stream[index++], res);

    return true;
}

/**
 * @brief 24函数实参表 FuncRParams -> Exp { ',' Exp }
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseFuncRParams(AstNode *root)
{
    FuncRParams *res = new FuncRParams(root);
    parseExp(res);
    while (token_stream[index].type == frontend::TokenType::COMMA)
    {
        new Term(token_stream[index++], res);
        parseExp(res);
    }
    return true;
}

/**
 * @brief 25乘除模表达式 MulExp -> UnaryExp { ( '*' | '/' | '%' ) UnaryExp }
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseMulExp(AstNode *root)
{
    MulExp *res = new MulExp(root);
    parseUnaryExp(res);
    while (token_stream[index].type == frontend::TokenType::MULT || token_stream[index].type == frontend::TokenType::DIV || token_stream[index].type == frontend::TokenType::MOD)
    {
        new Term(token_stream[index++], res);
        parseUnaryExp(res);
    }
    return true;
}

/**
 * @brief 26加减表达式 AddExp -> MulExp { ( '+' | '-' ) MulExp }
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseAddExp(AstNode *root)
{
    AddExp *res = new AddExp(root);
    parseMulExp(res);
    while (token_stream[index].type == frontend::TokenType::PLUS || token_stream[index].type == frontend::TokenType::MINU)
    {
        new Term(token_stream[index++], res);
        parseMulExp(res);
    }
    return true;
}

/**
 * @brief 27关系表达式 RelExp -> AddExp { ( '<' | '>' | '<=' | '>=' ) AddExp }
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseRelExp(AstNode *root)
{
    RelExp *res = new RelExp(root);
    parseAddExp(res);
    while (token_stream[index].type == frontend::TokenType::LSS || token_stream[index].type == frontend::TokenType::GTR || token_stream[index].type == frontend::TokenType::LEQ || token_stream[index].type == frontend::TokenType::GEQ)
    {
        new Term(token_stream[index++], res);
        parseAddExp(res);
    }
    return true;
}

/**
 * @brief 28相等性表达式 EqExp -> RelExp { ( '==' | '!=' ) RelExp }
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseEqExp(AstNode *root)
{
    EqExp *res = new EqExp(root);
    parseRelExp(res);
    while (token_stream[index].type == frontend::TokenType::EQL || token_stream[index].type == frontend::TokenType::NEQ)
    {
        new Term(token_stream[index++], res);
        parseRelExp(res);
    }
    return true;
}

/**
 * @brief 29逻辑与表达式 LAndExp -> EqExp [ '&&' LAndExp ]
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseLAndExp(AstNode *root)
{
    LAndExp *res = new LAndExp(root);
    parseEqExp(res);
    if (token_stream[index].type == frontend::TokenType::AND)
    {
        new Term(token_stream[index++], res);
        parseLAndExp(res);
    }
    return true;
}

// 30逻辑或表达式 LOrExp -> LAndExp [ '||' LOrExp ]
bool Parser::parseLOrExp(AstNode *root)
{
    LOrExp *res = new LOrExp(root);
    parseLAndExp(res);
    if (token_stream[index].type == frontend::TokenType::OR)
    {
        new Term(token_stream[index++], res);
        parseLOrExp(res);
    }
    return true;
}

/**
 * @brief 31常量表达式 ConstExp -> AddExp
 * @param root
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-17
 */
bool Parser::parseConstExp(AstNode *root)
{
    ConstExp *res = new ConstExp(root);
    parseAddExp(res);
    return true;
}

std::set<frontend::TokenType> Parser::matchCompUnit()
{ // CompUnit -> (Decl | FuncDef) [CompUnit]
    return {frontend::TokenType::CONSTTK, frontend::TokenType::INTTK,
            frontend::TokenType::FLOATTK, frontend::TokenType::VOIDTK};
}

std::set<frontend::TokenType> Parser::matchDecl()
{ // Decl -> ConstDecl | VarDecl
    return {frontend::TokenType::CONSTTK, frontend::TokenType::INTTK,
            frontend::TokenType::FLOATTK};
}

std::set<frontend::TokenType> Parser::matchConstDecl()
{ // ConstDecl->'const' BType ConstDef { ',' ConstDef } ';'
    return {frontend::TokenType::CONSTTK};
}

std::set<frontend::TokenType> Parser::matchBType()
{ // BType -> 'int' | 'float'
    return {frontend::TokenType::INTTK, frontend::TokenType::FLOATTK};
}

std::set<frontend::TokenType> Parser::matchConstDef()
{ // ConstDef -> Ident { '[' ConstExp ']' } '=' ConstInitVal
    return {frontend::TokenType::IDENFR};
}

std::set<frontend::TokenType> Parser::matchConstInitVal()
{ // ConstInitVal -> ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT, frontend::TokenType::LBRACE};
}

std::set<frontend::TokenType> Parser::matchVarDecl()
{ // VarDecl -> BType VarDef { ',' VarDef } ';'
    return {frontend::TokenType::INTTK, frontend::TokenType::FLOATTK};
}

std::set<frontend::TokenType> Parser::matchVarDef()
{ // VarDef -> Ident { '[' ConstExp ']' } [ '=' InitVal ]
    return {frontend::TokenType::IDENFR};
}

std::set<frontend::TokenType> Parser::matchInitVal()
{ // InitVal -> Exp | '{' [ InitVal { ',' InitVal } ] '}'
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT, frontend::TokenType::LBRACE};
}

std::set<frontend::TokenType> Parser::matchFuncDef()
{ // FuncDef -> FuncType Ident '(' [FuncFParams] ')' Block
    return {frontend::TokenType::VOIDTK, frontend::TokenType::INTTK,
            frontend::TokenType::FLOATTK};
}

std::set<frontend::TokenType> Parser::matchFuncType()
{ // FuncType -> 'void' | 'int' | 'float'
    return {frontend::TokenType::VOIDTK, frontend::TokenType::INTTK,
            frontend::TokenType::FLOATTK};
}

std::set<frontend::TokenType> Parser::matchFuncFParam()
{ // FuncFParam -> BType Ident [ '[' ']' { '[' Exp ']' } ]
    return {frontend::TokenType::INTTK, frontend::TokenType::FLOATTK};
}

std::set<frontend::TokenType> Parser::matchFuncFParams()
{ // FuncFParams -> FuncFParam { ',' FuncFParam }
    return {frontend::TokenType::INTTK, frontend::TokenType::FLOATTK};
}

std::set<frontend::TokenType> Parser::matchBlock()
{ // Block -> '{' { BlockItem } '}'
    return {frontend::TokenType::LBRACE};
}

std::set<frontend::TokenType> Parser::matchBlockItem()
{ // BlockItem -> Decl | Stmt
    // auto setDecl = matchDecl();
    return {frontend::TokenType::CONSTTK, frontend::TokenType::INTTK,
            frontend::TokenType::FLOATTK, frontend::TokenType::IDENFR,
            frontend::TokenType::IFTK, frontend::TokenType::WHILETK,
            frontend::TokenType::BREAKTK, frontend::TokenType::CONTINUETK,
            frontend::TokenType::RETURNTK, frontend::TokenType::SEMICN,
            frontend::TokenType::INTLTR, frontend::TokenType::FLOATLTR,
            frontend::TokenType::LPARENT, frontend::TokenType::PLUS,
            frontend::TokenType::MINU, frontend::TokenType::NOT,
            frontend::TokenType::LBRACE};
}

std::set<frontend::TokenType> Parser::matchStmt()
{ // Stmt -> LVal '=' Exp ';' | Block | 'if' '(' Cond ')' Stmt [ 'else' Stmt ] | 'while' '(' Cond ')' Stmt | 'break' ';' | 'continue' ';' | 'return' [Exp] ';' | [Exp] ';'
    return {frontend::TokenType::IDENFR, frontend::TokenType::LBRACE,
            frontend::TokenType::IFTK, frontend::TokenType::WHILETK,
            frontend::TokenType::BREAKTK, frontend::TokenType::CONTINUETK,
            frontend::TokenType::RETURNTK, frontend::TokenType::SEMICN,
            frontend::TokenType::INTLTR, frontend::TokenType::FLOATLTR,
            frontend::TokenType::LPARENT, frontend::TokenType::PLUS,
            frontend::TokenType::MINU, frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchExp()
{ // Exp -> AddExp
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchCond()
{ // Cond -> LOrExp
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchLVal()
{ // LVal -> Ident { '[' Exp ']' }
    return {frontend::TokenType::IDENFR};
}

std::set<frontend::TokenType> Parser::matchNumber()
{ // Number -> IntConst | floatConst
    return {frontend::TokenType::INTLTR, frontend::TokenType::FLOATLTR};
}

std::set<frontend::TokenType> Parser::matchPrimaryExp()
{ // PrimaryExp -> '(' Exp ')' | LVal | Number
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT};
}

std::set<frontend::TokenType> Parser::matchUnaryExp()
{ // UnaryExp -> PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchUnaryOp()
{ // UnaryOp -> '+' | '-' | '!'
    return {frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchFuncRParams()
{ // FuncRParams -> Exp { ',' Exp }
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchMulExp()
{ // MulExp -> UnaryExp { ( '*' | '/' | '%' ) UnaryExp }
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchAddExp()
{ // AddExp -> MulExp { ( '+' | '-' ) MulExp }
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchRelExp()
{ // RelExp -> AddExp { ( '<' | '>' | '<=' | '>=' ) AddExp }
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchEqExp()
{ // EqExp -> RelExp { ( '==' | '!=' ) RelExp }
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchLAndExp()
{ // LAndExp -> EqExp [ '&&' LAndExp ]
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchLOrExp()
{ // LOrExp -> LAndExp [ '||' LOrExp ]
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

std::set<frontend::TokenType> Parser::matchConstExp()
{ // ConstExp -> AddExp
    return {frontend::TokenType::IDENFR, frontend::TokenType::INTLTR,
            frontend::TokenType::FLOATLTR, frontend::TokenType::LPARENT,
            frontend::TokenType::PLUS, frontend::TokenType::MINU,
            frontend::TokenType::NOT};
}

void Parser::undo(int _lastIndex, AstNode *_res, int _curChildrenNums)
{
    index = _lastIndex;
    while (_res->children.size() > _curChildrenNums)
        _res->children.pop_back();
    return;
}
