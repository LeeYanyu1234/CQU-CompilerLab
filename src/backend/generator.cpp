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
// int backend::stackVarMap::find_operand(ir::Operand op)
// {
//     return 0;
// }

/**
 * @brief 向映射表中添加变量与偏移量的映射
 * @param op 加入的变量
 * @param size 变量大小
 * @return int 返回偏移量
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
// int backend::stackVarMap::add_operand(ir::Operand op, uint32_t size)
// {
//     return 0;
// }

void backend::Generator::gen()
{
    // TODO; lab3todo1 gen
    setOption();
    initGlobaVar(program.functions.front());

    setText();
    //* 跳过global函数，因为global已经单独处理
    for (int i = 1; i < program.functions.size(); i++)
    {
        gen_func(program.functions[i]);
    }
}

/**
 * @brief 初始化全局变量
 * @param func 全局函数
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::initGlobaVar(const ir::Function &func)
{
    // TODO; lab3todo14 initGlobaVar
    setData();

    std::map<std::string, int> initVal;

    for (int i = 0; i < func.InstVec.size(); i += 2)
    {
        if (func.InstVec[i]->op == ir::Operator::def)
        {
            ir::Instruction *defInst = func.InstVec[i];
            ir::Instruction *movInst = func.InstVec[i + 1];
            setGlobal(movInst->des.name);
            setTypeObj(movInst->des.name);
            fout << "\t.size\t" << movInst->des.name << ", 4\n";
            setLabel(movInst->des.name);
            setIntInitVar(defInst->op1.name);
            initVal[movInst->des.name] = 1;
        }
        else if (func.InstVec[i]->op == ir::Operator::_return)
        {
            break;
        }
        else
        {
            assert(0 && "to be continue");
        }
    }

    for (auto globalVar : program.globalVal)
    {
        if (initVal[globalVar.val.name] != 1)
        {
            if (globalVar.val.type == ir::Type::IntPtr)
            {
                fout << "\t.comm\t" << globalVar.val.name << ", " << globalVar.maxlen * 4 << ", 4\n";
            }
            else
            {
                assert(0 && "to be continue");
            }
        }
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

    fout << "\t.size\t" << func.name << ", .-" << func.name << "\n";
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
    else if (op == ir::Operator::def)
        genInstDef(inst);
    else if (op == ir::Operator::mov)
        genInstMov(inst);
    else if (op == ir::Operator::add)
        genInstAdd(inst);
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
    stackVarMap.clear();
    stackSize = 0;

    for (auto inst : func.InstVec)
    {
        if (inst->des.type == ir::Type::Int)
        {
            if (!isGlobal(inst->des.name))
                addOperand(inst->des);
        }
        if (inst->op1.type == ir::Type::Int)
        {
            if (!isGlobal(inst->op1.name))
                addOperand(inst->op1);
        }
        if (inst->op2.type == ir::Type::Int)
        {
            if (!isGlobal(inst->op2.name))
                addOperand(inst->op2);
        }
    }
    fout << "\taddi\tsp,sp,-" << stackSize << "\n";
    // fout << "\tsw	s0,12(sp)\n";
}

/**
 * @brief 恢复保存寄存器，介绍函数调用
 * @param func
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::recoverReg()
{
    // TODO; lab3todo10 recoverReg
    // fout << "\tlw	s0,12(sp)\n";
    fout << "\taddi\tsp,sp," << stackSize << "\n";
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
        fout << "\tli\ta0, " << std::stoi(inst.op1.name) << "\n";
        recoverReg();
        fout << "\tjr\tra\n";
    }
    else if (inst.op1.type == ir::Type::Int)
    {
        fout << "\tlw\ta0, " << findOperand(inst.op1.name) << "(sp)\n";
        recoverReg();
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
    // TODO; lab3todo13 genInstCall
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
 * @brief 生成def语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::genInstDef(const ir::Instruction &inst)
{
    // TODO; lab3todo20 genInstDef
    if (inst.op1.type == ir::Type::IntLiteral && inst.des.type == ir::Type::Int)
    {
        fout << "\tli\tt6, " << inst.op1.name << "\n";
        fout << "\tsw\tt6, " << findOperand(inst.des.name) << "(sp)" << "\n";
    }
    else
    {
        assert(0 && "to be continue");
    }
}

/**
 * @brief 生成mov语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::genInstMov(const ir::Instruction &inst)
{
    // TODO; lab3todo21 genInstMov
    if (inst.op1.type == ir::Type::Int && inst.des.type == ir::Type::Int)
    {
        if (isGlobal(inst.op1.name))
        {
            assert(0 && "to be continue");
        }
        else
        {
            fout << "\tlw\tt6, " << findOperand(inst.op1) << "(sp)" << "\n";
        }

        if (isGlobal(inst.des.name))
        {
            assert(0 && "to be continue");
        }
        else
        {
            fout << "\tsw\tt6, " << findOperand(inst.des) << "(sp)" << "\n";
        }
    }
    else
    {
        assert(0 && "to be continue");
    }
}

/**
 * @brief 生成mov语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::genInstAdd(const ir::Instruction &inst)
{
    // TODO; lab3todo22 genInstAdd
    loadVarOp1(inst.op1);
    loadVarOp2(inst.op2);
    fout << "\tadd\t t5, t5, t4\n";
    storeVarDes(inst.des);
    fout.flush();
}

void backend::Generator::loadVarOp1(const ir::Operand &op)
{
    // TODO; lab3todo23 loadVarOp1
    if (op.type == ir::Type::Int)
    {
        fout << "\tlw\tt5 ," << findOperand(op) << "(sp)\n";
    }
    else
        assert(0 && "to be continue");
}
void backend::Generator::loadVarOp2(const ir::Operand &op)
{
    // TODO; lab3todo24 loadVarOp2
    if (op.type == ir::Type::Int)
    {
        fout << "\tlw\tt4 ," << findOperand(op) << "(sp)\n";
    }
    else
        assert(0 && "to be continue");
}
void backend::Generator::storeVarDes(const ir::Operand &op)
{
    // TODO; lab3todo25 storeVarDes
    if (op.type == ir::Type::Int)
    {
        fout << "\tsw\tt5 ," << findOperand(op) << "(sp)\n";
    }
    else
        assert(0 && "to be continue");
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
 * @brief 生成.section .data
 * @note 由于储存已经初始化的全局变量
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::setData()
{
    // TODO; lab3todo15 initGlobaVar
    fout << "\t.section\t.data\n";
    fout << "\t.align\t2\n";
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

/**
 * @brief 设置整型全局变量的初值
 * @param val
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::setIntInitVar(std::string val)
{
    // TODO; lab3todo16 setIntInitVar
    fout << "\t.word\t" << val << "\n";
}

/**
 * @brief 判断变量是否为全局变量
 * @param name 变量名
 * @return bool
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
bool backend::Generator::isGlobal(const std::string &name)
{
    // TODO; lab3todo17 isGlobal
    for (auto var : program.globalVal)
        if (var.val.name == name)
            return true;
    return false;
}

/**
 * @brief 在映射表中查找局部变量
 * @param opd
 * @return int
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
int backend::Generator::findOperand(ir::Operand opd)
{
    // TODO; lab3todo18 findOperand
    return stackVarMap[opd.name];
}

/**
 * @brief 向映射表中添加变量与偏移量的映射
 * @param opd
 * @param size
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::addOperand(ir::Operand opd, uint32_t size)
{
    // TODO; lab3todo19 addOperand
    if (stackVarMap.find(opd.name) == stackVarMap.end())
    {
        stackVarMap[opd.name] = stackSize;
        stackSize += size;
    }
}
