/**
 * @file semantic.cpp
 * @brief 语义分析器相关函数实现
 * @author LeeYanyu1234 (343820386@qq.com)
 * @version 1.0.1
 * @date 2024-05-31
 *
 * @copyright Copyright (c) 2024 Chongqing University
 *
 */
#include "front/semantic.h"

#include <cassert>

using ir::Function;
using ir::Instruction;
using ir::Operand;
using ir::Operator;

#define TODO assert(0 && "TODO");

/**
 * @brief 匹配节点类型
 * @param node 节点类型
 * @param index 根节点上的索引
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
#define MATCH_NODE_TYPE(node, index) root->children[index]->type == node

/**
 * @brief 复制表达式节点属性
 * @param from 复制对象
 * @param to 粘贴对象
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
#define COPY_EXP_NODE(from, to)              \
    to->is_computable = from->is_computable; \
    to->v = from->v;                         \
    to->t = from->t;

/**
 * @brief 获取节点指针
 * @param type 节点类型
 * @param node 指针名
 * @param index 根节点上的索引
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
#define GET_NODE_PTR(type, node, index)                      \
    auto node = dynamic_cast<type *>(root->children[index]); \
    assert(node);

/**
 * @brief 分析节点
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
#define ANALYZE(type, index, buffer)                         \
    auto node = dynamic_cast<type *>(root->children[index]); \
    assert(node);                                            \
    analyze##type(node, buffer);

/**
 * @brief 建立运行时库
 * @return map<std::string, ir::Function *>*
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
map<std::string, ir::Function *> *frontend::get_lib_funcs()
{
    static map<std::string, ir::Function *> lib_funcs = {
        {"getint", new Function("getint", Type::Int)},
        {"getch", new Function("getch", Type::Int)},
        {"getfloat", new Function("getfloat", Type::Float)},
        {"getarray", new Function("getarray", {Operand("arr", Type::IntPtr)}, Type::Int)},
        {"getfarray", new Function("getfarray", {Operand("arr", Type::FloatPtr)}, Type::Int)},
        {"putint", new Function("putint", {Operand("i", Type::Int)}, Type::null)},
        {"putch", new Function("putch", {Operand("i", Type::Int)}, Type::null)},
        {"putfloat", new Function("putfloat", {Operand("f", Type::Float)}, Type::null)},
        {"putarray", new Function("putarray", {Operand("n", Type::Int), Operand("arr", Type::IntPtr)}, Type::null)},
        {"putfarray", new Function("putfarray", {Operand("n", Type::Int), Operand("arr", Type::FloatPtr)}, Type::null)},
    };
    return &lib_funcs;
}

/**
 * @brief 进入新作用域时，向符号表中压栈(作用域)
 * @note 作用域的id用作用域栈的大小确定
 * @note 作用域的名称为"Scp"+cnt
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::SymbolTable::add_scope()
{
    // TODO; lab2todo2 add_scope
    ScopeInfo scopeInfo;
    scopeInfo.cnt = scope_stack.size();
    scopeInfo.name = "Scp" + std::to_string(scopeInfo.cnt);
    scope_stack.push_back(scopeInfo);
}

/**
 * @brief 退出作用域时弹栈(作用域)
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::SymbolTable::exit_scope()
{
    // TODO; lab2todo4 exit_scope
    scope_stack.pop_back();
}

/**
 * @brief 变量重命名
 * @param id 变量原名
 * @return string 重命名后的名称
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
string frontend::SymbolTable::get_scoped_name(string id) const
{
    TODO;
}

/**
 * @brief 寻找符号表中最近的同名变量，返回对应Operand
 * @param id 变量名
 * @return Operand 对应操作数
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
Operand frontend::SymbolTable::get_operand(string id) const
{
    TODO;
}

/**
 * @brief 寻找符号表中最近的同名变量，返回对应STE
 * @param id 变量名
 * @return frontend::STE 返回对应STE
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
frontend::STE frontend::SymbolTable::get_ste(string id) const
{
    TODO;
}

frontend::Analyzer::Analyzer() : tmp_cnt(0), symbol_table() {}

/**
 * @brief 实验2主函数，根据AST生成ir代码
 * @param root AST的根节点
 * @return ir::Program
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
ir::Program frontend::Analyzer::get_ir_program(CompUnit *root)
{
    // TODO; lab2todo1 get_ir_program
    ir::Program program; // 存放ir程序

    // 添加运行时库到符号表中
    map<std::string, ir::Function *> *libFuncs = get_lib_funcs();
    for (auto libFunc = libFuncs->begin(); libFunc != libFuncs->end(); libFunc++)
    {
        symbol_table.functions[libFunc->first] = libFunc->second;
    }

    symbol_table.add_scope(); // 添加全局作用域

    analyzeCompUnit(root); // 从根节点开始分析AST

    //* 处理全局变量的初始化
    //* 采用了实验指导书中的做法，创建一个global函数用于初始化
    Function globalFunc("global", Type::null);
    globalFunc.InstVec = g_init_inst;
    globalFunc.addInst(new Instruction({}, {}, {}, {Operator::_return}));

    program.functions.push_back(globalFunc);

    for (auto func = symbol_table.functions.begin(); func != symbol_table.functions.end(); func++)
    {
        if (func->first == "main") // 在main函数的最前面生成对global函数的调用
            func->second->InstVec.insert(func->second->InstVec.begin(), new ir::CallInst(Operand("global", Type::null), {}));
        program.functions.push_back(*func->second);
    }

    symbol_table.exit_scope(); // 退出全局作用域
    return program;
}

/**
 * @brief 1 编译单元 CompUnit -> (Decl | FuncDef) [CompUnit]
 * @param root
 * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
 * @date 2024-05-15
 */
