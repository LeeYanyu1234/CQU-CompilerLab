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

// rv::rvREG backend::Generator::getRd(ir::Operand)
// {
//     TODO;
//     return rv::rvREG();
// }

// rv::rvFREG backend::Generator::fgetRd(ir::Operand)
// {
//     TODO;
//     return rv::rvFREG();
// }

// rv::rvREG backend::Generator::getRs1(ir::Operand)
// {
//     TODO;
//     return rv::rvREG();
// }

// rv::rvREG backend::Generator::getRs2(ir::Operand)
// {
//     TODO;
//     return rv::rvREG();
// }

// rv::rvFREG backend::Generator::fgetRs1(ir::Operand)
// {
//     TODO;
//     return rv::rvFREG();
// }

// rv::rvFREG backend::Generator::fgetRs2(ir::Operand)
// {
//     TODO;
//     return rv::rvFREG();
// }

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

    for (int i = 0; i < func.InstVec.size(); i++)
    {
        if (func.InstVec[i]->op == ir::Operator::def) // 处理全局普通整型变量
        {
            ir::Instruction *defInst = func.InstVec[i];
            ir::Instruction *movInst = func.InstVec[i + 1];
            setGlobal(movInst->des.name);
            setTypeObj(movInst->des.name);
            fout << "\t.size\t" << movInst->des.name << ", 4\n";
            setLabel(movInst->des.name);
            setIntInitVar(defInst->op1.name);
            initVal[movInst->des.name] = 1;
            i++;
        }
        else if (func.InstVec[i]->op == ir::Operator::alloc) // 处理数组
        {
            ir::Instruction *loadInst(func.InstVec[i]);
            setGlobal(loadInst->des.name);
            setTypeObj(loadInst->des.name);
            fout << "\t.size\t" << loadInst->des.name << ", " << stoi(loadInst->op1.name) * 4 << "\n";
            setLabel(loadInst->des.name);
            initVal[loadInst->des.name] = 1;
            for (int j = i + 1;; j++) // 处理数组的初始化
            {
                if (func.InstVec[j]->op == ir::Operator::store)
                {
                    ir::Instruction *storeInst(func.InstVec[j]);
                    if (storeInst->des.type == ir::Type::IntLiteral)
                    {
                        setIntInitVar(storeInst->op2.name);
                    }
                    else
                        assert(0 && "to be continue");
                }
                else
                {
                    i = j - 1;
                    break;
                }
            }
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

    for (auto globalVar : program.globalVal) // 处理未初始化的全局变量
    {
        if (initVal[globalVar.val.name] != 1)
        {
            if (globalVar.val.type == ir::Type::IntPtr) // 数组
            {
                fout << "\t.comm\t" << globalVar.val.name << ", " << globalVar.maxlen * 4 << ", 4\n";
            }
            else if (globalVar.val.type == ir::Type::Int) // 普通变量
            {
                fout << "\t.comm\t" << globalVar.val.name << ", 4, 4\n";
            }
            else
            {
                assert(0 && "to be continue");
            }
        }
    }
    fout.flush();
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
    else if (op == ir::Operator::alloc)
        genInstAlloc(inst);
    else if (op == ir::Operator::store)
        genInstStore(inst);
    else if (op == ir::Operator::mul)
        genInstMul(inst);
    else if (op == ir::Operator::load)
        genInstLoad(inst);
    else if (op == ir::Operator::sub)
        genInstSub(inst);
    else if (op == ir::Operator::div)
        genInstDiv(inst);
    else if (op == ir::Operator::mod)
        genInstMod(inst);
    else
        assert(0 && "to be continue");
}

/**
 * @brief 计算函数的栈大小，保存寄存器，加载参数
 * @param func
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
void backend::Generator::saveReg(const ir::Function &func)
{
    // TODO; lab3todo9 saveReg
    stackVarMap.clear();
    stackSize = 4; // 需要为保留ra寄存器分配一个栈空间

    for (auto fParam : func.ParameterList) // 扫描函数形参表，为每一个形参分配一个栈空间
    {
        addOperand(fParam);
    }

    for (auto inst : func.InstVec) // 扫描指令中出现的变量，为每一个局部变量分配一个栈空间
    {
        if (inst->des.type == ir::Type::Int)
        {
            if (!isGlobal(inst->des.name))
                addOperand(inst->des);
        }
        else if (inst->des.type == ir::Type::IntPtr && inst->op == ir::Operator::alloc && inst->op1.type == ir::Type::IntLiteral)
        {
            addOperand(inst->des, stoi(inst->op1.name) * 4);
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
    fout << "\taddi\tsp, sp, -" << stackSize << "\n";
    fout << "\tsw\tra, 0(sp)\n";

    // 把参数加载到栈
    for (int i = 0; i < func.ParameterList.size(); i++)
    {
        if (i <= 7)
        {
            fout << "\tsw\ta" << i << ", " << findOperand(func.ParameterList[i].name) << "(sp)\n";
        }
        else
        {
            assert(0 && "to be continue");
        }
    }
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
    fout << "\tlw\tra, 0(sp)\n";
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
        const auto *instPtr = &inst;
        auto callInstPtr = dynamic_cast<const ir::CallInst *>(instPtr); // 获取函数指针
        if (callInstPtr->argumentList.size() > 8)                       // 参数大于8个，已经超过参数寄存器上限，需要通过栈传参
            assert(0 && "to be continue");
        for (int i = 0; i < callInstPtr->argumentList.size(); i++)
        {
            if (i <= 7)
            {
                loadRegT5(callInstPtr->argumentList[i]);
                fout << "\tmv\ta" << i << ", t5\n";
            }
            else
                assert(0 && "to be continue");
        }
        fout << "\tcall\t" << inst.op1.name << "\n";
        fout << "\tsw\ta0, " << findOperand(inst.des.name) << "(sp)\n";
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
            fout << "\tlui\tt3, %hi(" << inst.des.name << ")\n";
            fout << "\taddi\tt3, t3, %lo(" << inst.des.name << ")\n"; // 全局变量地址在t3寄存器中
            fout << "\tlw\tt6, 0(t3)" << "\n";
        }
        else
        {
            fout << "\tlw\tt6, " << findOperand(inst.op1) << "(sp)" << "\n";
        }
        fout.flush();
        if (isGlobal(inst.des.name))
        {
            fout << "\tlui\tt3, %hi(" << inst.des.name << ")\n";
            fout << "\taddi\tt3, t3, %lo(" << inst.des.name << ")\n"; // 全局变量地址在t3寄存器中
            fout << "\tsw\tt6, 0(t3)" << "\n";
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
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tadd\t t5, t5, t4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成alloc语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::genInstAlloc(const ir::Instruction &inst)
{
    // TODO; lab3todo26 genInstAlloc
    //* 已经计算栈大小时分配空间，所以这里什么都不做
    return;
}

/**
 * @brief 生成store语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::genInstStore(const ir::Instruction &inst)
{
    // TODO; lab3todo27 genInstStore
    if (inst.des.type == ir::Type::Int && inst.op2.type == ir::Type::IntLiteral)
    {
        loadRegT5(inst.des);
        fout << "\tsw\tt5, " << findOperand(inst.op1) + stoi(inst.op2.name) * 4 << "(sp)\n";
    }
}

/**
 * @brief 生成mul语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::genInstMul(const ir::Instruction &inst)
{
    // TODO; lab3todo28 genInstMul
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tmul\t t5, t5, t4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成load语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::genInstLoad(const ir::Instruction &inst)
{
    // TODO; lab3todo29 genInstLoad
    if (isGlobal(inst.op1.name)) // 如果是全局变量数组
    {
        fout << "\tlui\tt3, %hi(" << inst.op1.name << ")\n";
        fout << "\taddi\tt3, t3, %lo(" << inst.op1.name << ")\n"; // 全局数组的基地址在t3寄存器中
        loadRegT4(inst.op2);                                      // 加载偏移量到t4寄存器
        fout << "\tslli\tt4, t4, 2\n";                            // 偏移量*4
        fout << "\tadd\tt3, t3, t4\n";                            // 计算地址
        fout << "\tlw\tt5, 0(t3)\n";
        storeRegT5(inst.des);
    }
    else // 如果是局部变量数组
    {
        loadRegT4(inst.op2);
        fout << "\tslli\tt4, t4, 2\n";
        fout << "\tadd\tt3, sp, t4\n";
        fout << "\tlw\tt5, " << findOperand(inst.op1) << "(t3)\n";
        storeRegT5(inst.des);
    }
}

/**
 * @brief 生成sub语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstSub(const ir::Instruction &inst)
{
    // TODO; lab3todo30 genInstSub
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tsub\t t5, t5, t4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成div语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstDiv(const ir::Instruction &inst)
{
    // TODO; lab3todo31 genInstDiv
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tdiv\t t5, t5, t4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成mod语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstMod(const ir::Instruction &inst)
{
    // TODO; lab3todo31 genInstDiv
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\trem\t t5, t5, t4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 把操作数1加载到t5寄存器
 * @param op
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::loadRegT5(const ir::Operand &op)
{
    // TODO; lab3todo23 loadRegT5
    if (isGlobal(op.name)) // 如果是全局变量
    {
        if (op.type == ir::Type::Int) // 如果是全局普通变量
        {
            fout << "\tlui\tt3, %hi(" << op.name << ")\n";
            fout << "\taddi\tt3, t3, %lo(" << op.name << ")\n"; // 全局变量地址在t3寄存器中
            fout << "\tlw\tt5, 0(t3)\n";
        }
        else
            assert(0 && "to be continue");
    }
    else
    {
        if (op.type == ir::Type::Int)
        {
            fout << "\tlw\tt5, " << findOperand(op) << "(sp)\n";
        }
        else
            assert(0 && "to be continue");
    }
}

/**
 * @brief 把操作数2加载到t4寄存器
 * @param op
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::loadRegT4(const ir::Operand &op)
{
    // TODO; lab3todo24 loadRegT4
    if (isGlobal(op.name)) // 如果是全局变量
    {
        if (op.type == ir::Type::Int) // 如果是全局普通变量
        {
            fout << "\tlui\tt3, %hi(" << op.name << ")\n";
            fout << "\taddi\tt3, t3, %lo(" << op.name << ")\n"; // 全局变量地址在t3寄存器中
            fout << "\tlw\tt4, 0(t3)\n";
        }
        else
            assert(0 && "to be continue");
    }
    else
    {
        if (op.type == ir::Type::Int)
        {
            fout << "\tlw\tt4, " << findOperand(op) << "(sp)\n";
        }
        else
            assert(0 && "to be continue");
    }
}

/**
 * @brief 把t5寄存器值放入栈中
 * @param op
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::storeRegT5(const ir::Operand &op)
{
    // TODO; lab3todo25 storeRegT5
    if (op.type == ir::Type::Int)
    {
        fout << "\tsw\tt5, " << findOperand(op) << "(sp)\n";
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
    if (stackVarMap.find(opd.name) == stackVarMap.end())
        assert(0 && "can not find opd in stack");
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
