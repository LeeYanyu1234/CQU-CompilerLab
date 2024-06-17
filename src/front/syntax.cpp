#include "front/syntax.h"
#include <iostream>
#include <cassert>

using frontend::Parser;

// #define DEBUG_PARSER
#define TODO assert(0 && "todo")
#define CUR_TOKEN_IS(tk_type) (token_stream[index].type == TokenType::tk_type)
#define PARSE_TOKEN(tk_type) root->children.push_back(parseTerm(root, TokenType::tk_type))
#define PARSE(name, type)       \
    auto name = new type(root); \
    assert(parse##type(name));  \
    root->children.push_back(name);

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
// 1 编译单元 CompUnit -> (Decl | FuncDef) [CompUnit]
frontend::CompUnit *Parser::parseCompUnit(AstNode *root)
{
    CompUnit *res = new CompUnit(root); // 构造根节点

    bool isDecl = false;                        // 标记Decl分支
    bool isFuncDef = false;                     // 标记FuncDef分支
    int lastIndex = index;                      // 记录当前index
    int curChildrenNums = res->children.size(); // 记录进入分支前的children数量

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

// 2 声明 Decl -> ConstDecl | VarDecl
bool Parser::parseDecl(AstNode *root)
{
    Decl *res = new Decl(root);

    bool isConstDecl = false;
    bool isVarDecl = false;
    int lastIndex = index;
    int curChildrenNums = res->children.size();

    if (matchConstDecl().count(token_stream[index].type)) // 匹配ConstDecl
    {
        isConstDecl = parseConstDecl(res);
        if (!isConstDecl)
            undo(lastIndex, res, curChildrenNums);
    }

    if (!isConstDecl && matchVarDecl().count(token_stream[index].type)) // 匹配VarDecl
        isVarDecl = parseVarDecl(res);

    if (!isConstDecl && !isVarDecl)
        return false;

    return true;
}

// 3 常量声明 ConstDecl -> 'const' BType ConstDef { ',' ConstDef } ';'
bool Parser::parseConstDecl(AstNode *root)
{
    ConstDecl *res = new ConstDecl(root);

    if (token_stream[index].type != frontend::TokenType::CONSTTK) // 匹配const
        return false;
    new Term(token_stream[index++], res);

    if (!parseBType(res)) // 匹配BType
        return false;

    if (!parseConstDef(res)) // 匹配ConstDef
        return false;

    while (token_stream[index].type == frontend::TokenType::COMMA) // 匹配{ ',' ConstDef }
    {
        new Term(token_stream[index++], res);
        if (!parseConstDef(res))
            return false;
    }

    if (token_stream[index].type != frontend::TokenType::SEMICN) // 匹配';'
        return false;
    new Term(token_stream[index++], res);

    return true;
}

// 4 基本类型 BType -> 'int' | 'float'
bool Parser::parseBType(AstNode *root)
{
    BType *res = new BType(root);

    if (token_stream[index].type == frontend::TokenType::INTTK) // 匹配'int'
        new Term(token_stream[index++], res);
    else if (token_stream[index].type == frontend::TokenType::FLOATTK) // 匹配'float'
        new Term(token_stream[index++], res);
    else
        return false;

    return true;
}

// 5 常量定义 ConstDef -> Ident { '[' ConstExp ']' } '=' ConstInitVal
bool Parser::parseConstDef(AstNode *root)
{
    ConstDef *res = new ConstDef(root);

    if (token_stream[index].type != frontend::TokenType::IDENFR) // 匹配Ident
        return false;
    new Term(token_stream[index++], res);

    while (token_stream[index].type == frontend::TokenType::LBRACK) // 匹配{ '[' ConstExp ']' }
    {
        new Term(token_stream[index++], res);
        if (!parseConstExp(res))
            return false;
        if (token_stream[index].type != frontend::TokenType::RBRACK)
            return false;
        new Term(token_stream[index++], res);
    }

    if (token_stream[index].type != frontend::TokenType::ASSIGN) // 匹配'='
        return false;
    new Term(token_stream[index++], res);

    if (!parseConstInitVal(res)) // 匹配ConstInitVal
        return false;

    return true;
}

// 6 常量初值 ConstInitVal -> ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
bool Parser::parseConstInitVal(AstNode *root)
{
    ConstInitVal *res = new ConstInitVal(root);

    bool isConstExp = false;
    bool isLBrace = false;
    int lastIndex = index;
    int curChildrenNums = res->children.size();

    if (matchConstExp().count(token_stream[index].type)) // 匹配ConstExp
    {
        isConstExp = parseConstExp(res);
        if (!isConstExp)
            undo(lastIndex, res, curChildrenNums);
    }

    if (!isConstExp && token_stream[index].type == frontend::TokenType::LBRACE) // 匹配'{' [ ConstInitVal { ',' ConstInitVal } ] '}'
    {
        new Term(token_stream[index++], res);
        if (matchConstInitVal().count(token_stream[index].type))
        {
            if (!parseConstInitVal(res))
                return false;

            while (token_stream[index].type == frontend::TokenType::COMMA)
            {
                new Term(token_stream[index++], res);
                if (!parseConstInitVal(res))
                    return false;
            }
        }
        if (token_stream[index].type != frontend::TokenType::RBRACE)
            return false;
        new Term(token_stream[index++], res);

        isLBrace = true;
    }

    if (!isConstExp && !isLBrace)
        return false;

    return true;
}

// 7 变量声明 VarDecl -> BType VarDef { ',' VarDef } ';'
bool Parser::parseVarDecl(AstNode *root)
{
    VarDecl *res = new VarDecl(root);

    if (!parseBType(res)) // 匹配BType
        return false;

    if (!parseVarDef(res)) // 匹配VarDef
        return false;

    while (token_stream[index].type == frontend::TokenType::COMMA) // 匹配{ ',' VarDef }
    {
        new Term(token_stream[index++], res);
        if (!parseVarDef(res))
            return false;
    }

    if (token_stream[index].type != frontend::TokenType::SEMICN) // 匹配';'
        return false;
    new Term(token_stream[index++], res);

    return true;
}

// 8 变量定义 VarDef -> Ident { '[' ConstExp ']' } [ '=' InitVal ]
bool Parser::parseVarDef(AstNode *root)
{
    VarDef *res = new VarDef(root);

    if (token_stream[index].type != frontend::TokenType::IDENFR) // 匹配Ident
        return false;
    new Term(token_stream[index++], res);

    while (token_stream[index].type == frontend::TokenType::LBRACK) // 匹配{ '[' ConstExp ']' }
    {
        new Term(token_stream[index++], res);
        if (!parseConstExp(res))
            return false;

        if (token_stream[index].type != frontend::TokenType::RBRACK)
            return false;
        new Term(token_stream[index++], res);
    }

    if (token_stream[index].type == frontend::TokenType::ASSIGN) // 匹配[ '=' InitVal ]
    {
        new Term(token_stream[index++], res);
        if (!parseInitVal(res))
            return false;
    }

    return true;
}

// 9 变量初值 InitVal -> Exp | '{' [ InitVal { ',' InitVal } ] '}'
bool Parser::parseInitVal(AstNode *root)
{
    InitVal *res = new InitVal(root);

    bool isExp = false;
    bool isLBrace = false;
    int lastIndex = index;
    int curChildrenNums = res->children.size();

    if (matchExp().count(token_stream[index].type)) // 匹配Exp
    {
        isExp = parseExp(res);
        if (!isExp)
            undo(lastIndex, res, curChildrenNums);
    }

    if (!isExp && token_stream[index].type == frontend::TokenType::LBRACE) // 匹配'{' [ InitVal { ',' InitVal } ] '}'
    {
        new Term(token_stream[index++], res);

        if (matchInitVal().count(token_stream[index].type))
        {
            if (!parseInitVal(res))
                return false;

            while (token_stream[index].type == frontend::TokenType::COMMA)
            {
                new Term(token_stream[index++], res);

                if (!parseInitVal(res))
                    return false;
            }
        }

        if (token_stream[index].type != frontend::TokenType::RBRACE)
            return false;
        new Term(token_stream[index++], res);

        isLBrace = true;
    }

    if (!isExp && !isLBrace)
        return false;

    return true;
}

// 10函数定义 FuncDef -> FuncType Ident '(' [FuncFParams] ')' Block
bool Parser::parseFuncDef(AstNode *root)
{
    FuncDef *res = new FuncDef(root);

    if (!parseFuncType(res)) // 匹配FuncType
        return false;

    if (token_stream[index].type != frontend::TokenType::IDENFR) // 匹配Ident
        return false;
    new Term(token_stream[index++], res);

    if (token_stream[index].type != frontend::TokenType::LPARENT) // 匹配'('
        return false;
    new Term(token_stream[index++], res);

    if (matchFuncFParams().count(token_stream[index].type)) // 匹配[FuncFParams]
        if (!parseFuncFParams(res))
            return false;

    if (token_stream[index].type != frontend::TokenType::RPARENT) // 匹配')'
        return false;
    new Term(token_stream[index++], res);

    if (!parseBlock(res)) // 匹配Block
        return false;

    return true;
}

// 11函数类型 FuncType -> 'void' | 'int' | 'float'
bool Parser::parseFuncType(AstNode *root)
{
    FuncType *res = new FuncType(root);

    if (token_stream[index].type == frontend::TokenType::VOIDTK)
        new Term(token_stream[index++], res);
    else if (token_stream[index].type == frontend::TokenType::INTTK)
        new Term(token_stream[index++], res);
    else if (token_stream[index].type == frontend::TokenType::FLOATTK)
        new Term(token_stream[index++], res);
    else
        return false;

    return true;
}

// 12函数形参 FuncFParam -> BType Ident [ '[' ']' { '[' Exp ']' } ]
bool Parser::parseFuncFParam(AstNode *root)
{
    FuncFParam *res = new FuncFParam(root);

    if (!parseBType(res)) // 匹配BType
        return false;

    if (token_stream[index].type != frontend::TokenType::IDENFR) // 匹配Ident
        return false;
    new Term(token_stream[index++], res);

    if (token_stream[index].type == frontend::TokenType::LBRACK) // 匹配[ '[' ']' { '[' Exp ']' } ]
    {
        new Term(token_stream[index++], res);

        if (token_stream[index].type != frontend::TokenType::RBRACK)
            return false;
        new Term(token_stream[index++], res);

        while (token_stream[index].type == frontend::TokenType::LBRACK)
        {
            new Term(token_stream[index++], res);

            if (!parseExp(res))
                return false;

            if (token_stream[index].type != frontend::TokenType::RBRACK)
                return false;
            new Term(token_stream[index++], res);
        }
    }

    return true;
}

// 13函数形参表 FuncFParams -> FuncFParam { ',' FuncFParam }
bool Parser::parseFuncFParams(AstNode *root)
{
    FuncFParams *res = new FuncFParams(root);

    if (!parseFuncFParam(res)) // 匹配FuncFParam
        return false;

    while (token_stream[index].type == frontend::TokenType::COMMA) // 匹配{ ',' FuncFParam }
    {
        new Term(token_stream[index++], res);

        if (!parseFuncFParam(res))
            return false;
    }

    return true;
}

// 14语句块 Block -> '{' { BlockItem } '}'
bool Parser::parseBlock(AstNode *root)
{
    Block *res = new Block(root);

    if (token_stream[index].type != frontend::TokenType::LBRACE) // 匹配'{'
        return false;
    new Term(token_stream[index++], res);

    while (matchBlockItem().count(token_stream[index].type)) // 匹配{ BlockItem }
        if (!parseBlockItem(res))
            return false;

    if (token_stream[index].type != frontend::TokenType::RBRACE) // 匹配'}'
        return false;
    new Term(token_stream[index++], res);

    return true;
}

// 15语句块项 BlockItem -> Decl | Stmt
bool Parser::parseBlockItem(AstNode *root)
{
    BlockItem *res = new BlockItem(root);
    bool isDecl = false;
    bool isStmt = false;
    int lastIndex = index;
    int curChildrenNums = res->children.size();

    if (matchDecl().count(token_stream[index].type)) // 匹配Decl
    {
        isDecl = parseDecl(res);
        if (!isDecl)
            undo(lastIndex, res, curChildrenNums);
    }

    if (!isDecl && matchStmt().count(token_stream[index].type)) // 匹配Stmt
        isStmt = parseStmt(res);

    if (!isDecl && !isStmt)
        return false;

    return true;
}

// 16语句 Stmt -> LVal '=' Exp ';' | Block | 'if' '(' Cond ')' Stmt [ 'else' Stmt ] | 'while' '(' Cond ')' Stmt | 'break' ';' | 'continue' ';' | 'return' [Exp] ';' | [Exp] ';'
bool Parser::parseStmt(AstNode *root)
{
    Stmt *res = new Stmt(root);

    bool isLVal = false;
    bool isBlock = false;
    bool isKeyword = false; // 匹配'if'、'while'、'break'、'continue'、'return'
    bool isExp = false;
    bool isSemicn = false;
    int lastIndex = index;
    int curChildrenNums = res->children.size();

    if (matchLVal().count(token_stream[index].type)) // 匹配LVal '=' Exp ';'
    {
        isLVal = parseLVal(res);

        if (!isLVal)
        {
            undo(lastIndex, res, curChildrenNums);
            goto BlockCase;
        }

        if (token_stream[index].type != frontend::TokenType::ASSIGN)
        {
            isLVal = false;
            undo(lastIndex, res, curChildrenNums);
            goto BlockCase;
        }
        new Term(token_stream[index++], res);

        if (!parseExp(res))
        { // Exp
            isLVal = false;
            undo(lastIndex, res, curChildrenNums);
            goto BlockCase;
        }

        if (token_stream[index].type != frontend::TokenType::SEMICN)
        {
            isLVal = false;
            undo(lastIndex, res, curChildrenNums);
            goto BlockCase;
        }
        new Term(token_stream[index++], res);

        return true;
    }

BlockCase:
    if (!isLVal && matchBlock().count(token_stream[index].type)) // 匹配Block
    {
        isBlock = parseBlock(res);
        if (!isBlock)
            undo(lastIndex, res, curChildrenNums);
        else
            return true;
    }

    if (token_stream[index].type == frontend::TokenType::IFTK) // 匹配'if' '(' Cond ')' Stmt [ 'else' Stmt ]
    {
        new Term(token_stream[index++], res);

        if (token_stream[index].type != frontend::TokenType::LPARENT)
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }
        new Term(token_stream[index++], res);

        if (!parseCond(res))
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }

        if (token_stream[index].type != frontend::TokenType::RPARENT)
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }
        new Term(token_stream[index++], res);

        if (!parseStmt(res))
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }

        if (token_stream[index].type == frontend::TokenType::ELSETK)
        {
            new Term(token_stream[index++], res);
            if (!parseStmt(res))
            {
                isKeyword = false;
                undo(lastIndex, res, curChildrenNums);
                goto ExpCase;
            }
        }
        return true;
    }
    else if (token_stream[index].type == frontend::TokenType::WHILETK) // 匹配'while' '(' Cond ')' Stmt
    {
        new Term(token_stream[index++], res);

        if (token_stream[index].type != frontend::TokenType::LPARENT)
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }
        new Term(token_stream[index++], res);

        if (!parseCond(res))
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }

        if (token_stream[index].type != frontend::TokenType::RPARENT)
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }
        new Term(token_stream[index++], res);

        if (!parseStmt(res))
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }

        return true;
    }
    else if (token_stream[index].type == frontend::TokenType::BREAKTK) // 匹配'break' ';'
    {
        new Term(token_stream[index++], res);

        if (token_stream[index].type != frontend::TokenType::SEMICN)
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }
        new Term(token_stream[index++], res);

        return true;
    }
    else if (token_stream[index].type == frontend::TokenType::CONTINUETK) // 匹配'continue' ';'
    {
        new Term(token_stream[index++], res);

        if (token_stream[index].type != frontend::TokenType::SEMICN)
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }
        new Term(token_stream[index++], res);

        return true;
    }
    else if (token_stream[index].type == frontend::TokenType::RETURNTK) // 匹配'return' [Exp] ';'
    {
        new Term(token_stream[index++], res);

        if (matchExp().count(token_stream[index].type))
            parseExp(res);

        if (token_stream[index].type != frontend::TokenType::SEMICN)
        {
            isKeyword = false;
            undo(lastIndex, res, curChildrenNums);
            goto ExpCase;
        }
        new Term(token_stream[index++], res);

        return true;
    }

