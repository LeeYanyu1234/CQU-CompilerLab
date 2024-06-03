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
#define COPY_EXP_NODE(from, to) \
    to->v = from->v;            \
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
 * @brief 转换为字符串
 * @param value 待转换值
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
#define TOS(value) std::to_string(value)

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

frontend::STE::STE() {}

frontend::STE::STE(ir::Operand _operand, vector<int> _dimension, int _size, bool _isConst = false) : operand(_operand), dimension(_dimension), size(_size), isConst(_isConst), val(string()) {}

/**
 * @brief 进入新作用域时，向符号表中压栈(作用域)
 * @note 作用域的id通过作用域栈的大小确定
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
 * @note 为变量加上当前作用域的后缀，例如id_Scp0
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
string frontend::SymbolTable::get_scoped_name(string id) const
{
    // TODO; lab2todo24 get_scoped_name
    assert(id != "");
    return id + "_" + scope_stack.back().name;
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
 * @note 由于需要找到最近变量，所以需要用到rbegin，从最近添加的作用域开始遍历
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
frontend::STE frontend::SymbolTable::get_ste(string id) const
{
    // TODO; lab2todo32 get_ste
    for (auto scope = scope_stack.rbegin(); scope != scope_stack.rend(); scope++)
    {
        if (scope->table.find(id) != scope->table.end())
            return scope->table.find(id)->second;
    }
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

    // 将全局变量添加到globalVal中，Scp0中的变量即为全局变量
    for (auto it = symbol_table.scope_stack[0].table.begin(); it != symbol_table.scope_stack[0].table.end(); it++)
    {
        auto &ste = it->second;
        if (ste.dimension.size())
            program.globalVal.push_back({ste.operand, ste.size});
        else
            program.globalVal.push_back({ste.operand, 0});
    }

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
        GET_NODE_PTR(Decl, decl, 0)
        analyzeDecl(decl, g_init_inst);
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
    if (MATCH_NODE_TYPE(NodeType::CONSTDECL, 0))
    {
        GET_NODE_PTR(ConstDecl, constDecl, 0)
        analyzeConstDecl(constDecl, buffer);
    }
    else if (MATCH_NODE_TYPE(NodeType::VARDECL, 0))
    {
        GET_NODE_PTR(VarDecl, varDecl, 0)
        analyzeVarDecl(varDecl, buffer);
    }
    else
        assert(0 && "analyzeDecl error");
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
    GET_NODE_PTR(FuncType, funcType, 0)
    analyzeFuncType(funcType); // FuncType->t 函数返回值类型
    GET_NODE_PTR(Term, funcName, 1)
    analyzeTerm(funcName);    // Indent 函数名
    symbol_table.add_scope(); // 从函数形参开始进入函数作用域
    vector<ir::Operand> fParams;
    if (MATCH_NODE_TYPE(NodeType::FUNCFPARAMS, 3)) // 如果函数有形参
    {
        GET_NODE_PTR(FuncFParams, funcFParams, 3)
        analyzeFuncFParams(funcFParams, fParams);
    }
    curFuncPtr = new Function(funcName->v, fParams, funcType->t); // 创建当前函数的指针
    symbol_table.functions[funcName->v] = curFuncPtr;             // 将函数添加到符号表
    GET_NODE_PTR(Block, block, root->children.size() - 1)
    analyzeBlock(block, curFuncPtr->InstVec); // 分析函数的指令，指令需要放到函数指令集内
    symbol_table.exit_scope();                // 分析完block后退出函数作用域

    if (curFuncPtr->InstVec.back()->op != Operator::_return)
    {
        //* 部分返回值为void的函数可能没有retrun，需要自动添加一条return;
        if (funcType->t == ir::Type::null)
            curFuncPtr->addInst(new Instruction({}, {}, {}, Operator::_return));
        //* 部分main函数可能没有return，需要自动添加一条return 0;
        else if (funcName->v == "main")
            curFuncPtr->addInst(new Instruction({"0", ir::Type::IntLiteral}, {}, {}, {Operator::_return}));
        else
            assert(0 && "function no return");
    }
}

/**
 * @brief 11函数类型 FuncType -> 'void' | 'int' | 'float'
 * @param root
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::Analyzer::analyzeFuncType(FuncType *root)
{
    // TODO; lab2todo7 analyzeFuncType
    GET_NODE_PTR(Term, term, 0)
    if (term->token.type == TokenType::VOIDTK)
        root->t = Type::null;
    else if (term->token.type == TokenType::INTTK)
        root->t = Type::Int;
    else if (term->token.type == TokenType::FLOATTK)
        root->t = Type::Float;
    else
        assert(0 && "analyzeFuncType error");
}

/**
 * @brief 传递标识符名称
 * @param root
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void frontend::Analyzer::analyzeTerm(Term *root)
{
    // TODO; lab2todo8 analyzeTerm
    root->v = root->token.value;
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
    for (int i = 0; i < root->children.size(); i += 2)
    {
        GET_NODE_PTR(FuncFParam, funcFParam, i)
        analyzeFuncFParam(funcFParam, fParams);
    }
}

/**
 * @brief 12函数形参 FuncFParam -> BType Ident [ '[' ']' { '[' Exp ']' } ]
 * @param root
 * @param fParams
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-03
 */
