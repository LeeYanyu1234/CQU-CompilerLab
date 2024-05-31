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
        Token token; // 终结点token
        Term(Token t, AstNode *p = nullptr);
    };

    struct CompUnit : AstNode
    {
        /**
         * @brief constructor
         */
        CompUnit(AstNode *p = nullptr);
    };

    struct Decl : AstNode
    {
        /**
         * @brief constructor
         */
        Decl(AstNode *p = nullptr);
    };

    struct FuncDef : AstNode
    {
        string n;
        Type t;

        /**
         * @brief constructor
         */
        FuncDef(AstNode *p = nullptr);
    };

    struct ConstDecl : AstNode
    {
        Type t;

        /**
         * @brief constructor
         */
        ConstDecl(AstNode *p = nullptr);
    };

    struct BType : AstNode
    {
        Type t;

        /**
         * @brief constructor
         */
        BType(AstNode *p = nullptr);
    };

    struct ConstDef : AstNode
    {
        std::string arr_name;

        /**
         * @brief constructor
         */
        ConstDef(AstNode *p = nullptr);
    };

    struct ConstInitVal : AstNode
    {
        string v;
        Type t;

        /**
         * @brief constructor
         */
        ConstInitVal(AstNode *p = nullptr);
    };

    struct VarDecl : AstNode
    {
        Type t;

        /**
         * @brief constructor
         */
        VarDecl(AstNode *p = nullptr);
    };

    struct VarDef : AstNode
    {
        std::string arr_name;

        /**
         * @brief constructor
         */
        VarDef(AstNode *p = nullptr);
    };

    struct InitVal : AstNode
    {
        bool is_computable = false;
        string v;
        Type t;

        /**
         * @brief constructor
         */
        InitVal(AstNode *p = nullptr);
    };

    struct FuncType : AstNode
    {
        /**
         * @brief constructor
         */
        FuncType(AstNode *p = nullptr);
    };

    struct FuncFParam : AstNode
    {
        /**
         * @brief constructor
         */
        FuncFParam(AstNode *p = nullptr);
    };

    struct FuncFParams : AstNode
    {
        /**
         * @brief constructor
         */
        FuncFParams(AstNode *p = nullptr);
    };

    struct Block : AstNode
    {
        /**
         * @brief constructor
         */
        Block(AstNode *p = nullptr);
    };

    struct BlockItem : AstNode
    {
        /**
         * @brief constructor
         */
        BlockItem(AstNode *p = nullptr);
    };

    struct Stmt : AstNode
    {
        // for while & break & continue, we need a vector to remember break & continue instruction
        std::set<ir::Instruction *> jump_eow; // jump to end of while
        std::set<ir::Instruction *> jump_bow; // jump to begin of while

        /**
         * @brief constructor
         */
        Stmt(AstNode *p = nullptr);
    };

    /**
     * @brief 表达式
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-16
     */
    struct Exp : AstNode
    {
        bool is_computable = false; // 节点以下子树是否可以化简为常数，用于常数合并优化
        string v;                   // 记录重命名后的变量名/临时变量名/常数字符串
        Type t;                     // 该表达式计算得到的类型

        Exp(AstNode *p = nullptr);
    };

    struct Cond : AstNode
    {
        bool is_computable = false;
        string v;
        Type t;

        /**
         * @brief constructor
         */
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
        bool is_computable = false;
        string v; // 左值表达式的值
        Type t;   // 左值表达式的类型
        int i;    // array index, legal if t is IntPtr or FloatPtr

        LVal(AstNode *p = nullptr);
    };

    /**
     * @brief 数值节点
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-17
     */
    struct Number : AstNode
    {
        bool is_computable = true;
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
        bool is_computable = false;
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
        bool is_computable = false;
        string v;
        Type t;

        UnaryExp(AstNode *p = nullptr);
    };

    struct UnaryOp : AstNode
    {
        TokenType op;

        /**
         * @brief constructor
         */
        UnaryOp(AstNode *p = nullptr);
    };

    struct FuncRParams : AstNode
    {
        /**
         * @brief constructor
         */
        FuncRParams(AstNode *p = nullptr);
    };

    struct MulExp : AstNode
    {
        bool is_computable = false;
        string v;
        Type t;

        /**
         * @brief constructor
         */
        MulExp(AstNode *p = nullptr);
    };

    struct AddExp : AstNode
    {
        bool is_computable = false;
        string v;
        Type t;

        /**
         * @brief constructor
         */
        AddExp(AstNode *p = nullptr);
    };

    struct RelExp : AstNode
    {
        bool is_computable = false;
        string v;
        Type t = Type::Int;

        /**
         * @brief constructor
         */
        RelExp(AstNode *p = nullptr);
    };

    struct EqExp : AstNode
    {
        bool is_computable = false;
        string v;
        Type t = Type::Int;

        /**
         * @brief constructor
         */
        EqExp(AstNode *p = nullptr);
    };

    struct LAndExp : AstNode
    {
        bool is_computable = false;
        string v;
        Type t = Type::Int;

        /**
         * @brief constructor
         */
        LAndExp(AstNode *p = nullptr);
    };

    struct LOrExp : AstNode
    {
        bool is_computable = false;
        string v;
        Type t = Type::Int;

        /**
         * @brief constructor
         */
        LOrExp(AstNode *p = nullptr);
    };

    struct ConstExp : AstNode
    {
        bool is_computable = true;
        string v;
        Type t;

        /**
         * @brief constructor
         */
        ConstExp(AstNode *p = nullptr);
    };

} // namespace frontend

#endif