ExpCase:
    if (!isLVal && !isBlock && !isKeyword && matchExp().count(token_stream[index].type)) // 匹配[Exp] ';'
    {
        isExp = parseExp(res);

        if (isExp)
        {
            if (token_stream[index].type == frontend::TokenType::SEMICN)
            {
                new Term(token_stream[index++], res);
                return true;
            }
            else
            {
                isExp = false;
                undo(lastIndex, res, curChildrenNums);
                goto SEMICNCase;
            }
        }
        else
        {
            isExp = false;
            undo(lastIndex, res, curChildrenNums);
            goto SEMICNCase;
        }
    }

SEMICNCase:
    if (!isLVal && !isBlock && !isKeyword && !isExp && token_stream[index].type == frontend::TokenType::SEMICN) // 匹配';'
    {
        new Term(token_stream[index++], res);
        return true;
    }

    if (!isLVal && !isBlock && !isKeyword && !isExp && !isSemicn)
        return false;

    return true;
}

// 17表达式 Exp -> AddExp
bool Parser::parseExp(AstNode *root)
{
    Exp *res = new Exp(root);

    if (!parseAddExp(res))
        return false;

    return true;
}

// 18条件表达式 Cond -> LOrExp
bool Parser::parseCond(AstNode *root)
{
    Cond *res = new Cond(root);

    if (!parseLOrExp(res))
        return false;

    return true;
}