void frontend::Analyzer::analyzeFuncFParam(FuncFParam *root, vector<ir::Operand> &fParams)
{
    // TODO; lab2todo37 analyzeFuncFParam
    GET_NODE_PTR(BType, bType, 0)
    analyzeBType(bType); // BType
    GET_NODE_PTR(Term, varName, 1)
    analyzeTerm(varName);                                              // Ident，变量原名存放在varName->v中
    std::string varScpName = symbol_table.get_scoped_name(varName->v); // 重命名变量

    vector<int> dimension;
    int size;
    if (root->children.size() > 2) // 如果是数组，初始size设置为1
    {
        assert(0 && "to be continue");
        // size = 1;
        // //* 解析每一维的大小，并计算总大小
        // for (int i = 2; i < root->children.size(); i += 3) // { '[' ConstExp ']' }
        // {
        //     if (MATCH_NODE_TYPE(NodeType::CONSTEXP, i))
        //     {
        //         GET_NODE_PTR(ConstExp, constExp, i)
        //         analyzeConstExp(constExp);
        //         assert(constExp->t == Type::IntLiteral && std::stoi(constExp->v) >= 0); // constExp一定是非负整数
        //         dimension.push_back(std::stoi(constExp->v));
        //         size *= std::stoi(constExp->v);
        //     }
        //     else
        //         break;
        // }
    }
    else // 如果只是变量，大小设置为0
        size = 0;

    symbol_table.scope_stack.back().table[varName->v] = STE({varScpName, bType->t}, {dimension}, size);
    fParams.push_back({varScpName, bType->t});
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
    if (MATCH_NODE_TYPE(NodeType::LVAL, 0)) // LVal '=' Exp ';'
    {
        GET_NODE_PTR(LVal, lVal, 0)
        analyzeLVal(lVal, buffer);
        GET_NODE_PTR(Exp, exp, 2)
        analyzeExp(exp, buffer);

        Operand lValVar = Operand(lVal->v, lVal->t);
        Operand rValVar;
        if (exp->t == Type::IntLiteral)
        {
            rValVar = IntLiteral2Int(exp->v, buffer);
        }
        else if (exp->t == Type::FloatLiteral)
        {
            rValVar = FloatLiteral2Float(exp->v, buffer);
        }
        else
        {
            rValVar = Operand(exp->v, exp->t);
        }

        if (lValVar.type == Type::Int || lValVar.type == Type::Float) // 左值是变量，进行变量赋值
        {
            if (lValVar.type == Type::Int && rValVar.type == Type::Int) // 左值和右值的变量类型都为整型
            {
                buffer.push_back(new Instruction({rValVar}, {}, {lValVar}, {Operator::mov}));
            }
        }
        else if (lValVar.type == Type::IntPtr || lValVar.type == Type::FloatPtr) // 左值是数组，进行数组赋值
        {
        }
        else
            assert(0 && "lVal type error");
    }
    else
    {
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
                //* 由于返回值的类型可能不匹配，所以还需要增加一个指向当前函数的全局指针来读取当前的返回值类型
                if (curFuncPtr->returnType == Type::Int)
                {
                    buffer.push_back(new Instruction({exp->v, exp->t}, {}, {}, {Operator::_return}));
                }
            }
            else
                assert(0 && "function return error");
        }
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
    GET_NODE_PTR(MulExp, mulExp1, 0)
    analyzeMulExp(mulExp1, buffer);

    int idx = -1;
    //* 如果表达式中有常量
    if (mulExp1->t == Type::IntLiteral || mulExp1->t == Type::FloatLiteral)
    {
        for (int i = 2; i < root->children.size(); i += 2)
        {
            vector<Instruction *> mulInst;
            GET_NODE_PTR(Term, term, i - 1)
            GET_NODE_PTR(MulExp, mulExp2, i)
            analyzeMulExp(mulExp2, mulInst);
            if (mulExp2->t == Type::IntLiteral || mulExp2->t == Type::FloatLiteral)
            {
                if (mulExp1->t == Type::IntLiteral && mulExp2->t == Type::IntLiteral)
                {
                    if (term->token.type == TokenType::PLUS)
                    {
                        mulExp1->v = TOS(std::stoi(mulExp1->v) + std::stoi(mulExp2->v));
                    }
                    else if (term->token.type == TokenType::MINU)
                    {
                        mulExp1->v = TOS(std::stoi(mulExp1->v) - std::stoi(mulExp2->v));
                    }
                    else
                        assert(0 && "AddExp op error");
                }
                else if (mulExp1->t == Type::IntLiteral && mulExp2->t == Type::FloatLiteral)
                {
                    mulExp1->t = Type::FloatLiteral;
                    if (term->token.type == TokenType::PLUS)
                    {
                        mulExp1->v = TOS(std::stoi(mulExp1->v) + std::stof(mulExp2->v));
                    }
                    else if (term->token.type == TokenType::MINU)
                    {
                        mulExp1->v = TOS(std::stoi(mulExp1->v) - std::stof(mulExp2->v));
                    }
                    else
                        assert(0 && "AddExp op error");
                }
                else if (mulExp1->t == Type::FloatLiteral && mulExp2->t == Type::IntLiteral)
                {
                    if (term->token.type == TokenType::PLUS)
                    {
                        mulExp1->v = TOS(std::stof(mulExp1->v) + std::stoi(mulExp2->v));
                    }
                    else if (term->token.type == TokenType::MINU)
                    {
                        mulExp1->v = TOS(std::stof(mulExp1->v) - std::stoi(mulExp2->v));
                    }
                    else
                        assert(0 && "AddExp op error");
                }
                else
                {
                    if (term->token.type == TokenType::PLUS)
                    {
                        mulExp1->v = TOS(std::stof(mulExp1->v) + std::stof(mulExp2->v));
                    }
                    else if (term->token.type == TokenType::MINU)
                    {
                        mulExp1->v = TOS(std::stof(mulExp1->v) - std::stof(mulExp2->v));
                    }
                    else
                        assert(0 && "AddExp op error");
                }
            }
            else
            {
                idx = i;
                break;
            }
        }
    }

    if (mulExp1->t == Type::IntLiteral || mulExp1->t == Type::FloatLiteral && idx == -1) // 如果表达式只有常量
    {
        COPY_EXP_NODE(mulExp1, root)
    }
    else
    {
        Operand op1;
        if (idx == -1) // 表达式有常量
        {
            op1.name = mulExp1->v;
            op1.type = mulExp1->t;
            idx = 2;
        }
        else
        {
            if (mulExp1->t == Type::IntLiteral)
            {
                op1 = IntLiteral2Int(mulExp1->v, buffer);
            }
            else
            {
                op1 = FloatLiteral2Float(mulExp1->v, buffer);
            }
        }

        if (root->children.size() > 1)
        {
            if ((op1.type == Type::Int || op1.type == Type::Float) && op1.name.find('_') != op1.name.npos)
            {
                auto tmpVar = Operand(getTmp(), op1.type == Type::Int ? Type::Int : Type::Float);
                Operator cal = (op1.type == Type::Int) ? Operator::mov : Operator::fmov;
                buffer.push_back(new Instruction(op1, {}, tmpVar, cal));
                std::swap(op1, tmpVar);
            }
            for (int i = idx; i < root->children.size(); i += 2)
            {
                GET_NODE_PTR(Term, term, i - 1)
                GET_NODE_PTR(MulExp, mulExp2, i)
                analyzeMulExp(mulExp2, buffer);
                Operand op2;
                if (mulExp2->t == Type::IntLiteral)
                {
                    op2 = IntLiteral2Int(mulExp2->v, buffer);
                }
                else if (mulExp2->t == Type::FloatLiteral)
                {
                    op2 = FloatLiteral2Float(mulExp2->v, buffer);
                }
                else
                {
                    op2 = Operand(mulExp2->v, mulExp2->t);
                }

                if (term->token.type == TokenType::PLUS)
                {
                    if (op1.type == Type::Int && op2.type == Type::Int)
                    {
                        buffer.push_back(new Instruction(op1, op2, op1, Operator::add));
                    }
                    else
                    {
                        assert(0 && "to be continue");
                    }
                }
                else if (term->token.type == TokenType::MINU)
                {
                    if (op1.type == Type::Int && op2.type == Type::Int)
                    {
                        buffer.push_back(new Instruction(op1, op2, op1, Operator::sub));
                    }
                    else
                    {
                        assert(0 && "to be continue");
                    }
                }
                else
                    assert(0 && "AddExp op error");
            }
        }

        root->t = op1.type;
        root->v = op1.name;
    }
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
    if (MATCH_NODE_TYPE(NodeType::PRIMARYEXP, 0)) // PrimaryExp
    {
        GET_NODE_PTR(PrimaryExp, primaryExp, 0)
        analyzePrimaryExp(primaryExp, buffer);
        COPY_EXP_NODE(primaryExp, root)
    }
    else if (MATCH_NODE_TYPE(NodeType::TERMINAL, 0)) // Ident '(' [FuncRParams] ')' 函数调用
    {
        GET_NODE_PTR(Term, funcName, 0)
        analyzeTerm(funcName);                                             // 函数名存放在funcName->v中
        auto fParams = symbol_table.functions[funcName->v]->ParameterList; // 通过函数名找到函数形参表
        auto returnType = symbol_table.functions[funcName->v]->returnType; // 通过函数名找到函数返回值类型

        vector<Operand> args;
        if (root->children.size() > 3) // 函数调用需要传参
        {
            GET_NODE_PTR(FuncRParams, funcRParams, 2)
            analyzeFuncRParams(funcRParams, buffer, fParams, args);
        }

        if (returnType == Type::null)
        {
            buffer.push_back(new ir::CallInst({funcName->v, args, {}}));
            root->t = Type::null;
        }
        else
        {
            Operand tmpVarReturn = Operand({getTmp(), returnType});
            buffer.push_back(new ir::CallInst(funcName->v, args, tmpVarReturn));
            root->v = tmpVarReturn.name;
            root->t = returnType;
        }
    }
    else if (MATCH_NODE_TYPE(NodeType::UNARYOP, 0)) // UnaryOp UnaryExp
    {
        assert(0 && "to be continue");
    }
    else
        assert(0 && "analyzeUnaryExp error");
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
    if (root->children.size() > 1) // '(' Exp ')'
    {
    }
    else if (MATCH_NODE_TYPE(NodeType::LVAL, 0)) // LVal
    {
        GET_NODE_PTR(LVal, lVal, 0)
        analyzeLVal(lVal, buffer);

        if (lVal->t == Type::IntPtr || lVal->t == Type::FloatPtr) // 如果左值是数组，返回的是指针
        {
            Operand lValVar = Operand(lVal->v, lVal->t);
            Operand offsetVar = Operand(lVal->offset, Type::Int);
            if (lVal->isPtr)
            {
                auto tmpVar = Operand(getTmp(), lVal->t);
                buffer.push_back(new Instruction({lValVar}, {offsetVar}, {tmpVar}, {Operator::getptr}));
                root->v = tmpVar.name;
                root->t = tmpVar.type;
            }
            else
            {
                if (lVal->t == Type::IntPtr)
                {
                    buffer.push_back(new Instruction({lValVar}, {offsetVar}, {offsetVar}, {Operator::load}));
                    root->v = offsetVar.name;
                    root->t = offsetVar.type;
                }
                else
                    assert(0 && "to be continue");
            }
        }
        else // 如果左值是变量，返回的是变量名
        {
            COPY_EXP_NODE(lVal, root)
        }
    }
    else if (MATCH_NODE_TYPE(NodeType::NUMBER, 0))
    {
        GET_NODE_PTR(Number, number, 0)
        analyzeNumber(number, buffer);
        COPY_EXP_NODE(number, root)
    }
    else
        assert(0 && "analyzePrimaryExp error");
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

