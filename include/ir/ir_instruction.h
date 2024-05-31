#ifndef IRINSTRUCTION_H
#define IRINSTRUCTION_H

#include "ir/ir_operand.h"
#include "ir/ir_operator.h"

#include <vector>
#include <string>

namespace ir
{

    /**
     * @brief ir程序的指令结构
     * @note 三地址四元式，op1, op2, des, op
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-05-31
     */
    struct Instruction
    {
        Operand op1;
        Operand op2;
        Operand des;
        Operator op;
        Instruction();
        Instruction(const Operand &op1, const Operand &op2, const Operand &des, const Operator &op);
        virtual std::string draw() const;
    };

    /**
     * @brief ir程序的call型指令结构
     * @note 用于函数调用，之所以要单独列出，是因为函数调用还需要传递函数形参表
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-05-31
     */
    struct CallInst : public Instruction
    {
        std::vector<Operand> argumentList; // 函数形参表
        CallInst(const Operand &op1, std::vector<Operand> paraList, const Operand &des);
        CallInst(const Operand &op1, const Operand &des); // 无参数情况
        std::string draw() const;
    };

}
#endif