// 19左值表达式 LVal -> Ident { '[' Exp ']' }
bool Parser::parseLVal(AstNode *root)
{
    LVal *res = new LVal(root);

    if (token_stream[index].type != frontend::TokenType::IDENFR) // 匹配Ident
        return false;
    new Term(token_stream[index++], res);

    while (token_stream[index].type == frontend::TokenType::LBRACK) // 匹配{ '[' Exp ']' }
    {
        new Term(token_stream[index++], res);

        if (!parseExp(res))
            return false;

        if (token_stream[index].type != frontend::TokenType::RBRACK)
            return false;
        new Term(token_stream[index++], res);
    }

    return true;
}

// 20数值 Number -> IntConst | floatConst
bool Parser::parseNumber(AstNode *root)
{
    Number *res = new Number(root);

    if (token_stream[index].type != frontend::TokenType::INTLTR && token_stream[index].type != frontend::TokenType::FLOATLTR)
        return false;
    new Term(token_stream[index++], res);

    return true;
}

// 21基本表达式 PrimaryExp -> '(' Exp ')' | LVal | Number
bool Parser::parsePrimaryExp(AstNode *root)
{
    PrimaryExp *res = new PrimaryExp(root);

    bool isLParent = false;
    bool isLVal = false;
    bool isNumber = false;
    int lastIndex = index;
    int curChildrenNums = res->children.size();

    if (token_stream[index].type == frontend::TokenType::LPARENT) // 匹配'(' Exp ')'
    {
        new Term(token_stream[index++], res);

        isLParent = parseExp(res);
        if (isLParent && token_stream[index].type == frontend::TokenType::RPARENT)
        {
            new Term(token_stream[index++], res); // ')'
            return true;
        }
        else
        {
            isLParent = false;
            undo(lastIndex, res, curChildrenNums);
        }
    }

    if (!isLParent && matchLVal().count(token_stream[index].type)) // 匹配LVal
    {
        isLVal = parseLVal(res);
        if (!isLVal)
            undo(lastIndex, res, curChildrenNums);
        else
            return true;
    }

    if (!isLParent && !isLVal && matchNumber().count(token_stream[index].type)) // 匹配Number
    {
        isNumber = parseNumber(res);
        if (!isNumber)
            undo(lastIndex, res, curChildrenNums);
        else
            return true;
    }

    if (!isLParent && !isLVal && !isNumber)
        return false;

    return true;
}