/**
 * @brief 7 变量声明 VarDecl -> BType VarDef { ',' VarDef } ';'
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeVarDecl(VarDecl *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo19 analyzeVarDecl
    GET_NODE_PTR(BType, bType, 0)
    analyzeBType(bType);
    root->t = bType->t; // 标识符类型
    for (int i = 1; i < root->children.size(); i += 2)
    {
        GET_NODE_PTR(VarDef, varDef, i)
        analyzeVarDef(varDef, buffer, root->t);
    }
}

/**
 * @brief 4 基本类型 BType -> 'int' | 'float'
 * @param root
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeBType(BType *root)
{
    // TODO; lab2todo22 analyzeBType
    GET_NODE_PTR(Term, term, 0)
    if (term->token.type == TokenType::INTTK)
        root->t = Type::Int;
    else if (term->token.type == TokenType::FLOATTK)
        root->t = Type::Float;
    else
        assert(0 && "analyzeBType error");
}

/**
 * @brief 8 变量定义 VarDef -> Ident { '[' ConstExp ']' } [ '=' InitVal ]
 * @param root
 * @param buffer
 * @param type 标识符类型
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeVarDef(VarDef *root, vector<ir::Instruction *> &buffer, ir::Type type)
{
    // TODO; lab2todo23 analyzeVarDef
    GET_NODE_PTR(Term, term, 0)
    analyzeTerm(term);
    root->arr_name = symbol_table.get_scoped_name(term->v); // 获取变量名并重命名

    vector<int> dimension; // 存储每一维变量的大小
    int size;              // 变量空间大小

    //* 数组的size一定>=1，如果是变量size必须设置为0
    //? 这里不能直接匹配idx=2的节点，不然会出现段错误
    //? 如果遇到int a;这种情况，会出现越界访问，所以需要先判断节点的数目
    if (root->children.size() > 1 && MATCH_NODE_TYPE(NodeType::CONSTEXP, 2)) // 如果是数组，初始size设置为1
    {
        size = 1;
        //* 解析每一维的大小，并计算总大小
        for (int i = 2; i < root->children.size(); i += 3) // { '[' ConstExp ']' }
        {
            if (MATCH_NODE_TYPE(NodeType::CONSTEXP, i))
            {
                GET_NODE_PTR(ConstExp, constExp, i)
                analyzeConstExp(constExp);
                assert(constExp->t == Type::IntLiteral && std::stoi(constExp->v) >= 0); // constExp一定是非负整数
                dimension.push_back(std::stoi(constExp->v));
                size *= std::stoi(constExp->v);
            }
            else
                break;
        }
    }
    else // 如果只是变量，大小设置为0
        size = 0;

    //* 定义一个变量需要做两件事，一是为其分配空间，二是将其插入符号表中
    if (MATCH_NODE_TYPE(NodeType::INITVAL, root->children.size() - 1)) // [ '=' InitVal ]
    {
        GET_NODE_PTR(InitVal, initVal, root->children.size() - 1)
        initVal->v = root->arr_name;
        if (type == Type::Int) // 变量类型为整型
        {
            initVal->t = Type::Int;
            if (size == 0) // 如果是变量，符号表中为整型变量
            {
                symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::Int), dimension, size);
            }
            else // 如果是数组，符号表中为整型指针
            {
                symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::IntPtr), dimension, size);
                if (symbol_table.scope_stack.size() > 1) // 如果不是全局变量，需要添加一条alloc指令分配空间
                    buffer.push_back(new Instruction({TOS(size), Type::IntLiteral}, {}, {root->arr_name, Type::IntPtr}, {Operator::alloc}));
            }
        }
        else if (type == Type::Float) // 变量类型为浮点型
        {
            initVal->t = Type::Float;
            if (size == 0) // 如果是变量，符号表中为浮点型变量
            {
                symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::Float), dimension, size);
            }
            else // 如果是数组，符号表中为浮点型指针
            {
                symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::FloatPtr), dimension, size);
                if (symbol_table.scope_stack.size() > 1) // 如果不是全局变量，需要添加一条alloc指令分配空间
                    buffer.push_back(new Instruction({TOS(size), Type::IntLiteral}, {}, {root->arr_name, Type::FloatPtr}, {Operator::alloc}));
            }
        }
        else
            assert(0 && "InitVal type error");

        //* 对变量进行初始化
        analyzeInitVal(initVal, buffer, size, 0, 0, dimension);
    }
    else // 不包含可选项[ '=' InitVal ]，也就是说只声明，没有初始化
    {
        //* 还是需要将变量插入符号表中
        if (type == Type::Int) // 变量类型为整型
        {
            if (size == 0) // 如果是变量，符号表中为整型变量
            {
                symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::Int), dimension, size); // 插入符号表
                //* 如果不是全局变量，那么这个变量还会被初始化为一个随机值
                if (symbol_table.scope_stack.size() > 1)
                    buffer.push_back(new Instruction({"473289", Type::IntLiteral}, {}, {root->arr_name, Type::Int}, {Operator::def}));
            }
            else // 如果是数组，符号表中为整型指针
            {
                symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::IntPtr), dimension, size);
                if (symbol_table.scope_stack.size() > 1) // 如果不是全局变量，需要添加一条alloc指令分配空间
                    buffer.push_back(new Instruction({TOS(size), Type::IntLiteral}, {}, {root->arr_name, Type::IntPtr}, {Operator::alloc}));
            }
        }
        else if (type == Type::Float)
        {
            if (size == 0) // 如果是变量，符号表中为浮点型变量
            {
                symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::Float), dimension, size);
                //* 如果不是全局变量，那么这个变量还会被初始化为一个随机值
                if (symbol_table.scope_stack.size() > 1)
                    buffer.push_back(new Instruction({"3.1415926", Type::FloatLiteral}, {}, {root->arr_name, Type::Float}, {Operator::fdef}));
            }
            else // 如果是数组，符号表中为浮点型指针
            {
                symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::FloatPtr), dimension, size);
                if (symbol_table.scope_stack.size() > 1) // 如果不是全局变量，需要添加一条alloc指令分配空间
                    buffer.push_back(new Instruction({TOS(size), Type::IntLiteral}, {}, {root->arr_name, Type::FloatPtr}, {Operator::alloc}));
            }
        }
        else
            assert(0 && "InitVal type error");
    }
}

/**
 * @brief 31常量表达式 ConstExp -> AddExp
 * @param root
 * @note 根据SysY语言定义，每一个ConstExp都必须在编译时能求值到非负整数
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeConstExp(ConstExp *root)
{
    // TODO; lab2todo25 analyzeConstExp
    GET_NODE_PTR(AddExp, addExp, 0)
    vector<Instruction *> tmpInst;
    analyzeAddExp(addExp, tmpInst);
    COPY_EXP_NODE(addExp, root)
}

/**
 * @brief 9 变量初值 InitVal -> Exp | '{' [ InitVal { ',' InitVal } ] '}'
 * @param root InitVal类型节点
 * @param buffer
 * @param size 变量大小，0为变量，>=1为数组
 * @param cur 当前数组的索引
 * @param offset
 * @param dimension 数组的维度数组
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeInitVal(InitVal *root, vector<ir::Instruction *> &buffer, int size, int cur, int offset, vector<int> &dimension)
{
    // TODO; lab2todo27 analyzeInitVal
    if (MATCH_NODE_TYPE(NodeType::EXP, 0) && size == 0) // Exp
    {
        // 只有当size==0时初始化变量，而非数组
        GET_NODE_PTR(Exp, exp, 0)
        analyzeExp(exp, buffer);

        if (root->t == Type::Int && exp->t == Type::IntLiteral)
        {
            if (symbol_table.scope_stack.size() > 1) // 不是全局变量，直接def即可
                buffer.push_back(new Instruction({exp->v, exp->t}, {}, {root->v, Type::Int}, {Operator::def}));
            else // 是全局变量，由于已经在静态区域申请了空间，所以需要通过mov指令初始化
            {
                auto tmpVar = IntLiteral2Int(exp->v, buffer);
                buffer.push_back(new Instruction(tmpVar, {}, Operand(root->v, Type::Int), Operator::mov));
            }
        }
    }
    else if (MATCH_NODE_TYPE(NodeType::TERMINAL, 0)) // '{' [ InitVal { ',' InitVal } ] '}'
    {
        assert(size >= 1); // {}都是对于数组赋值，此时的size>=1表示是数组
        size /= dimension[cur];
        int cnt = 0, tot = root->children.size() / 2;
        for (int i = 1; i < root->children.size() - 1; i += 2) // 对已经初始化的索引进行初始化
        {
            GET_NODE_PTR(InitVal, initVal, i)
            COPY_EXP_NODE(root, initVal)
            if (tot <= dimension[cur])
            {
                analyzeInitVal(initVal, buffer, size, cur + 1, offset + cnt * size, dimension);
            }
            else
            {
                analyzeInitVal(initVal, buffer, 1, cur, offset + cnt, dimension);
            }
            cnt++;
        }

        for (int i = cnt * size; i < dimension[cur] * size; i++) // 剩下未明确初始化的索引自动初始化为0
        {
            Type type = (root->t == Type::Int) ? Type::IntLiteral : Type::FloatLiteral;
            Operand tmpVar = (type == Type::IntLiteral) ? IntLiteral2Int("0", buffer) : FloatLiteral2Float("0.0", buffer);
            buffer.push_back(new Instruction({root->v, (root->t == Type::Int ? Type::IntPtr : Type::FloatLiteral)}, {TOS(i), Type::IntLiteral}, {tmpVar}, {Operator::store}));
        }
    }
    else if (dynamic_cast<InitVal *>(root->parent))
    {
        GET_NODE_PTR(Exp, exp, 0);
        analyzeExp(exp, buffer);
        Operand expVar = exp->t == Type::IntLiteral ? IntLiteral2Int(exp->v, buffer) : FloatLiteral2Float(exp->v, buffer);
        if (root->t == Type::Int && exp->t == Type::IntLiteral)
        {
            buffer.push_back(new Instruction({root->v, Type::IntPtr}, {TOS(offset), Type::IntLiteral}, {expVar}, {Operator::store}));
        }
    }
    else
        assert(0 && "analyzeInitVal error");
}

/**
 * @brief 19左值表达式 LVal -> Ident { '[' Exp ']' }
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
void frontend::Analyzer::analyzeLVal(LVal *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo30 analyzeLVal
    GET_NODE_PTR(Term, valName, 0); // valName->v为标识符名称
    analyzeTerm(valName);
    auto ste = symbol_table.get_ste(valName->v); // 获取当前左值的符号表项

    //* 如果是常量直接进行值替换
    if (ste.isConst && (ste.operand.type == Type::Int || ste.operand.type == Type::Float))
    {
        root->v = ste.val;
        root->t = (ste.operand.type == Type::Int) ? Type::IntLiteral : Type::FloatLiteral;
        root->isPtr = false;
    }
    else //* 变量或数组
    {
        root->v = ste.operand.name; // 复制变量名/数组名
        root->t = ste.operand.type; // 复制变量类型/数组类型

        if (ste.size == 0) // size==0，说明是变量
        {
            root->isPtr = false;
        }
        else // size>0，说明是数组
        {
            int cur = 0, curSize = ste.size;
            Operand offsetVar = IntLiteral2Int("0", buffer);
            for (int i = 2; i < root->children.size(); i += 3)
            {
                curSize /= ste.dimension[cur++];
                Operand tmpVar = IntLiteral2Int(TOS(curSize), buffer);
                GET_NODE_PTR(Exp, exp, i)
                analyzeExp(exp, buffer);
                auto expVar = exp->t == Type::IntLiteral ? IntLiteral2Int(exp->v, buffer) : Operand(exp->v, exp->t);
                buffer.push_back(new Instruction({tmpVar}, {expVar}, {tmpVar}, {Operator::mul}));
                buffer.push_back(new Instruction({offsetVar}, {tmpVar}, {offsetVar}, {Operator::add}));
            }
            root->offset = offsetVar.name;
            root->isPtr = cur < ste.dimension.size();
        }
    }
}

/**
 * @brief 3 常量声明 ConstDecl -> 'const' BType ConstDef { ',' ConstDef } ';'
 * @param root
 * @param buffer
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-02
 */
