/**
 * @name 语法分析器
 * @file syntax.h
 * @author Yuntao Dai (d1581209858@live.com)
 * @brief
 * in the second part, we already has a token stream, now we should analysis it and result in a syntax tree,
 * which we also called it AST(abstract syntax tree)
 * @version 0.1
 * @date 2022-12-15
 * @copyright Copyright (c) 2022
 */

#ifndef SYNTAX_H
#define SYNTAX_H

#include "front/abstract_syntax_tree.h"
#include "front/token.h"

#include <vector>
#include <set>

namespace frontend
{
    // definition of Parser
    // 语法分析器接受token输入，输出抽象语法树
    struct Parser
    {
        uint32_t index; // current token index
        const std::vector<Token> &token_stream;

        /**
         * @brief constructor
         * @param tokens: the input token_stream
         */
        Parser(const std::vector<Token> &tokens);

        ~Parser();

        /**
         * @brief creat the abstract syntax tree
         * @return the root of abstract syntax tree
         */
        CompUnit *get_abstract_syntax_tree();

        // TODO; lab1todo8
        // 添加相关函数的声明，parseCompUnit函数创建根节点，直接对接get_abstract_syntax_tree函数，返回值为*AstNode
        // 其余递归下降法函数返回值都是bool类型
        CompUnit *parseCompUnit(AstNode *root);
        bool parseDecl(AstNode *root);
        bool parseConstDecl(AstNode *root);
        bool parseBType(AstNode *root);
        bool parseConstDef(AstNode *root);
        bool parseConstInitVal(AstNode *root);
        bool parseVarDecl(AstNode *root);
        bool parseVarDef(AstNode *root);
        bool parseInitVal(AstNode *root);
        bool parseFuncDef(AstNode *root);
        bool parseFuncType(AstNode *root);
        bool parseFuncFParam(AstNode *root);
        bool parseFuncFParams(AstNode *root);
        bool parseBlock(AstNode *root);
        bool parseBlockItem(AstNode *root);
        bool parseStmt(AstNode *root);
        bool parseExp(AstNode *root);
        bool parseCond(AstNode *root);
        bool parseLVal(AstNode *root);
        bool parseNumber(AstNode *root);
        bool parsePrimaryExp(AstNode *root);
        bool parseUnaryExp(AstNode *root);
        bool parseUnaryOp(AstNode *root);
        bool parseFuncRParams(AstNode *root);
        bool parseMulExp(AstNode *root);
        bool parseAddExp(AstNode *root);
        bool parseRelExp(AstNode *root);
        bool parseEqExp(AstNode *root);
        bool parseLAndExp(AstNode *root);
        bool parseLOrExp(AstNode *root);
        bool parseConstExp(AstNode *root);
        // CompUnit *parseCompUnit(CompUnit *root);
        // bool parseDecl(Decl *root);
        // bool parseConstDecl(ConstDecl *root);
        // bool parseBType(BType *root);
        // bool parseConstDef(ConstDef *root);
        // bool parseConstInitVal(ConstInitVal *root);
        // bool parseVarDecl(VarDecl *root);
        // bool parseVarDef(VarDef *root);
        // bool parseInitVal(InitVal *root);
        // bool parseFuncDef(FuncDef *root);
        // bool parseFuncType(FuncType *root);
        // bool parseFuncFParam(FuncFParam *root);
        // bool parseFuncFParams(FuncFParams *root);
        // bool parseBlock(Block *root);
        // bool parseBlockItem(BlockItem *root);
        // bool parseStmt(Stmt *root);
        // bool parseExp(Exp *root);
        // bool parseCond(Cond *root);
        // bool parseLVal(LVal *root);
        // bool parseNumber(Number *root);
        // bool parsePrimaryExp(PrimaryExp *root);
        // bool parseUnaryExp(UnaryExp *root);
        // bool parseUnaryOp(UnaryOp *root);
        // bool parseFuncRParams(FuncRParams *root);
        // bool parseMulExp(MulExp *root);
        // bool parseAddExp(AddExp *root);
        // bool parseRelExp(RelExp *root);
        // bool parseEqExp(EqExp *root);
        // bool parseLAndExp(LAndExp *root);
        // bool parseLOrExp(LOrExp *root);
        // bool parseConstExp(ConstExp *root);

        // 分支判断函数，判断是否存在这一语法分支
        std::set<frontend::TokenType> matchCompUnit();
        std::set<frontend::TokenType> matchDecl();
        std::set<frontend::TokenType> matchConstDecl();
        std::set<frontend::TokenType> matchBType();
        std::set<frontend::TokenType> matchConstDef();
        std::set<frontend::TokenType> matchConstInitVal();
        std::set<frontend::TokenType> matchVarDecl();
        std::set<frontend::TokenType> matchVarDef();
        std::set<frontend::TokenType> matchInitVal();
        std::set<frontend::TokenType> matchFuncDef();
        std::set<frontend::TokenType> matchFuncType();
        std::set<frontend::TokenType> matchFuncFParam();
        std::set<frontend::TokenType> matchFuncFParams();
        std::set<frontend::TokenType> matchBlock();
        std::set<frontend::TokenType> matchBlockItem();
        std::set<frontend::TokenType> matchStmt();
        std::set<frontend::TokenType> matchExp();
        std::set<frontend::TokenType> matchCond();
        std::set<frontend::TokenType> matchLVal();
        std::set<frontend::TokenType> matchNumber();
        std::set<frontend::TokenType> matchPrimaryExp();
        std::set<frontend::TokenType> matchUnaryExp();
        std::set<frontend::TokenType> matchUnaryOp();
        std::set<frontend::TokenType> matchFuncRParams();
        std::set<frontend::TokenType> matchMulExp();
        std::set<frontend::TokenType> matchAddExp();
        std::set<frontend::TokenType> matchRelExp();
        std::set<frontend::TokenType> matchEqExp();
        std::set<frontend::TokenType> matchLAndExp();
        std::set<frontend::TokenType> matchLOrExp();
        std::set<frontend::TokenType> matchConstExp();

        void undo(int _lastIndex, AstNode *_res, int _curChildrenNums); // 撤销函数，撤销匹配

        /**
         * @brief for debug, should be called in the beginning of recursive descent functions
         * @param node: current parsing node
         */
        void log(AstNode *node);
    };

} // namespace frontend

#endif