// 22一元表达式 UnaryExp -> PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
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

    bool isPrimaryExp = false;
    bool isIdent = false;
    bool isUnaryOp = false;
    int lastIndex = index;
    int curChildrenNums = res->children.size();

    if (token_stream[index].type == frontend::TokenType::IDENFR) // 匹配Ident '(' [FuncRParams] ')'
    {
        new Term(token_stream[index++], res);
        isIdent = true;

        if (token_stream[index].type != frontend::TokenType::LPARENT)
        {
            isIdent = false;
            undo(lastIndex, res, curChildrenNums);
            goto PrimaryExpCase;
        }
        new Term(token_stream[index++], res);

        if (matchFuncRParams().count(token_stream[index].type))
            parseFuncRParams(res);

        if (token_stream[index].type != frontend::TokenType::RPARENT)
        {
            isIdent = false;
            undo(lastIndex, res, curChildrenNums);
            goto PrimaryExpCase;
        }
        new Term(token_stream[index++], res);

        return true;
    }

PrimaryExpCase:
    if (!isIdent && matchPrimaryExp().count(token_stream[index].type)) // 匹配PrimaryExp
    {
        isPrimaryExp = parsePrimaryExp(res);
        if (!isPrimaryExp)
            undo(lastIndex, res, curChildrenNums);
        else
            return true;
    }