void frontend::Analyzer::analyzeConstDecl(ConstDecl *root, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo34 analyzeConstDecl
    GET_NODE_PTR(BType, bType, 1)
    analyzeBType(bType); // BType
    root->t = bType->t;
    for (int i = 2; i < root->children.size(); i += 2) // ConstDef { ',' ConstDef }
    {
        GET_NODE_PTR(ConstDef, constDef, i)
        analyzeConstDef(constDef, buffer, root->t);
    }
}

/**
 * @brief 5 常量定义 ConstDef -> Ident { '[' ConstExp ']' } '=' ConstInitVal
 * @param root
 * @param buffer
 * @param type
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-02
 */
void frontend::Analyzer::analyzeConstDef(ConstDef *root, vector<ir::Instruction *> &buffer, ir::Type type)
{
    // TODO; lab2todo35 analyzeConstDef
    GET_NODE_PTR(Term, term, 0)
    analyzeTerm(term);
    root->arr_name = symbol_table.get_scoped_name(term->v); // 获取变量名并重命名

    //* 这里的处理方法与analyzeVarDef一致
    vector<int> dimension;                                                   // 存储每一维变量的大小
    int size;                                                                // 变量空间大小                                                              // 变量空间大小
    if (root->children.size() > 1 && MATCH_NODE_TYPE(NodeType::CONSTEXP, 2)) // 如果是数组，初始size设置为1
    {
        size = 1;
        //* 解析每一维的大小，并计算总大小
        for (int i = 2; i < root->children.size(); i += 3) // { '[' ConstExp ']' }
        {
            if (MATCH_NODE_TYPE(NodeType::CONSTEXP, i))
            {
                GET_NODE_PTR(ConstExp, constExp, i)
                analyzeConstExp(constExp);
                assert(constExp->t == Type::IntLiteral && std::stoi(constExp->v) >= 0); // constExp一定是非负整数
                dimension.push_back(std::stoi(constExp->v));
                size *= std::stoi(constExp->v);
            }
            else
                break;
        }
    }
    else // 如果只是变量，大小设置为0
        size = 0;

    //* 这里的处理与变量有一点区别，常量在声明时必须初始化，所以不需要匹配节点
    GET_NODE_PTR(ConstInitVal, constInitVal, root->children.size() - 1)
    constInitVal->v = term->token.value; // 放入变量原名
    if (type == Type::Int)               // 常量类型为整型
    {
        if (size == 0) // 如果是普通变量
        {
            symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::Int), dimension, size, true); // 插入符号表
            constInitVal->t = Type::Int;
        }
        else // 如果是数组
        {
            symbol_table.scope_stack.back().table[term->token.value] = STE(Operand(root->arr_name, Type::IntPtr), dimension, size, true); // 插入符号表
            constInitVal->t = Type::IntLiteral;
            if (symbol_table.scope_stack.size() > 1) // 如果不是一个全局变量，需要通过alloc分配空间
            {
                buffer.push_back(new Instruction({TOS(size), Type::IntLiteral}, {}, {root->arr_name, Type::IntPtr}, {Operator::alloc}));
            }
        }
    }
    analyzeConstInitVal(constInitVal, buffer, size, 0, 0, dimension);
}

