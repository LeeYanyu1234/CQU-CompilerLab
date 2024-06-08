/**
 * @file generator.h
 * @brief 目标函数生成器
 * @author LeeYanyu1234 (343820386@qq.com)
 * @version 1.0.1
 * @date 2024-06-06
 *
 * @copyright Copyright (c) 2024 Chongqing University
 *
 */
#ifndef GENERARATOR_H
#define GENERARATOR_H

#include "ir/ir.h"
#include "backend/rv_def.h"
#include "backend/rv_inst_impl.h"

#include <map>
#include <string>
#include <vector>
#include <fstream>

namespace backend
{

    /**
     * @brief 栈中变量与偏移量的映射表
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-06-06
     */
    // struct stackVarMap
    // {
    //     std::map<ir::Operand, int> _table;

    //     int find_operand(ir::Operand);
    //     int add_operand(ir::Operand, uint32_t size = 4);
    // };

    struct Generator
    {
        // TODO; lab3todo18 Generator
        const ir::Program &program;              // the program to gen
        std::ofstream &fout;                     // output file
        std::map<std::string, int> stackVarMap;  // 栈中变量与偏移量的映射表
        int stackSize;                           // 栈大小
        std::map<int, std::string> instLabelMap; // 指令地址与标签的映射
        int instLabelSize;                       // 指令地址标签计数
        int orCnt = 0;                           // or指令跳转计数
        int andCnt = 0;                          // and指令跳转计数

        Generator(ir::Program &, std::ofstream &);

        // reg allocate api
        // rv::rvREG getRd(ir::Operand);
        // rv::rvFREG fgetRd(ir::Operand);
        // rv::rvREG getRs1(ir::Operand);
        // rv::rvREG getRs2(ir::Operand);
        // rv::rvFREG fgetRs1(ir::Operand);
        // rv::rvFREG fgetRs2(ir::Operand);

        // generate wrapper function
        void gen();
        void initGlobaVar(const ir::Function &);
        void gen_func(const ir::Function &);
        void gen_instr(const ir::Instruction &, int idx);

        // 辅助函数生成
        void saveReg(const ir::Function &);
        void genJumpLabel(const ir::Function &);
        void recoverReg();

        // 辅助代码生成
        void genInstReturn(const ir::Instruction &);
        void genInstCall(const ir::Instruction &);
        void genInstDef(const ir::Instruction &);
        void genInstMov(const ir::Instruction &);
        void genInstAdd(const ir::Instruction &);
        void genInstAlloc(const ir::Instruction &);
        void genInstStore(const ir::Instruction &);
        void genInstMul(const ir::Instruction &);
        void genInstLoad(const ir::Instruction &);
        void genInstSub(const ir::Instruction &);
        void genInstDiv(const ir::Instruction &);
        void genInstMod(const ir::Instruction &);
        void genInstEq(const ir::Instruction &);
        void genInstGoto(const ir::Instruction &, int idx);
        void genInstOr(const ir::Instruction &);
        void genInstAnd(const ir::Instruction &);
        void genInstUnuse(const ir::Instruction &);
        void genInstLss(const ir::Instruction &);
        void genInstGtr(const ir::Instruction &);
        void genInstNeq(const ir::Instruction &);

        void loadRegT5(const ir::Operand &);
        void loadRegT4(const ir::Operand &);
        void storeRegT5(const ir::Operand &);

        // 设置对应字段
        void setOption();
        void setText();
        void setData();
        void setGlobal(std::string);
        void setTypeFunc(std::string);
        void setTypeObj(std::string);
        void setLabel(std::string);
        void setIntInitVar(std::string);

        bool isGlobal(const std::string &);
        int findOperand(ir::Operand);
        void addOperand(ir::Operand, uint32_t size = 4);
    };

} // namespace backend

#endif