void frontend::Analyzer::analyzeCompUnit(CompUnit *root)
{
    // TODO; lab2todo3 analyzeCompUnit
    if (MATCH_NODE_TYPE(NodeType::DECL, 0)) // Decl
    {
        ANALYZE(Decl, 0, g_init_inst)
    }
    else if (MATCH_NODE_TYPE(NodeType::FUNCDEF, 0)) // FuncDef
    {
        GET_NODE_PTR(FuncDef, funcDef, 0)
        analyzeFuncDef(funcDef);
    }
    else
        assert(0 && "analyzeCompUnit error");

    if (root->children.size() > 1) // [CompUnit]
    {
        GET_NODE_PTR(CompUnit, compUnit, 1)
        analyzeCompUnit(compUnit);
    }
}

/**
 * @brief 2 声明 Decl -> ConstDecl | VarDecl
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::Analyzer::analyzeDecl(Decl *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo5 analyzeDecl
}

/**
 * @brief 10函数定义 FuncDef -> FuncType Ident '(' [FuncFParams] ')' Block
 * @param root
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::Analyzer::analyzeFuncDef(FuncDef *root)
{
    // TODO; lab2todo6 analyzeFuncDef
    GET_NODE_PTR(FuncType, type, 0)
    auto funcType = analyzeFuncType(type); // FuncType 函数返回值类型
    GET_NODE_PTR(Term, term, 1)
    auto funcName = analyzeTerm(term); // Indent 函数名

    symbol_table.add_scope(); // 从函数形参开始进入函数作用域
    vector<ir::Operand> fParams;
    if (MATCH_NODE_TYPE(NodeType::FUNCFPARAMS, 3)) // 如果函数有形参
    {
        GET_NODE_PTR(FuncFParams, funcFParams, 3)
        analyzeFuncFParams(funcFParams, fParams);
    }
    curFuncPtr = new Function(funcName, fParams, funcType); // 创建当前函数的指针
    symbol_table.functions[funcName] = curFuncPtr;          // 将函数添加到符号表
    GET_NODE_PTR(Block, block, root->children.size() - 1)
    analyzeBlock(block, curFuncPtr->InstVec); // 分析函数的指令，指令需要放到函数指令集内
    symbol_table.exit_scope();                // 分析完block后退出函数作用域

    if (curFuncPtr->InstVec.back()->op != Operator::_return)
    {
        //* 部分返回值为void的函数可能没有retrun，需要自动添加一条return;
        if (funcType == ir::Type::null)
            curFuncPtr->addInst(new Instruction({}, {}, {}, Operator::_return));
        //* 部分main函数可能没有return，需要自动添加一条return 0;
        else if (funcName == "main")
            curFuncPtr->addInst(new Instruction({"0", ir::Type::IntLiteral}, {}, {}, {Operator::_return}));
        else
            assert(0 && "function no return");
    }
}

/**
 * @brief 11函数类型 FuncType -> 'void' | 'int' | 'float'
 * @return ir::Type
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
ir::Type frontend::Analyzer::analyzeFuncType(FuncType *root)
{
    // TODO; lab2todo7 analyzeFuncType
    GET_NODE_PTR(Term, term, 0)
    if (term->token.type == TokenType::VOIDTK)
        return Type::null;
    else if (term->token.type == TokenType::INTTK)
        return Type::Int;
    else if (term->token.type == TokenType::FLOATTK)
        return Type::Float;
    else
        assert(0 && "analyzeFuncType error");
}

/**
 * @brief 返回标识符名称
 * @param root
 * @return std::string
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
std::string frontend::Analyzer::analyzeTerm(Term *root)
{
    // TODO; lab2todo8 analyzeTerm
    return root->token.value;
}

/**
 * @brief 13函数形参表 FuncFParams -> FuncFParam { ',' FuncFParam }
 * @param root
 * @param fParams
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::Analyzer::analyzeFuncFParams(FuncFParams *root, vector<ir::Operand> &fParams)
{
    // TODO; lab2todo9 analyzeFuncFParams
}

/**
 * @brief 14语句块 Block -> '{' { BlockItem } '}'
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::Analyzer::analyzeBlock(Block *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo10 analyzeBlock
    for (int i = 1; i < root->children.size() - 1; i++)
    {
        GET_NODE_PTR(BlockItem, blockItem, i)
        analyzeBlockItem(blockItem, buffer);
    }
}

/**
 * @brief 15语句块项 BlockItem -> Decl | Stmt
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::Analyzer::analyzeBlockItem(BlockItem *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo11 analyzeBlockItem
    if (MATCH_NODE_TYPE(NodeType::DECL, 0)) // Decl
    {
        GET_NODE_PTR(Decl, decl, 0)
        analyzeDecl(decl, buffer);
    }
    else if (MATCH_NODE_TYPE(NodeType::STMT, 0)) // Stmt
    {
        GET_NODE_PTR(Stmt, stmt, 0)
        analyzeStmt(stmt, buffer);
    }
    else
        assert(0 && "analyzeBlockItem error");
}

/**
 * @brief 16语句 Stmt ->
 * @brief LVal '=' Exp ';' |
 * @brief Block |
 * @brief 'if' '(' Cond ')' Stmt [ 'else' Stmt ] |
 * @brief 'while' '(' Cond ')' Stmt |
 * @brief 'break' ';' |
 * @brief 'continue' ';' |
 * @brief 'return' [Exp] ';' |
 * @brief [Exp] ';'
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::Analyzer::analyzeStmt(Stmt *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo12 analyzeStmt
    GET_NODE_PTR(Term, term, 0)
    if (term->token.type == TokenType::RETURNTK) // 'return' [Exp] ';' |
    {
        if (root->children.size() == 2) // 不存在可选项[Exp]，只有'return' ';'
        {
            buffer.push_back(new Instruction({}, {}, {}, {Operator::_return}));
        }
        else if (MATCH_NODE_TYPE(NodeType::EXP, 1)) // 'return' Exp ';'
        {
            GET_NODE_PTR(Exp, exp, 1)
            analyzeExp(exp, buffer); // 需要根据返回值的类型生成return语句
            //* 但是由于返回值的类型可能不匹配，所以还需要增加一个指向当前函数的全局指针来读取当前的返回值类型
            if (curFuncPtr->returnType == Type::Int)
            {
                buffer.push_back(new Instruction({exp->v, exp->t}, {}, {}, {Operator::_return}));
            }
        }
        else
            assert(0 && "function return error");
    }
}

/**
 * @brief 17表达式 Exp -> AddExp
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeExp(Exp *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo13 analyzeExp
    GET_NODE_PTR(AddExp, addExp, 0)
    analyzeAddExp(addExp, buffer);
    COPY_EXP_NODE(addExp, root)
}

/**
 * @brief 26加减表达式 AddExp -> MulExp { ( '+' | '-' ) MulExp }
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeAddExp(AddExp *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo14 analyzeAddExp
    GET_NODE_PTR(MulExp, mulExp, 0)
    analyzeMulExp(mulExp, buffer);
    COPY_EXP_NODE(mulExp, root)
}

/**
 * @brief 25乘除模表达式 MulExp -> UnaryExp { ( '*' | '/' | '%' ) UnaryExp }
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeMulExp(MulExp *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo15 analyzeMulExp
    GET_NODE_PTR(UnaryExp, unaryExp, 0)
    analyzeUnaryExp(unaryExp, buffer);
    COPY_EXP_NODE(unaryExp, root)
}

/**
 * @brief 22一元表达式 UnaryExp -> PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeUnaryExp(UnaryExp *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo16 analyzeUnaryExp
    GET_NODE_PTR(PrimaryExp, primaryExp, 0)
    analyzePrimaryExp(primaryExp, buffer);
    COPY_EXP_NODE(primaryExp, root)
}

/**
 * @brief 21基本表达式 PrimaryExp -> '(' Exp ')' | LVal | Number
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzePrimaryExp(PrimaryExp *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo17 analyzePrimaryExp
    GET_NODE_PTR(Number, number, 0)
    analyzeNumber(number, buffer);
    COPY_EXP_NODE(number, root)
}

/**
 * @brief 20数值 Number -> IntConst | floatConst
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeNumber(Number *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo18 analyzeNumber
    GET_NODE_PTR(Term, term, 0)
    if (term->token.type == TokenType::INTLTR) // IntConst
    {
        root->t = Type::IntLiteral;  // 补充Number节点的属性为整型常量
        root->v = term->token.value; // 补充Number节点的值为value
    }
}