UnaryOpCase:
    if (!isPrimaryExp && !isIdent && matchUnaryOp().count(token_stream[index].type)) // 匹配UnaryOp UnaryExp
    {
        isUnaryOp = parseUnaryOp(res);
        if (!isUnaryOp)
        {
            undo(lastIndex, res, curChildrenNums);
            return false;
        }

        if (!parseUnaryExp(res))
        {
            isUnaryOp = false;
            undo(lastIndex, res, curChildrenNums);
            return false;
        }
    }

    if (!isPrimaryExp && !isIdent && !isUnaryOp)
        return false;

    return true;
}

// 23单目运算符 UnaryOp -> '+' | '-' | '!'
bool Parser::parseUnaryOp(AstNode *root)
{
    UnaryOp *res = new UnaryOp(root);

    if (token_stream[index].type != frontend::TokenType::PLUS && token_stream[index].type != frontend::TokenType::MINU && token_stream[index].type != frontend::TokenType::NOT)
        return false;
    new Term(token_stream[index++], res);

    return true;
}

// 24函数实参表 FuncRParams -> Exp { ',' Exp }
bool Parser::parseFuncRParams(AstNode *root)
{
    FuncRParams *res = new FuncRParams(root);

    if (!parseExp(res))
        return false;

    while (token_stream[index].type == frontend::TokenType::COMMA)
    {
        new Term(token_stream[index++], res);
        if (!parseExp(res))
            return false;
    }

    return true;
}