/**
 * @brief 6 常量初值 ConstInitVal -> ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
 * @param root
 * @param buffer
 * @param size
 * @param cur
 * @param offset
 * @param dimension
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-02
 */
void frontend::Analyzer::analyzeConstInitVal(ConstInitVal *root, vector<ir::Instruction *> &buffer, int size, int cur, int offset, vector<int> &dimension)
{
    // TODO; lab2todo36 analyzeConstInitVal
    //* 这里的处理类似于analyzeInitVal，但是也有区别
    if (size == 0) // 如果是普通变量
    {
        GET_NODE_PTR(ConstExp, constExp, 0) // 常量变量的初始化一定是通过ConstExp
        analyzeConstExp(constExp);
        symbol_table.scope_stack.back().table[root->v].val = constExp->v; // 将常量的字面量记录到符号表
    }
    else // 如果是数组
    {
        if (MATCH_NODE_TYPE(NodeType::TERMINAL, 0)) // '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
        {
            assert(size >= 1); // {}都是对于数组赋值，此时的size>=1表示是数组
            size /= dimension[cur];
            int cnt = 0, tot = root->children.size() / 2;
            for (int i = 1; i < root->children.size() - 1; i += 2) // 对已经初始化的索引进行初始化
            {
                GET_NODE_PTR(ConstInitVal, constInitVal, i)
                COPY_EXP_NODE(root, constInitVal)
                if (tot <= dimension[cur])
                {
                    analyzeConstInitVal(constInitVal, buffer, size, cur + 1, offset + cnt * size, dimension);
                }
                else
                {
                    analyzeConstInitVal(constInitVal, buffer, 1, cur, offset + cnt, dimension);
                }
                cnt++;
            }

            for (int i = cnt * size; i < dimension[cur] * size; i++) // 剩下未明确初始化的索引自动初始化为0
            {
                Type type = (root->t == Type::Int) ? Type::IntLiteral : Type::FloatLiteral;
                Operand tmpVar = (type == Type::IntLiteral) ? IntLiteral2Int("0", buffer) : FloatLiteral2Float("0.0", buffer);
                //? 这里的root->v存放的是变量原名
                buffer.push_back(new Instruction({symbol_table.get_scoped_name(root->v), (root->t == Type::Int ? Type::IntPtr : Type::FloatLiteral)}, {TOS(i), Type::IntLiteral}, {tmpVar}, {Operator::store}));
            }
        }
        else if (dynamic_cast<ConstInitVal *>(root->parent))
        {
            GET_NODE_PTR(ConstExp, constExp, 0);
            analyzeConstExp(constExp);
            // Operand constExpVar = constExp->t == Type::IntLiteral ? IntLiteral2Int(constExp->v, buffer) : FloatLiteral2Float(constExp->v, buffer);
            if (root->t == Type::IntLiteral && constExp->t == Type::IntLiteral)
            {
                buffer.push_back(new Instruction({symbol_table.get_scoped_name(root->v), Type::IntPtr}, {TOS(offset), Type::IntLiteral}, {constExp->v, constExp->t}, {Operator::store}));
            }
            else
            {
                assert(0 && "to be continue");
            }
        }
        else
            assert(0 && "analyzeConstInitVal error");
    }
}

