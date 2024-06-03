/**
 * @file semantic.h
 * @brief 语义分析器头文件
 * @author LeeYanyu1234 (343820386@qq.com)
 * @version 1.0.1
 * @date 2024-05-31
 *
 * @copyright Copyright (c) 2024 Chongqing University
 *
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ir/ir.h"
#include "front/abstract_syntax_tree.h"

#include <map>
#include <string>
#include <vector>
using std::map;
using std::string;
using std::vector;

namespace frontend
{

    /**
     * @brief 符号表中的一条记录
     * @note 符号表会面临同名变量的问题，不同的作用域中的符号是可以同名的
     * @note 解决方案是同名变量加上作用域相关的一个后缀来重命名变量
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-05-31
     */
    struct STE
    {
        ir::Operand operand;   // 操作数=名称+类型
        vector<int> dimension; // 各个维度的大小
        // TODO; lab2todo26 struct STE
        int size;     // 总大小
        bool isConst; // 标记是否为常量
        string val;   // 常量数值

        STE();
        STE(ir::Operand, vector<int>, int, bool);
    };

    /**
     * @brief 变量原始名称和重命名的映射
     * @note string是操作数的原始名称，表项STE中操作数的name存放变量重命名后的名称
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-05-31
     */
    using map_str_ste = map<string, STE>;

    /**
     * @brief 一个作用域内的符号表
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-05-31
     */
    struct ScopeInfo
    {
        int cnt;           // 作用域在函数中的唯一编号
        string name;       // name可用于分辨作用域的类别
                           /**
                            ** b：单独嵌套的作用域
                            ** i：if产生的新作用域
                            ** e：else产生的新作用域
                            ** w：while产生的新作用域
                            */
        map_str_ste table; // 单个作用域存放符号的表
    };

    /**
     * @brief 获取运行时库函数的函数映射
     * @return map<std::string, ir::Function *>*
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-15
     */
    map<std::string, ir::Function *> *get_lib_funcs();

    /**
     * @brief ir程序的符号表
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-05-31
     */
    struct SymbolTable
    {
        vector<ScopeInfo> scope_stack;              // 作用域栈
        map<std::string, ir::Function *> functions; // ir程序的函数

        void add_scope();

        void exit_scope();

        /**
         * @brief Get the scoped name, to deal the same name in different scopes, we change origin id to a new one with scope infomation,
         * for example, we have these code:
         * "
         * int a;
         * {
         *      int a; ....
         * }
         * "
         * in this case, we have two variable both name 'a', after change they will be 'a' and 'a_block'
         * @param id: origin id
         * @return string: new name with scope infomations
         */
        string get_scoped_name(string id) const;

        ir::Operand get_operand(string id) const;

        STE get_ste(string id) const;
    };

    /**
     * @brief 语义分析器
     * @note 语义分析需要维护一张符号表
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-05-31
     */
    struct Analyzer
    {
        int tmp_cnt;                           // 目前临时变量的计数
        vector<ir::Instruction *> g_init_inst; // 存放中间代码
        SymbolTable symbol_table;              // 语义分析符号表
        ir::Function *curFuncPtr = nullptr;    // 指向正在处理的函数的指针，用于获取返回值类型

        Analyzer();

        ir::Program get_ir_program(CompUnit *);

        // reject copy & assignment
        Analyzer(const Analyzer &) = delete;
        Analyzer &operator=(const Analyzer &) = delete;

        //***************************语义分析函数*****************************
        void analyzeCompUnit(CompUnit *);
        void analyzeDecl(Decl *, vector<ir::Instruction *> &);
        void analyzeFuncDef(FuncDef *);
        void analyzeFuncType(FuncType *);
        void analyzeTerm(Term *);
        void analyzeFuncFParams(FuncFParams *, vector<ir::Operand> &);
        void analyzeFuncFParam(FuncFParam *, vector<ir::Operand> &);
        void analyzeBlock(Block *, vector<ir::Instruction *> &);
        void analyzeBlockItem(BlockItem *, vector<ir::Instruction *> &);
        void analyzeStmt(Stmt *, vector<ir::Instruction *> &);
        void analyzeExp(Exp *, vector<ir::Instruction *> &);
        void analyzeAddExp(AddExp *, vector<ir::Instruction *> &);
        void analyzeMulExp(MulExp *, vector<ir::Instruction *> &);
        void analyzeUnaryExp(UnaryExp *, vector<ir::Instruction *> &);
        void analyzePrimaryExp(PrimaryExp *, vector<ir::Instruction *> &);
        void analyzeNumber(Number *, vector<ir::Instruction *> &);
        void analyzeVarDecl(VarDecl *, vector<ir::Instruction *> &);
        void analyzeBType(BType *);
        void analyzeVarDef(VarDef *, vector<ir::Instruction *> &, ir::Type);
        void analyzeConstExp(ConstExp *);
        void analyzeInitVal(InitVal *, vector<ir::Instruction *> &, int, int, int, vector<int> &);
        void analyzeLVal(LVal *, vector<ir::Instruction *> &);
        void analyzeConstDecl(ConstDecl *, vector<ir::Instruction *> &);
        void analyzeConstDef(ConstDef *, vector<ir::Instruction *> &, ir::Type);
        void analyzeConstInitVal(ConstInitVal *, vector<ir::Instruction *> &, int, int, int, vector<int> &);
        void analyzeFuncRParams(FuncRParams *, vector<ir::Instruction *> &, vector<ir::Operand> &, vector<ir::Operand> &);

        ir::Operand IntLiteral2Int(string, vector<ir::Instruction *> &);
        ir::Operand FloatLiteral2Float(string, vector<ir::Instruction *> &);
        string getTmp();
    };

} // namespace frontend

#endif