// 25乘除模表达式 MulExp -> UnaryExp { ( '*' | '/' | '%' ) UnaryExp }
bool Parser::parseMulExp(AstNode *root)
{
    MulExp *res = new MulExp(root);

    if (!parseUnaryExp(res))
        return false;

    while (token_stream[index].type == frontend::TokenType::MULT || token_stream[index].type == frontend::TokenType::DIV || token_stream[index].type == frontend::TokenType::MOD)
    {
        new Term(token_stream[index++], res);

        if (!parseUnaryExp(res))
            return false;
    }

    return true;
}

// 26加减表达式 AddExp -> MulExp { ( '+' | '-' ) MulExp }
bool Parser::parseAddExp(AstNode *root)
{
    AddExp *res = new AddExp(root);

    if (!parseMulExp(res))
        return false;

    while (token_stream[index].type == frontend::TokenType::PLUS || token_stream[index].type == frontend::TokenType::MINU)
    {
        new Term(token_stream[index++], res);
        if (!parseMulExp(res))
            return false;
    }

    return true;
}

// 27关系表达式 RelExp -> AddExp { ( '<' | '>' | '<=' | '>=' ) AddExp }
bool Parser::parseRelExp(AstNode *root)
{
    RelExp *res = new RelExp(root);

    if (!parseAddExp(res))
        return false;

    while (token_stream[index].type == frontend::TokenType::LSS || token_stream[index].type == frontend::TokenType::GTR || token_stream[index].type == frontend::TokenType::LEQ || token_stream[index].type == frontend::TokenType::GEQ)
    {
        new Term(token_stream[index++], res);

        if (!parseAddExp(res))
            return false;
    }

    return true;
}

// 28相等性表达式 EqExp -> RelExp { ( '==' | '!=' ) RelExp }
bool Parser::parseEqExp(AstNode *root)
{
    EqExp *res = new EqExp(root);

    if (!parseRelExp(res))
        return false;

    while (token_stream[index].type == frontend::TokenType::EQL || token_stream[index].type == frontend::TokenType::NEQ)
    {
        new Term(token_stream[index++], res);
        if (!parseRelExp(res))
            return false;
    }

    return true;
}

// 29逻辑与表达式 LAndExp -> EqExp [ '&&' LAndExp ]
bool Parser::parseLAndExp(AstNode *root)
{
    LAndExp *res = new LAndExp(root);

    if (!parseEqExp(res))
        return false;
    else if (token_stream[index].type == frontend::TokenType::AND)
    {
        new Term(token_stream[index++], res);

        if (!parseLAndExp(res))
            return false;
    }

    return true;
}

// 30逻辑或表达式 LOrExp -> LAndExp [ '||' LOrExp ]
bool Parser::parseLOrExp(AstNode *root)
{
    LOrExp *res = new LOrExp(root);

    if (!parseLAndExp(res))
        return false;
    else if (token_stream[index].type == frontend::TokenType::OR)
    {
        new Term(token_stream[index++], res);

        if (!parseLOrExp(res))
            return false;
    }

    return true;
}

// 31常量表达式 ConstExp -> AddExp
bool Parser::parseConstExp(AstNode *root)
{
    ConstExp *res = new ConstExp(root);

    if (!parseAddExp(res))
        return false;
    else
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