/**
 * @brief 24函数实参表 FuncRParams -> Exp { ',' Exp }
 * @param root
 * @param buffer
 * @param fParams
 * @param args
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-03
 */
void frontend::Analyzer::analyzeFuncRParams(FuncRParams *root, vector<ir::Instruction *> &buffer, vector<ir::Operand> &fParams, vector<ir::Operand> &args)
{
    for (int i = 0, curParam = 0; i < root->children.size(); i += 2)
    {
        GET_NODE_PTR(Exp, exp, i)
        analyzeExp(exp, buffer);
        Operand fParam = fParams[curParam++];
        Operand arg = Operand(exp->v, exp->t);
        if (fParam.type == Type::Int)
        {
            args.push_back(arg);
        }
        else
        {
            assert("to be continue");
        }
    }
}

/**
 * @brief 整型常量转换为整型变量
 * @param val
 * @param buffer
 * @return ir::Operand
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
ir::Operand frontend::Analyzer::IntLiteral2Int(string val, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo28 IntLiteral2Int
    Operand opd = {getTmp(), Type::Int};
    buffer.push_back(new Instruction({val, Type::IntLiteral}, {}, {opd}, {Operator::def}));
    return opd;
}

/**
 * @brief 浮点型常量转换为浮点型变量
 * @param val
 * @param buffer
 * @return ir::Operand
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
ir::Operand frontend::Analyzer::FloatLiteral2Float(string val, vector<ir::Instruction *> &buffer)
{
    // TODO; lab2todo33 FloatLiteral2Float
    Operand opd = {getTmp(), Type::Float};
    buffer.push_back(new Instruction({val, Type::FloatLiteral}, {}, {opd}, {Operator::fdef}));
    return opd;
}

/**
 * @brief 创建新的临时变量
 * @return string 变量名
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-01
 */
std::string frontend::Analyzer::getTmp()
{
    // TODO; lab2todo29 getTmp
    return "t" + std::to_string(tmp_cnt++);
}
