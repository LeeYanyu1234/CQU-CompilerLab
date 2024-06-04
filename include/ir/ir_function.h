#ifndef IRFUNCTION_H
#define IRFUNCTION_H
#include <vector>
#include <string>
#include "ir/ir_operand.h"
#include "ir/ir_instruction.h"
namespace ir
{
    /**
     * @brief ir程序的函数结构
     * @note 每一个函数体对应一个function，
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-05-31
     */
    struct Function
    {
        std::string name;                   // 函数名
        ir::Type returnType;                // 返回值类型
        std::vector<Operand> ParameterList; // 函数实参表
        std::vector<Instruction *> InstVec; // 函数包含的ir指令
        Function();
        Function(const std::string &, const ir::Type &);
        Function(const std::string &, const std::vector<Operand> &, const ir::Type &);
        void addInst(Instruction *inst);

        std::string draw();
    };

}
#endif
