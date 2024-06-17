/**
 * @file AST.h
 * @author Yuntao Dai (d1581209858@live.com)
 * @brief
 * abstract syntax tree
 * there is a basic class AstNode,
 * and for every non-terminal lexical unit, we create a sub-class for it,
 * sub-class should implement the IR generating function for itself
 * @version 0.1
 * @date 2022-12-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef AST_H
#define AST_H

#include "front/token.h"
#include "json/json.h"
#include "ir/ir.h"
using ir::Type;

#include <set>
#include <vector>
#include <string>
using std::string;
using std::vector;

namespace frontend
{

    // enumerate for node type
    enum class NodeType
    {
        TERMINAL, // terminal lexical unit
        COMPUINT,
        DECL,
        FUNCDEF,
        CONSTDECL,
        BTYPE,
        CONSTDEF,
        CONSTINITVAL,
        VARDECL,
        VARDEF,
        INITVAL,
        FUNCTYPE,
        FUNCFPARAM,
        FUNCFPARAMS,
        BLOCK,
        BLOCKITEM,
        STMT,
        EXP,
        COND,
        LVAL,
        NUMBER,
        PRIMARYEXP,
        UNARYEXP,
        UNARYOP,
        FUNCRPARAMS,
        MULEXP,
        ADDEXP,
        RELEXP,
        EQEXP,
        LANDEXP,
        LOREXP,
        CONSTEXP,
    };
    std::string toString(NodeType);

    /**
     * @brief 抽象语法树节点
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-17
     */
    struct AstNode
    {
        NodeType type;              // 节点类型
        AstNode *parent;            // 指向父节点的指针
        vector<AstNode *> children; // 指向子节点的指针数组

        AstNode(NodeType t, AstNode *p = nullptr);

        virtual ~AstNode();

        /**
         * @brief Get the json output object
         * @param root: a Json::Value buffer, should be initialized before calling this function
         */
        void get_json_output(Json::Value &root) const;

        AstNode(const AstNode &) = delete;
        AstNode &operator=(const AstNode &) = delete;
    };

    /**
     * @brief 终节点
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-17
     */
    struct Term : AstNode
    {
        // TODO; lab2todo21 struct Term
        string v;    // 传递字符串值
        Token token; // 终结点token
        Term(Token t, AstNode *p = nullptr);
    };

    struct CompUnit : AstNode
    {

        CompUnit(AstNode *p = nullptr);
    };

    struct Decl : AstNode
    {

        Decl(AstNode *p = nullptr);
    };

    struct FuncDef : AstNode
    {
        string n;
        Type t;

        FuncDef(AstNode *p = nullptr);
    };

    struct ConstDecl : AstNode
    {
        Type t;

        ConstDecl(AstNode *p = nullptr);
    };

    struct BType : AstNode
    {
        Type t; // 用于传递标识符类型

        BType(AstNode *p = nullptr);
    };

    struct ConstDef : AstNode
    {
        std::string arr_name;

        ConstDef(AstNode *p = nullptr);
    };

    struct ConstInitVal : AstNode
    {
        string v;
        Type t;

        ConstInitVal(AstNode *p = nullptr);
    };

    struct VarDecl : AstNode
    {
        Type t;

        VarDecl(AstNode *p = nullptr);
    };

    struct VarDef : AstNode
    {
        std::string arr_name; // 变量名

        VarDef(AstNode *p = nullptr);
    };

    struct InitVal : AstNode
    {
        string v; // 向下传递变量名
        Type t;   // 向下传递变量类型

        InitVal(AstNode *p = nullptr);
    };

    struct FuncType : AstNode
    {
        // TODO; lab2todo20 struct FuncType
        Type t; // 用于传递函数返回值类型
        FuncType(AstNode *p = nullptr);
    };

    struct FuncFParam : AstNode
    {

        FuncFParam(AstNode *p = nullptr);
    };

    struct FuncFParams : AstNode
    {

        FuncFParams(AstNode *p = nullptr);
    };

    struct Block : AstNode
    {

        Block(AstNode *p = nullptr);
    };

    struct BlockItem : AstNode
    {

        BlockItem(AstNode *p = nullptr);
    };

    struct Stmt : AstNode
    {
        // for while & break & continue, we need a vector to remember break & continue instruction
        std::set<ir::Instruction *> jump_eow; // jump to end of while
        std::set<ir::Instruction *> jump_bow; // jump to begin of while

        Stmt(AstNode *p = nullptr);
    };

    /**
     * @brief 表达式
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-16
     */
    struct Exp : AstNode
    {
        string v; // 记录重命名后的变量名/临时变量名/常数字符串
        Type t;   // 该表达式计算得到的类型

        Exp(AstNode *p = nullptr);
    };

    struct Cond : AstNode
    {
        string v;
        Type t;

        Cond(AstNode *p = nullptr);
    };

    /**
     * @brief 左值表达式节点
     * @note 左值是可寻址的变量，具有持久性，左值可以出现在赋值的左边和右边，右值只能出现在右边
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-17
     */
    struct LVal : AstNode
    {
        // TODO; lab2todo31 struct LVal
        string v;      // 左值表达式的值
        Type t;        // 左值表达式的类型
        bool isPtr;    // 左值是否是指针
        string offset; // array index, legal if t is IntPtr or FloatPtr

        LVal(AstNode *p = nullptr);
    };

    /**
     * @brief 数值节点
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-17
     */
    struct Number : AstNode
    {
        string v; // 数值的字面量
        Type t;   // 数值节点的类型 int/float

        Number(AstNode *p = nullptr);
    };

    /**
     * @brief 基本表达式节点
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-17
     */
    struct PrimaryExp : AstNode
    {
        string v;
        Type t;

        PrimaryExp(AstNode *p = nullptr);
    };

    /**
     * @brief 一元表达式
     * @note 一次函数调用也是一元表达式
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-17
     */
    struct UnaryExp : AstNode
    {
        string v;
        Type t;

        UnaryExp(AstNode *p = nullptr);
    };

    struct UnaryOp : AstNode
    {
        TokenType op; // 单目运算符类型

        UnaryOp(AstNode *p = nullptr);
    };

    struct FuncRParams : AstNode
    {

        FuncRParams(AstNode *p = nullptr);
    };

    struct MulExp : AstNode
    {
        string v;
        Type t;

        MulExp(AstNode *p = nullptr);
    };

    struct AddExp : AstNode
    {
        string v;
        Type t;

        AddExp(AstNode *p = nullptr);
    };

    struct RelExp : AstNode
    {
        string v;
        Type t = Type::Int;

        RelExp(AstNode *p = nullptr);
    };

    struct EqExp : AstNode
    {
        string v;
        Type t = Type::Int;

        EqExp(AstNode *p = nullptr);
    };

    struct LAndExp : AstNode
    {
        string v;
        Type t = Type::Int;

        LAndExp(AstNode *p = nullptr);
    };

    struct LOrExp : AstNode
    {
        string v;
        Type t = Type::Int;

        LOrExp(AstNode *p = nullptr);
    };

    struct ConstExp : AstNode
    {
        string v; // 数值，一定是非负整数
        Type t;

        ConstExp(AstNode *p = nullptr);
    };

} // namespace frontend

#endif