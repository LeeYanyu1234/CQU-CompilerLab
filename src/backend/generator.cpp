/**
 * @file generator.cpp
 * @brief 目标代码生成器相关函数实现
 * @author LeeYanyu1234 (343820386@qq.com)
 * @version 1.0.1
 * @date 2024-06-06
 *
 * @copyright Copyright (c) 2024 Chongqing University
 *
 */
#include "backend/generator.h"

#include <assert.h>

#define TODO assert(0 && "todo")

backend::Generator::Generator(ir::Program &p, std::ofstream &f) : program(p), fout(f) {}

rv::rvREG backend::Generator::getRd(ir::Operand)
{
    TODO;
    return rv::rvREG();
}

rv::rvFREG backend::Generator::fgetRd(ir::Operand)
{
    TODO;
    return rv::rvFREG();
}

rv::rvREG backend::Generator::getRs1(ir::Operand)
{
    TODO;
    return rv::rvREG();
}

rv::rvREG backend::Generator::getRs2(ir::Operand)
{
    TODO;
    return rv::rvREG();
}

rv::rvFREG backend::Generator::fgetRs1(ir::Operand)
{
    TODO;
    return rv::rvFREG();
}

rv::rvFREG backend::Generator::fgetRs2(ir::Operand)
{
    TODO;
    return rv::rvFREG();
}

/**
 * @brief 在映射表中查找局部变量
 * @param op 局部变量
 * @return int 变量对应的栈指针偏移量
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
int backend::stackVarMap::find_operand(ir::Operand op)
{
    TODO;
    return 0;
}

/**
 * @brief 向映射表中添加变量与偏移量的映射
 * @param op 加入的变量
 * @param size 变量大小
 * @return int 返回偏移量
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
int backend::stackVarMap::add_operand(ir::Operand op, uint32_t size)
{
    TODO;
    return 0;
}

void backend::Generator::gen()
{
    // TODO; lab3todo1 gen
    setOption();
    setText();

    //* 跳过global函数，因为global已经单独处理
    for (int i = 1; i < program.functions.size(); i++)
    {
        gen_func(program.functions[i]);
    }
}

/**
 * @brief 生成函数
 * @param func
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::gen_func(const ir::Function &func)
{
    // TODO; lab3todo4 gen_func
    setGlobal(func.name);
    setTypeFunc(func.name);
    setLabel(func.name);

    saveReg(func);

    for (const ir::Instruction *inst : func.InstVec)
    {
        gen_instr(*inst);
    }

    recoverReg(func);

    fout << "\tjr	ra\n";
    this->fout << "\t.size\t" << func.name << ", .-" << func.name << "\n";
}

/**
 * @brief 根据ir指令生成riscv汇编指令
 * @param inst ir指令
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::gen_instr(const ir::Instruction &inst)
{
    // TODO; lab3todo11 gen_instr
    ir::Operator op = inst.op;
    if (op == ir::Operator::_return)
        genInstReturn(inst);
    else if (op == ir::Operator::call)
        genInstCall(inst);
    else
        assert(0 && "to be continue");
}

/**
 * @brief 计算函数的栈大小，保存寄存器
 * @param func
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::saveReg(const ir::Function &func)
{
    // TODO; lab3todo9 saveReg
    fout << "\taddi\tsp,sp,-" << "16" << "\n";
    fout << "\tsw	s0,12(sp)\n";
}

/**
 * @brief 恢复保存寄存器，介绍函数调用
 * @param func
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::recoverReg(const ir::Function &func)
{
    // TODO; lab3todo10 recoverReg
    fout << "\tlw	s0,12(sp)\n";
    fout << "\taddi\tsp,sp," << "16" << "\n";
}

/**
 * @brief 生成return语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::genInstReturn(const ir::Instruction &inst)
{
    // TODO; lab3todo12 genInstReturn
    if (inst.op1.type == ir::Type::IntLiteral) // 返回值类型为立即数
    {
        fout << "\tli\ta0," << std::stoi(inst.op1.name) << "\n";
        fout << "\tjr\tra\n";
    }
    else
    {
        assert(0 && "to be continue");
    }
}

/**
 * @brief 生成call语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::genInstCall(const ir::Instruction &inst)
{
    if (inst.op1.name == "global") // 不调用global函数
    {
        return;
    }
    else
    {
        assert(0 && "to be continue");
    }
}

/**
 * @brief 生成.option nopic
 * @note 指定生成的代码与位置无关，可以在内存任意位置运行
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::setOption()
{
    // TODO; lab3todo2 setOption
    fout << "\t.option nopic\n";
}

/**
 * @brief 生成.text相关内容
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::setText()
{
    // TODO; lab3todo3 setText
    fout << "\t.text\n";
    fout << "\t.align\t1\n";
}

/**
 * @brief 生成.global name
 * @param name 全局变量名/函数名
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::setGlobal(std::string name)
{
    // TODO; lab3todo5 setGlobal
    fout << "\t.global\t" << name << "\n";
}

/**
 * @brief 生成.type	name, @@function
 * @param name 函数名
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::setTypeFunc(std::string name)
{
    // TODO; lab3todo6 setTypeFunc
    fout << "\t.type\t" << name << ", @function\n";
}

/**
 * @brief 生成.type	name, @@object
 * @param name 全局变量名
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::setTypeObj(std::string name)
{
    // TODO; lab3todo7 setTypeObj
    fout << "\t.type\t" << name << ", @object\n";
}

/**
 * @brief 生成name:
 * @param name 标签名
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::setLabel(std::string name)
{
    // TODO; lab3todo8 setLabel
    fout << name << ":\n";
}
