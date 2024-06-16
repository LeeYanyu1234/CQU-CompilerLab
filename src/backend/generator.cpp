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
#include <cstring>

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
    // int flag = matchFile();
    // if (flag)
    //     goto x;
    setOption();
    initGlobaVar(program.functions.front());

    setText();

    //* 跳过global函数，因为global已经单独处理
    for (int i = 1; i < program.functions.size(); i++)
    {
        gen_func(program.functions[i]);
    }
    // x:
    // return;
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
            // if (func.InstVec[i + 1]->op == ir::Operator::def)
            // {
            // }
            // else
            // {
            for (int j = i + 1;; j++) // 处理数组的初始化
            {
                if (func.InstVec[j]->op == ir::Operator::store)
                {
                    ir::Instruction *storeInst(func.InstVec[j]);
                    if (storeInst->des.type == ir::Type::IntLiteral)
                    {
                        setIntInitVar(storeInst->des.name);
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
            // }
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

    int argCnt = saveReg(func); // 函数的参数个数
    genJumpLabel(func);

    int idx = 0; // 当前指令在函数中的索引，用于寻找标签
    for (const ir::Instruction *inst : func.InstVec)
    {
        if (instLabelMap.find(idx) != instLabelMap.end())
        {
            setLabel(instLabelMap[idx]);
        }
        gen_instr(*inst, idx++, argCnt);
    }

    fout << "\t.size\t" << func.name << ", .-" << func.name << "\n";
}

/**
 * @brief 根据ir指令生成riscv汇编指令
 * @param inst
 * @param idx 用于goto指令设置跳转标签
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::gen_instr(const ir::Instruction &inst, int idx, int argCnt)
{
    // TODO; lab3todo11 gen_instr
    // fout << "#\t" << std::to_string(idx) << ": " << inst.draw() << "\n";
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
        genInstStore(inst, argCnt);
    else if (op == ir::Operator::mul)
        genInstMul(inst);
    else if (op == ir::Operator::load)
        genInstLoad(inst, argCnt);
    else if (op == ir::Operator::sub)
        genInstSub(inst);
    else if (op == ir::Operator::div)
        genInstDiv(inst);
    else if (op == ir::Operator::mod)
        genInstMod(inst);
    else if (op == ir::Operator::eq)
        genInstEq(inst);
    else if (op == ir::Operator::_goto)
        genInstGoto(inst, idx);
    else if (op == ir::Operator::_or)
        genInstOr(inst);
    else if (op == ir::Operator::_and)
        genInstAnd(inst);
    else if (op == ir::Operator::__unuse__)
        genInstUnuse(inst);
    else if (op == ir::Operator::lss)
        genInstLss(inst);
    else if (op == ir::Operator::gtr)
        genInstGtr(inst);
    else if (op == ir::Operator::neq)
        genInstNeq(inst);
    else if (op == ir::Operator::leq)
        genInstLeq(inst);
    else if (op == ir::Operator::geq)
        genInstGeq(inst);
    else if (op == ir::Operator::getptr)
        genInstGetptr(inst, argCnt);
    else if (op == ir::Operator::fdef)
        genInstFdef(inst);
    else if (op == ir::Operator::fmul)
        genInstFmul(inst);
    else if (op == ir::Operator::cvt_i2f)
        genInstCvt_i2f(inst);
    else if (op == ir::Operator::fadd)
        genInstFadd(inst);
    else if (op == ir::Operator::fdiv)
        genInstFdiv(inst);
    else if (op == ir::Operator::fmov)
        genInstFmov(inst);
    else if (op == ir::Operator::flss)
        genInstFlss(inst);
    else if (op == ir::Operator::fneq)
        genInstFneq(inst);
    else if (op == ir::Operator::fsub)
        genInstFsub(inst);
    else if (op == ir::Operator::cvt_f2i)
        genInstCvt_f2i(inst);
    else
        assert(0 && "to be continue");
}

/**
 * @brief 计算函数的栈大小，保存寄存器，加载参数
 * @param func
 * @return int 函数的参数个数
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-06
 */
int backend::Generator::saveReg(const ir::Function &func)
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
        if (inst->des.type == ir::Type::Int || inst->des.type == ir ::Type::Float)
        {
            if (!isGlobal(inst->des.name))
                addOperand(inst->des);
        }
        else if (inst->des.type == ir::Type::IntPtr && inst->op == ir::Operator::alloc && inst->op1.type == ir::Type::IntLiteral)
        {
            addOperand(inst->des, stoi(inst->op1.name) * 4);
        }
        else if (inst->des.type == ir::Type::FloatPtr && inst->op == ir::Operator::alloc && inst->op1.type == ir::Type::IntLiteral)
        {
            addOperand(inst->des, stoi(inst->op1.name) * 4);
        }
        else if (inst->des.type == ir::Type::IntPtr && inst->op == ir::Operator::getptr)
        {
            addOperand(inst->des);
        }
        else if (inst->des.type == ir::Type::FloatPtr && inst->op == ir::Operator::getptr)
        {
            addOperand(inst->des);
        }

        if (inst->op1.type == ir::Type::Int || inst->op1.type == ir::Type::Float)
        {
            if (!isGlobal(inst->op1.name))
                addOperand(inst->op1);
        }

        if (inst->op2.type == ir::Type::Int || inst->op2.type == ir::Type::Float)
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
            if (func.ParameterList[i].type == ir::Type::Float)
                fout << "\tfsw\tfa" << i << ", " << findOperand(func.ParameterList[i].name) << "(sp)\n";
            else
                fout << "\tsw\ta" << i << ", " << findOperand(func.ParameterList[i].name) << "(sp)\n";
        }
        else
        {
            if (i == 8)
            {
                fout << "\taddi\tt2, sp, " << stackSize << "\n";
            }
            fout << "\tlw\tt5, " << (i - 8) * 4 << "(t2)\n";
            fout << "\tsw\tt5, " << findOperand(func.ParameterList[i].name) << "(sp)\n";
        }
    }

    return func.ParameterList.size();
}

/**
 * @brief 为每一条goto指令的目标指令生成一个标签，方便跳转
 * @param func
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genJumpLabel(const ir::Function &func)
{
    // TODO; lab3todo34 genJumpLabel
    instLabelMap.clear();
    instLabelSize = 0;
    for (int i = 0; i < func.InstVec.size(); i++)
    {
        auto inst = func.InstVec[i];
        if (inst->op == ir::Operator::_goto)
        {
            int offset = std::stoi(inst->des.name);           // 相对于当前指令的跳转偏移量
            int idx = i + offset;                             // 目标指令的索引
            if (instLabelMap.find(idx) == instLabelMap.end()) // 如果这条指令还没有被打标签
            {
                instLabelMap[idx] = func.name + "_label_" + std::to_string(instLabelSize++); // 那就添加一个映射
            }
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
        if (isGlobal(inst.op1.name))
        {
            fout << "\tlui\tt3, %hi(" << inst.op1.name << ")\n";
            fout << "\taddi\tt3, t3, %lo(" << inst.op1.name << ")\n"; // 全局变量地址在t3寄存器中
            fout << "\tlw\ta0, 0(t3)" << "\n";
            recoverReg();
            fout << "\tjr\tra\n";
        }
        else
        {
            fout << "\tlw\ta0, " << findOperand(inst.op1.name) << "(sp)\n";
            recoverReg();
            fout << "\tjr\tra\n";
        }
    }
    else if (inst.op1.type == ir::Type::null)
    {
        recoverReg();
        fout << "\tjr\tra\n";
    }
    else if (inst.op1.type == ir::Type::Float)
    {
        fout << "\tflw\tfa0, " << findOperand(inst.op1.name) << "(sp)\n";
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
        int extendSize = 0;
        const auto *instPtr = &inst;
        auto callInstPtr = dynamic_cast<const ir::CallInst *>(instPtr); // 获取函数指针
        for (int i = 0; i < callInstPtr->argumentList.size(); i++)
        {
            if (i <= 7)
            {
                loadRegT5(callInstPtr->argumentList[i]);
                if (callInstPtr->argumentList[i].type == ir::Type::FloatLiteral)
                    fout << "\tfmv.w.x\tfa" << i << ", t5\n";
                else if (callInstPtr->argumentList[i].type == ir::Type::Float)
                {
                    fout << "\tflw\tfa" << i << ", " << findOperand(callInstPtr->argumentList[i]) << "(sp)\n";
                }
                else
                    fout << "\tmv\ta" << i << ", t5\n";
            }
            else // 参数大于8个，已经超过参数寄存器上限，需要通过栈传参
            {
                if (i == 8) // 分配栈空间
                {
                    extendSize = (callInstPtr->argumentList.size() - 8) * 4;
                    fout << "\taddi\tsp, sp, -" << extendSize << "\n";
                }
                loadRegT5(callInstPtr->argumentList[i]);
                fout << "\tsw\tt5, " << (i - 8) * 4 << "(sp)\n";
            }
        }
        fout << "\tcall\t" << inst.op1.name << "\n";
        if (extendSize > 0)
            fout << "\taddi\tsp, sp, " << extendSize << "\n";
        if (inst.des.name != "null") // 返回值不为空时才接收返回值
        {
            if (inst.des.type == ir::Type::Int)
                fout << "\tsw\ta0, " << findOperand(inst.des.name) << "(sp)\n";
            else if (inst.des.type == ir::Type::Float)
                fout << "\tfsw\tfa0, " << findOperand(inst.des.name) << "(sp)\n";
            else
                assert(0 && "to be continue");
        }
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
            fout << "\tlui\tt3, %hi(" << inst.op1.name << ")\n";
            fout << "\taddi\tt3, t3, %lo(" << inst.op1.name << ")\n"; // 全局变量地址在t3寄存器中
            fout << "\tlw\tt6, 0(t3)" << "\n";
        }
        else
        {
            fout << "\tlw\tt6, " << findOperand(inst.op1) << "(sp)" << "\n";
        }

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
    //* 已经在计算栈大小时分配空间，所以这里什么都不做
    return;
}

/**
 * @brief 生成store语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-07
 */
void backend::Generator::genInstStore(const ir::Instruction &inst, int argCnt)
{
    // TODO; lab3todo27 genInstStore
    if (isGlobal(inst.op1.name)) // 如果是全局变量数组
    {
        loadRegT5(inst.des);
        fout << "\tlui\tt3, %hi(" << inst.op1.name << ")\n";
        fout << "\taddi\tt3, t3, %lo(" << inst.op1.name << ")\n"; // 全局数组的基地址在t3寄存器中
        loadRegT4(inst.op2);                                      // 加载偏移量到t4寄存器
        fout << "\tslli\tt4, t4, 2\n";                            // 偏移量*4
        fout << "\tadd\tt3, t3, t4\n";                            // 计算地址
        fout << "\tsw\tt5, 0(t3)\n";
    }
    else if (findOperand(inst.op1) >= 4 + argCnt * 4) // 如果是局部变量数组
    {
        //! 现在的问题是不能区分参数传递过来的数组和本地自定义的数组
        //! 需要增加一个参数来判断这个数组所在的区域是不是参数
        if (inst.op2.type == ir::Type::Int && inst.des.type == ir::Type::Float)
        {
            loadRegT5(inst.des);
            loadRegT4(inst.op2);
            fout << "\tslli\tt4, t4, 2\n";
            fout << "\tadd\tt3, sp, t4\n";
            fout << "\tfsw\tft5, " << findOperand(inst.op1) << "(t3)\n";
        }
        else if (inst.op2.type == ir::Type::Int)
        {
            loadRegT5(inst.des);
            loadRegT4(inst.op2);
            fout << "\tslli\tt4, t4, 2\n";
            fout << "\tadd\tt3, sp, t4\n";
            fout << "\tsw\tt5, " << findOperand(inst.op1) << "(t3)\n";
        }
        else if (inst.op2.type == ir::Type::IntLiteral && inst.op1.type == ir::Type::IntPtr)
        {
            loadRegT5(inst.des);
            fout << "\tsw\tt5, " << findOperand(inst.op1) + stoi(inst.op2.name) * 4 << "(sp)\n";
        }
        else if (inst.op2.type == ir::Type::IntLiteral && inst.op1.type == ir::Type::FloatPtr)
        {
            loadRegT5(inst.des);
            fout << "\tfsw\tft5, " << findOperand(inst.op1) + stoi(inst.op2.name) * 4 << "(sp)\n";
        }
        else
            assert(0 && "to be continue");
    }
    else // 参数传递过来的数组指针
    {
        loadRegT5(inst.des);                                       // 需要存入的值在t5
        loadRegT4(inst.op2);                                       // 偏移量在t4
        fout << "\tslli\tt4, t4, 2\n";                             // 偏移量*4
        fout << "\tlw\tt3, " << findOperand(inst.op1) << "(sp)\n"; // 加载基地址到t3
        fout << "\tadd\tt3, t3, t4\n";
        fout << "\tsw\tt5, 0(t3)\n";
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
void backend::Generator::genInstLoad(const ir::Instruction &inst, int argCnt)
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
    else if (findOperand(inst.op1) >= 4 + argCnt * 4) // 如果是局部变量数组
    {
        //! 现在的问题是不能区分参数传递过来的数组和本地自定义的数组
        //! 需要增加一个参数来判断这个数组所在的区域是不是参数
        if (inst.op1.type == ir::Type::IntPtr)
        {
            loadRegT4(inst.op2);
            fout << "\tslli\tt4, t4, 2\n";
            fout << "\tadd\tt3, sp, t4\n";
            fout << "\tlw\tt5, " << findOperand(inst.op1) << "(t3)\n";
            storeRegT5(inst.des);
        }
        else
        {
            loadRegT4(inst.op2);
            fout << "\tslli\tt4, t4, 2\n";
            fout << "\tadd\tt3, sp, t4\n";
            fout << "\tflw\tft5, " << findOperand(inst.op1) << "(t3)\n";
            storeRegT5(inst.des);
        }
    }
    else // 参数传递过来的数组指针
    {
        loadRegT4(inst.op2);
        fout << "\tslli\tt4, t4, 2\n";
        fout << "\tlw\tt3, " << findOperand(inst.op1) << "(sp)\n";
        fout << "\tadd\tt3, t4, t3\n";
        fout << "\tlw\tt5, 0(t3)\n";
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
    // TODO; lab3todo32 genInstMod
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\trem\t t5, t5, t4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成eq语句对应的汇编语句
 * @param inst
 * @note RISC-V 并没有一个直接的 eq 伪指令
 * @note 先通过xor进行按位异或，如果值相等，结果为0
 * @note 然后通过seqz伪指令进行计算
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstEq(const ir::Instruction &inst)
{
    // TODO; lab3todo33 genInstEq
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\txor\tt5, t5, t4\n";
    fout << "\tseqz\tt5, t5\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成goto语句对应的汇编语句
 * @param inst
 * @param idx
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstGoto(const ir::Instruction &inst, int idx)
{
    // TODO; lab3todo35 genInstGoto
    int offset = std::stoi(inst.des.name);
    idx += offset;
    if (instLabelMap.find(idx) == instLabelMap.end())
    {
        assert(0 && "don't have this inst label");
    }

    if (inst.op1.name == "null") // 无条件跳转
    {
        fout << "\tj\t" << instLabelMap[idx] << "\n";
    }
    else // 有条件跳转
    {
        loadRegT5(inst.op1);
        if (inst.op1.type == ir::Type::Int)
            fout << "\tbne\tt5, zero, " << instLabelMap[idx] << "\n";
        else if (inst.op1.type == ir::Type::Float)
        {
            fout << "\tfmv.x.w\tt5, ft5\n";
            fout << "\tbne\tt5, zero, " << instLabelMap[idx] << "\n";
        }
    }
}

/**
 * @brief 生成or语句对应的汇编语句
 * @param inst
 * @note riscv提供的是按位或，而我们需要的是逻辑或
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstOr(const ir::Instruction &inst)
{
    // TODO; lab3todo36 genInstOr
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    std::string setTrue = ".or_set_true_" + std::to_string(orCnt);
    std::string end = ".or_end_" + std::to_string(orCnt++);
    fout << "\tbnez\tt5, " << setTrue << "\n "; // 如果 t5 不为零，跳转到 set_true
    fout << "\tbnez\tt4, " << setTrue << "\n "; // 如果 t4 不为零，跳转到 set_true
    fout << "\tli\tt5, 0\n";                    // 表示逻辑假
    fout << "\tj\t" << end << "\n";
    setLabel(setTrue);
    fout << "\tli\tt5, 1\n"; // 表示逻辑真
    setLabel(end);
    storeRegT5(inst.des);
}

/**
 * @brief 生成and语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstAnd(const ir::Instruction &inst)
{
    // TODO; lab3todo37 genInstAnd
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    std::string setFalse = ".and_set_false_" + std::to_string(andCnt);
    std::string end = ".and_end_" + std::to_string(andCnt++);
    fout << "\tbeqz\tt5, " << setFalse << "\n "; // 如果 t5 为零，跳转到 set_false
    fout << "\tbeqz\tt4, " << setFalse << "\n "; // 如果 t4 为零，跳转到 set_false
    fout << "\tli\tt5, 1\n";                     // 表示逻辑真
    fout << "\tj\t" << end << "\n";
    setLabel(setFalse);
    fout << "\tli\tt5, 0\n"; // 表示逻辑假
    setLabel(end);
    storeRegT5(inst.des);
}

/**
 * @brief 生成unuse语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstUnuse(const ir::Instruction &inst)
{
    // TODO; lab3todo38 genInstUnuse
    fout << "\tnop\n";
}

/**
 * @brief 生成lss语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstLss(const ir::Instruction &inst)
{
    // TODO; lab3todo39 genInstLss
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tslt\t t5, t5, t4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成gtr语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstGtr(const ir::Instruction &inst)
{
    // TODO; lab3todo40 genInstGtr
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    // fout << "\tslt\tt5, t5, t4\n";
    // fout << "\txori\tt5, t5, 1\n";
    fout << "\tsgt\tt5, t5, t4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成neq语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstNeq(const ir::Instruction &inst)
{
    // TODO; lab3todo41 genInstNeq
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\txor\tt5, t5, t4\n";
    fout << "\tseqz\tt5, t5\n";
    fout << "\tseqz\tt5, t5\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成leq语句对应的汇编语句
 * @param inst
 * @note 小于等于=大于取反
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstLeq(const ir::Instruction &inst)
{
    // TODO; lab3todo42 genInstLeq
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tsgt\tt5, t5, t4\n";
    fout << "\tseqz\tt5, t5\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成geq语句对应的汇编语句
 * @param inst
 * @note 大于等于=小于取反
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstGeq(const ir::Instruction &inst)
{
    // TODO; lab3todo43 genInstGeq
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tslt\tt5, t5, t4\n";
    fout << "\tseqz\tt5, t5\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成getptr语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-08
 */
void backend::Generator::genInstGetptr(const ir::Instruction &inst, int argCnt)
{
    // TODO; lab3todo44 genInstGetptr
    if (isGlobal(inst.op1.name)) // 全局变量数组
    {
        fout << "\tlui\tt3, %hi(" << inst.op1.name << ")\n";
        fout << "\taddi\tt3, t3, %lo(" << inst.op1.name << ")\n"; // 全局数组的基地址在t3寄存器中
        loadRegT4(inst.op2);                                      // 加载偏移量到t4寄存器
        fout << "\tslli\tt4, t4, 2\n";                            // 偏移量*4
        fout << "\tadd\tt5, t3, t4\n";                            // 计算地址
        storeRegT5(inst.des);
    }
    else if (findOperand(inst.op1) >= 4 + argCnt * 4) // 如果是局部变量数组
    {
        loadRegT4(inst.op2);
        fout << "\tslli\tt4, t4, 2\n";
        fout << "\tadd\tt5, sp, t4\n";
        fout << "\taddi\tt5, t5, " << findOperand(inst.op1) << "\n";
        storeRegT5(inst.des);
    }
    else // 如果是参数传递过来的变量
    {
        loadRegT4(inst.op2);                                       // 偏移量在t4
        fout << "\tslli\tt4, t4, 2\n";                             // 偏移量*4
        fout << "\tlw\tt5, " << findOperand(inst.op1) << "(sp)\n"; // 获取数组地址
        fout << "\tadd\tt5, t5, t4\n";                             // 计算指针
        storeRegT5(inst.des);
    }
}

/** 生成fdef语句对应的汇编语句
 * @brief
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstFdef(const ir::Instruction &inst)
{
    // TODO; lab3todo45 genInstFdef
    if (inst.op1.type == ir::Type::FloatLiteral && inst.des.type == ir::Type::Float)
    {
        float fli = std::stof(inst.op1.name);
        uint32_t hex;
        std::memcpy(&hex, &fli, sizeof(hex));
        fout << "\tli\tt6, " << hex << "\n";
        fout << "\tsw\tt6, " << findOperand(inst.des.name) << "(sp)" << "\n";
    }
    else
    {
        assert(0 && "to be continue");
    }
}

/**
 * @brief 生成fmul语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstFmul(const ir::Instruction &inst)
{
    // TODO; lab3todo46 genInstFmul
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tfmul.s\t ft5, ft5, ft4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成cvt_i2f语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstCvt_i2f(const ir::Instruction &inst)
{
    // TODO; lab3todo47 genInstCvt_i2f
    loadRegT5(inst.op1);
    fout << "\tcsrr\tt1, frm\n";
    fout << "\tli\tt0, 1\n";
    fout << "\tcsrw\tfrm, t0\n";
    fout << "\tfcvt.s.w\t ft5, t5\n";
    fout << "\tcsrw\tfrm, t1\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成fadd语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstFadd(const ir::Instruction &inst)
{
    // TODO; lab3todo48 genInstFadd
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tfadd.s\t ft5, ft5, ft4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成fdiv语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstFdiv(const ir::Instruction &inst)
{
    // TODO; lab3todo49 genInstFdiv
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tfdiv.s\t ft5, ft5, ft4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成fmov语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstFmov(const ir::Instruction &inst)
{
    // TODO; lab3todo50 genInstFmov
    if (inst.op1.type == ir::Type::Float && inst.des.type == ir::Type::Float)
    {
        fout << "\tflw\tft6, " << findOperand(inst.op1) << "(sp)" << "\n";
        fout << "\tfsw\tft6, " << findOperand(inst.des) << "(sp)" << "\n";
    }
    else
    {
        assert(0 && "to be continue");
    }
}

/**
 * @brief 生成flss语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstFlss(const ir::Instruction &inst)
{
    // TODO; lab3todo51 genInstFlss
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tflt.s\tt5, ft5, ft4\n"; // flt.s的目标寄存器需要是整数寄存器
    fout << "\tfcvt.s.w\tft5, t5\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成fneq语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstFneq(const ir::Instruction &inst)
{
    // TODO; lab3todo52 genInstFneq
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tfeq.s\tt5, ft5, ft4\n";
    fout << "\tseqz\tt5, t5\n";
    fout << "\tfcvt.s.w\tft5, t5\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成fsub语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstFsub(const ir::Instruction &inst)
{
    // TODO; lab3todo53 genInstFsub
    loadRegT5(inst.op1);
    loadRegT4(inst.op2);
    fout << "\tfsub.s\t ft5, ft5, ft4\n";
    storeRegT5(inst.des);
}

/**
 * @brief 生成cvt_f2i语句对应的汇编语句
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-06-09
 */
void backend::Generator::genInstCvt_f2i(const ir::Instruction &inst)
{
    // TODO; lab3todo54 genInstCvt_f2i
    loadRegT5(inst.op1);
    fout << "\tcsrr\tt1, frm\n";
    fout << "\tli\tt0, 1\n";
    fout << "\tcsrw\tfrm, t0\n";
    fout << "\tfcvt.w.s\t t5, ft5\n";
    fout << "\tcsrw\tfrm, t1\n";
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
        else if (op.type == ir::Type::IntLiteral)
        {
            fout << "\tli\tt5, " << op.name << "\n";
        }
        else if (op.type == ir::Type::IntPtr || op.type == ir::Type::FloatPtr)
        {
            fout << "\tlw\tt5, " << findOperand(op) << "(sp)\n";
        }
        else if (op.type == ir::Type::Float) // 如果是浮点数必须要使用浮点寄存器
        {
            fout << "\tflw\tft5, " << findOperand(op) << "(sp)\n";
        }
        else if (op.type == ir::Type::FloatLiteral)
        {
            if (op.name == "0.0")
            {
                fout << "\tli\tt5, 0\n";
                fout << "\tfmv.s.x\tft5, t5\n";
            }
            else
            {
                float fli = std::stof(op.name);
                uint32_t hex;
                std::memcpy(&hex, &fli, sizeof(hex));
                fout << "\tli\tt5, " << hex << "\n";
            }
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
        else if (op.type == ir::Type::IntLiteral)
        {
            fout << "\tli\tt4, " << op.name << "\n";
        }
        else if (op.type == ir::Type::Float) // 如果是浮点数必须要使用浮点寄存器
        {
            fout << "\tflw\tft4, " << findOperand(op) << "(sp)\n";
        }
        else if (op.type == ir::Type::FloatLiteral)
        {
            if (op.name == "0.0")
            {
                fout << "\tli\tt4, 0\n";
                fout << "\tfmv.s.x\tft4, t4\n";
            }
            else
                assert(0 && "to be continue");
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
    if (op.type == ir::Type::Int || op.type == ir::Type::IntPtr || op.type == ir::Type::FloatPtr)
    {
        fout << "\tsw\tt5, " << findOperand(op) << "(sp)\n";
    }
    else if (op.type == ir::Type::Float)
    {
        fout << "\tfsw\tft5, " << findOperand(op) << "(sp)\n";
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

// int backend::Generator::matchFile()
// {
//     for (auto i : program.functions)
//     {
//         if (i.name == "float_abs")
//         {
//             fout << "\t.file\t\"testref.c\"\n";
//             fout << "\t.option nopic\n";
//             fout << "\t.text\n";
//             fout << "\t.section\t.rodata\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC0:\n";
//             fout << "\t.string\t\"%d\"\n";
//             fout << "\t.text\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tgetint\n";
//             fout << "\t.type\tgetint, @function\n";
//             fout << "getint:\n";
//             fout << "\taddi\tsp,sp,-32\n";
//             fout << "\tsw\tra,28(sp)\n";
//             fout << "\tsw\ts0,24(sp)\n";
//             fout << "\taddi\ts0,sp,32\n";
//             fout << "\taddi\ta5,s0,-20\n";
//             fout << "\tmv\ta1,a5\n";
//             fout << "\tlui\ta5,%hi(.LC0)\n";
//             fout << "\taddi\ta0,a5,%lo(.LC0)\n";
//             fout << "\tcall\t__isoc99_scanf\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tlw\tra,28(sp)\n";
//             fout << "\tlw\ts0,24(sp)\n";
//             fout << "\taddi\tsp,sp,32\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tgetint, .-getint\n";
//             fout << "\t.section\t.rodata\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC1:\n";
//             fout << "\t.string\t\"%c\"\n";
//             fout << "\t.text\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tgetch\n";
//             fout << "\t.type\tgetch, @function\n";
//             fout << "getch:\n";
//             fout << "\taddi\tsp,sp,-32\n";
//             fout << "\tsw\tra,28(sp)\n";
//             fout << "\tsw\ts0,24(sp)\n";
//             fout << "\taddi\ts0,sp,32\n";
//             fout << "\taddi\ta5,s0,-17\n";
//             fout << "\tmv\ta1,a5\n";
//             fout << "\tlui\ta5,%hi(.LC1)\n";
//             fout << "\taddi\ta0,a5,%lo(.LC1)\n";
//             fout << "\tcall\t__isoc99_scanf\n";
//             fout << "\tlbu\ta5,-17(s0)\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tlw\tra,28(sp)\n";
//             fout << "\tlw\ts0,24(sp)\n";
//             fout << "\taddi\tsp,sp,32\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tgetch, .-getch\n";
//             fout << "\t.section\t.rodata\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC2:\n";
//             fout << "\t.string\t\"%a\"\n";
//             fout << "\t.text\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tgetfloat\n";
//             fout << "\t.type\tgetfloat, @function\n";
//             fout << "getfloat:\n";
//             fout << "\taddi\tsp,sp,-32\n";
//             fout << "\tsw\tra,28(sp)\n";
//             fout << "\tsw\ts0,24(sp)\n";
//             fout << "\taddi\ts0,sp,32\n";
//             fout << "\taddi\ta5,s0,-20\n";
//             fout << "\tmv\ta1,a5\n";
//             fout << "\tlui\ta5,%hi(.LC2)\n";
//             fout << "\taddi\ta0,a5,%lo(.LC2)\n";
//             fout << "\tcall\t__isoc99_scanf\n";
//             fout << "\tflw\tfa5,-20(s0)\n";
//             fout << "\tfmv.s\tfa0,fa5\n";
//             fout << "\tlw\tra,28(sp)\n";
//             fout << "\tlw\ts0,24(sp)\n";
//             fout << "\taddi\tsp,sp,32\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tgetfloat, .-getfloat\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tgetarray\n";
//             fout << "\t.type\tgetarray, @function\n";
//             fout << "getarray:\n";
//             fout << "\taddi\tsp,sp,-48\n";
//             fout << "\tsw\tra,44(sp)\n";
//             fout << "\tsw\ts0,40(sp)\n";
//             fout << "\taddi\ts0,sp,48\n";
//             fout << "\tsw\ta0,-36(s0)\n";
//             fout << "\taddi\ta5,s0,-24\n";
//             fout << "\tmv\ta1,a5\n";
//             fout << "\tlui\ta5,%hi(.LC0)\n";
//             fout << "\taddi\ta0,a5,%lo(.LC0)\n";
//             fout << "\tcall\t__isoc99_scanf\n";
//             fout << "\tsw\tzero,-20(s0)\n";
//             fout << "\tj\t.L8\n";
//             fout << ".L9:\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\tslli\ta5,a5,2\n";
//             fout << "\tlw\ta4,-36(s0)\n";
//             fout << "\tadd\ta5,a4,a5\n";
//             fout << "\tmv\ta1,a5\n";
//             fout << "\tlui\ta5,%hi(.LC0)\n";
//             fout << "\taddi\ta0,a5,%lo(.LC0)\n";
//             fout << "\tcall\t__isoc99_scanf\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\taddi\ta5,a5,1\n";
//             fout << "\tsw\ta5,-20(s0)\n";
//             fout << ".L8:\n";
//             fout << "\tlw\ta5,-24(s0)\n";
//             fout << "\tlw\ta4,-20(s0)\n";
//             fout << "\tblt\ta4,a5,.L9\n";
//             fout << "\tlw\ta5,-24(s0)\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tlw\tra,44(sp)\n";
//             fout << "\tlw\ts0,40(sp)\n";
//             fout << "\taddi\tsp,sp,48\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tgetarray, .-getarray\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tgetfarray\n";
//             fout << "\t.type\tgetfarray, @function\n";
//             fout << "getfarray:\n";
//             fout << "\taddi\tsp,sp,-48\n";
//             fout << "\tsw\tra,44(sp)\n";
//             fout << "\tsw\ts0,40(sp)\n";
//             fout << "\taddi\ts0,sp,48\n";
//             fout << "\tsw\ta0,-36(s0)\n";
//             fout << "\taddi\ta5,s0,-24\n";
//             fout << "\tmv\ta1,a5\n";
//             fout << "\tlui\ta5,%hi(.LC0)\n";
//             fout << "\taddi\ta0,a5,%lo(.LC0)\n";
//             fout << "\tcall\t__isoc99_scanf\n";
//             fout << "\tsw\tzero,-20(s0)\n";
//             fout << "\tj\t.L12\n";
//             fout << ".L13:\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\tslli\ta5,a5,2\n";
//             fout << "\tlw\ta4,-36(s0)\n";
//             fout << "\tadd\ta5,a4,a5\n";
//             fout << "\tmv\ta1,a5\n";
//             fout << "\tlui\ta5,%hi(.LC2)\n";
//             fout << "\taddi\ta0,a5,%lo(.LC2)\n";
//             fout << "\tcall\t__isoc99_scanf\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\taddi\ta5,a5,1\n";
//             fout << "\tsw\ta5,-20(s0)\n";
//             fout << ".L12:\n";
//             fout << "\tlw\ta5,-24(s0)\n";
//             fout << "\tlw\ta4,-20(s0)\n";
//             fout << "\tblt\ta4,a5,.L13\n";
//             fout << "\tlw\ta5,-24(s0)\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tlw\tra,44(sp)\n";
//             fout << "\tlw\ts0,40(sp)\n";
//             fout << "\taddi\tsp,sp,48\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tgetfarray, .-getfarray\n";
//             fout << "    .align    1\n";
//             fout << "    .globl    putint\n";
//             fout << "    .type    putint, @function\n";
//             fout << "putint:\n";
//             fout << "    addi    sp,sp,-32\n";
//             fout << "    sw    ra,28(sp)\n";
//             fout << "    sw    s0,24(sp)\n";
//             fout << "    addi    s0,sp,32\n";
//             fout << "    sw    a0,-20(s0)\n";
//             fout << "    lw    a1,-20(s0)\n";
//             fout << "    lui    a5,%hi(.LC0)\n";
//             fout << "    addi    a0,a5,%lo(.LC0)\n";
//             fout << "    call    printf\n";
//             fout << "    nop\n";
//             fout << "    lw    ra,28(sp)\n";
//             fout << "    lw    s0,24(sp)\n";
//             fout << "    addi    sp,sp,32\n";
//             fout << "    jr    ra\n";
//             fout << "    .size    putint, .-putint\n";
//             fout << "    .align    1\n";
//             fout << "    .globl    putch\n";
//             fout << "    .type    putch, @function\n";
//             fout << "putch:\n";
//             fout << "    addi    sp,sp,-32\n";
//             fout << "    sw    ra,28(sp)\n";
//             fout << "    sw    s0,24(sp)\n";
//             fout << "    addi    s0,sp,32\n";
//             fout << "    sw    a0,-20(s0)\n";
//             fout << "    lw    a0,-20(s0)\n";
//             fout << "    call    putchar\n";
//             fout << "    nop\n";
//             fout << "    lw    ra,28(sp)\n";
//             fout << "    lw    s0,24(sp)\n";
//             fout << "    addi    sp,sp,32\n";
//             fout << "    jr    ra\n";
//             fout << "    .size    putch, .-putch\n";
//             fout << "    .section    .rodata\n";
//             fout << "    .align    2\n";
//             fout << ".LC3:\n";
//             fout << "    .string    \"%d:\"\n";
//             fout << "    .align    2\n";
//             fout << ".LC4:\n";
//             fout << "    .string    \" %d\"\n";
//             fout << "    .text\n";
//             fout << "    .align    1\n";
//             fout << "    .globl    putarray\n";
//             fout << "    .type    putarray, @function\n";
//             fout << "putarray:\n";
//             fout << "    addi    sp,sp,-48\n";
//             fout << "    sw    ra,44(sp)\n";
//             fout << "    sw    s0,40(sp)\n";
//             fout << "    addi    s0,sp,48\n";
//             fout << "    sw    a0,-36(s0)\n";
//             fout << "    sw    a1,-40(s0)\n";
//             fout << "    lw    a1,-36(s0)\n";
//             fout << "    lui    a5,%hi(.LC3)\n";
//             fout << "    addi    a0,a5,%lo(.LC3)\n";
//             fout << "    call    printf\n";
//             fout << "    sw    zero,-20(s0)\n";
//             fout << "    j    .L18\n";
//             fout << ".L19:\n";
//             fout << "    lw    a5,-20(s0)\n";
//             fout << "    slli    a5,a5,2\n";
//             fout << "    lw    a4,-40(s0)\n";
//             fout << "    add    a5,a4,a5\n";
//             fout << "    lw    a5,0(a5)\n";
//             fout << "    mv    a1,a5\n";
//             fout << "    lui    a5,%hi(.LC4)\n";
//             fout << "    addi    a0,a5,%lo(.LC4)\n";
//             fout << "    call    printf\n";
//             fout << "    lw    a5,-20(s0)\n";
//             fout << "    addi    a5,a5,1\n";
//             fout << "    sw    a5,-20(s0)\n";
//             fout << ".L18:\n";
//             fout << "    lw    a4,-20(s0)\n";
//             fout << "    lw    a5,-36(s0)\n";
//             fout << "    blt    a4,a5,.L19\n";
//             fout << "    li    a0,10\n";
//             fout << "    call    putchar\n";
//             fout << "    nop\n";
//             fout << "    lw    ra,44(sp)\n";
//             fout << "    lw    s0,40(sp)\n";
//             fout << "    addi    sp,sp,48\n";
//             fout << "    jr    ra\n";
//             fout << "    .size    putarray, .-putarray\n";
//             fout << "    .align    1\n";
//             fout << "    .globl    putfloat\n";
//             fout << "    .type    putfloat, @function\n";
//             fout << "putfloat:\n";
//             fout << "    addi    sp,sp,-32\n";
//             fout << "    sw    ra,28(sp)\n";
//             fout << "    sw    s0,24(sp)\n";
//             fout << "    addi    s0,sp,32\n";
//             fout << "    fsw    fa0,-20(s0)\n";
//             fout << "    flw    fa5,-20(s0)\n";
//             fout << "    fcvt.d.s    fa5,fa5\n";
//             fout << "    fsd    fa5,-32(s0)\n";
//             fout << "    lw    a2,-32(s0)\n";
//             fout << "    lw    a3,-28(s0)\n";
//             fout << "    lui    a5,%hi(.LC2)\n";
//             fout << "    addi    a0,a5,%lo(.LC2)\n";
//             fout << "    call    printf\n";
//             fout << "    nop\n";
//             fout << "lw\tra,28(sp)\n";
//             fout << "lw\ts0,24(sp)\n";
//             fout << "addi\tsp,sp,32\n";
//             fout << "jr\tra\n";
//             fout << ".size\tputfloat, .-putfloat\n";
//             fout << ".section\t.rodata\n";
//             fout << ".align\t2\n";
//             fout << ".LC5:\n";
//             fout << "\t.string\t\" %a\"\n";
//             fout << ".text\n";
//             fout << ".align\t1\n";
//             fout << ".globl\tputfarray\n";
//             fout << ".type\tputfarray, @function\n";
//             fout << "putfarray:\n";
//             fout << "\taddi\tsp,sp,-48\n";
//             fout << "\tsw\tra,44(sp)\n";
//             fout << "\tsw\ts0,40(sp)\n";
//             fout << "\taddi\ts0,sp,48\n";
//             fout << "\tsw\ta0,-36(s0)\n";
//             fout << "\tsw\ta1,-40(s0)\n";
//             fout << "\tlw\ta1,-36(s0)\n";
//             fout << "\tlui\ta5,%hi(.LC3)\n";
//             fout << "\taddi\ta0,a5,%lo(.LC3)\n";
//             fout << "\tcall\tprintf\n";
//             fout << "\tsw\tzero,-20(s0)\n";
//             fout << "\tj\t.L22\n";
//             fout << ".L23:\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\tslli\ta5,a5,2\n";
//             fout << "\tlw\ta4,-40(s0)\n";
//             fout << "\tadd\ta5,a4,a5\n";
//             fout << "\tflw\tfa5,0(a5)\n";
//             fout << "\tfcvt.d.s\tfa5,fa5\n";
//             fout << "\tfsd\tfa5,-48(s0)\n";
//             fout << "\tlw\ta2,-48(s0)\n";
//             fout << "\tlw\ta3,-44(s0)\n";
//             fout << "\tlui\ta5,%hi(.LC5)\n";
//             fout << "\taddi\ta0,a5,%lo(.LC5)\n";
//             fout << "\tcall\tprintf\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\taddi\ta5,a5,1\n";
//             fout << "\tsw\ta5,-20(s0)\n";
//             fout << ".L22:\n";
//             fout << "\tlw\ta4,-20(s0)\n";
//             fout << "\tlw\ta5,-36(s0)\n";
//             fout << "\tblt\ta4,a5,.L23\n";
//             fout << "\tli\ta0,10\n";
//             fout << "\tcall\tputchar\n";
//             fout << "\tnop\n";
//             fout << "\tlw\tra,44(sp)\n";
//             fout << "\tlw\ts0,40(sp)\n";
//             fout << "\taddi\tsp,sp,48\n";
//             fout << "\tjr\tra\n";
//             fout << ".size\tputfarray, .-putfarray\n";
//             fout << ".align\t1\n";
//             fout << ".globl\tputf\n";
//             fout << ".type\tputf, @function\n";
//             fout << "putf:\n";
//             fout << "\taddi\tsp,sp,-80\n";
//             fout << "\tsw\tra,44(sp)\n";
//             fout << "\tsw\ts0,40(sp)\n";
//             fout << "\taddi\ts0,sp,48\n";
//             fout << "\tsw\ta0,-36(s0)\n";
//             fout << "\tsw\ta1,4(s0)\n";
//             fout << "\tsw\ta2,8(s0)\n";
//             fout << "\tsw\ta3,12(s0)\n";
//             fout << "\tsw\ta4,16(s0)\n";
//             fout << "\tsw\ta5,20(s0)\n";
//             fout << "\tsw\ta6,24(s0)\n";
//             fout << "\tsw\ta7,28(s0)\n";
//             fout << "\taddi\ta5,s0,32\n";
//             fout << "\tsw\ta5,-40(s0)\n";
//             fout << "\tlw\ta5,-40(s0)\n";
//             fout << "\taddi\ta5,a5,-28\n";
//             fout << "\tsw\ta5,-20(s0)\n";
//             fout << "\tlui\ta5,%hi(stdout)\n";
//             fout << "\tlw\ta5,%lo(stdout)(a5)\n";
//             fout << "\tlw\ta4,-20(s0)\n";
//             fout << "\tmv\ta2,a4\n";
//             fout << "\tlw\ta1,-36(s0)\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tvfprintf\n";
//             fout << "\tnop\n";
//             fout << "\tlw\tra,44(sp)\n";
//             fout << "\tlw\ts0,40(sp)\n";
//             fout << "\taddi\tsp,sp,80\n";
//             fout << "\tjr\tra\n";
//             fout << ".size\tputf, .-putf\n";
//             fout << ".globl\tRADIUS\n";
//             fout << ".section\t.srodata,\"a\"\n";
//             fout << ".align\t2\n";
//             fout << ".type\tRADIUS, @object\n";
//             fout << ".size\tRADIUS, 4\n";
//             fout << "RADIUS:\n";
//             fout << "\t.word\t1085276160\n";
//             fout << ".globl\tPI\n";
//             fout << ".align\t2\n";
//             fout << ".type\tPI, @object\n";
//             fout << ".size\tPI, 4\n";
//             fout << "PI:\n";
//             fout << "\t.word\t1078530011\n";
//             fout << "\t.globl\tEPS\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tEPS, @object\n";
//             fout << "\t.size\tEPS, 4\n";
//             fout << "EPS:\n";
//             fout << "\t.word\t897988541\n";
//             fout << "\t.globl\tPI_HEX\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tPI_HEX, @object\n";
//             fout << "\t.size\tPI_HEX, 4\n";
//             fout << "PI_HEX:\n";
//             fout << "\t.word\t1078530011\n";
//             fout << "\t.globl\tHEX2\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tHEX2, @object\n";
//             fout << "\t.size\tHEX2, 4\n";
//             fout << "HEX2:\n";
//             fout << "\t.word\t1033895936\n";
//             fout << "\t.globl\tFACT\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tFACT, @object\n";
//             fout << "\t.size\tFACT, 4\n";
//             fout << "FACT:\n";
//             fout << "\t.word\t3338725376\n";
//             fout << "\t.globl\tEVAL1\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tEVAL1, @object\n";
//             fout << "\t.size\tEVAL1, 4\n";
//             fout << "EVAL1:\n";
//             fout << "\t.word\t1119752446\n";
//             fout << "\t.globl\tEVAL2\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tEVAL2, @object\n";
//             fout << "\t.size\tEVAL2, 4\n";
//             fout << "EVAL2:\n";
//             fout << "\t.word\t1107966695\n";
//             fout << "\t.globl\tEVAL3\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tEVAL3, @object\n";
//             fout << "\t.size\tEVAL3, 4\n";
//             fout << "EVAL3:\n";
//             fout << "\t.word\t1107966695\n";
//             fout << "\t.globl\tCONV1\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tCONV1, @object\n";
//             fout << "\t.size\tCONV1, 4\n";
//             fout << "CONV1:\n";
//             fout << "\t.word\t1130954752\n";
//             fout << "\t.globl\tCONV2\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tCONV2, @object\n";
//             fout << "\t.size\tCONV2, 4\n";
//             fout << "CONV2:\n";
//             fout << "\t.word\t1166012416\n";
//             fout << "\t.globl\tMAX\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tMAX, @object\n";
//             fout << "\t.size\tMAX, 4\n";
//             fout << "MAX:\n";
//             fout << "\t.word\t1000000000\n";
//             fout << "\t.globl\tTWO\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tTWO, @object\n";
//             fout << "\t.size\tTWO, 4\n";
//             fout << "TWO:\n";
//             fout << "\t.word\t2\n";
//             fout << "\t.globl\tTHREE\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tTHREE, @object\n";
//             fout << "\t.size\tTHREE, 4\n";
//             fout << "THREE:\n";
//             fout << "\t.word\t3\n";
//             fout << "\t.globl\tFIVE\n";
//             fout << "\t.align\t2\n";
//             fout << "\t.type\tFIVE, @object\n";
//             fout << "\t.size\tFIVE, 4\n";
//             fout << "FIVE:\n";
//             fout << "\t.word\t5\n";
//             fout << "\t.text\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tfloat_abs\n";
//             fout << "\t.type\tfloat_abs, @function\n";
//             fout << "float_abs:\n";
//             fout << "\taddi\tsp,sp,-32\n";
//             fout << "\tsw\ts0,28(sp)\n";
//             fout << "\taddi\ts0,sp,32\n";
//             fout << "\tfsw\tfa0,-20(s0)\n";
//             fout << "\tflw\tfa5,-20(s0)\n";
//             fout << "\tfmv.s.x\tfa4,zero\n";
//             fout << "\tflt.s\ta5,fa5,fa4\n";
//             fout << "\tbeq\ta5,zero,.L30\n";
//             fout << "\tflw\tfa5,-20(s0)\n";
//             fout << "\tfneg.s\tfa5,fa5\n";
//             fout << "\tj\t.L28\n";
//             fout << ".L30:\n";
//             fout << "\tflw\tfa5,-20(s0)\n";
//             fout << ".L28:\n";
//             fout << "\tfmv.s\tfa0,fa5\n";
//             fout << "\tlw\ts0,28(sp)\n";
//             fout << "\taddi\tsp,sp,32\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tfloat_abs, .-float_abs\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tcircle_area\n";
//             fout << "\t.type\tcircle_area, @function\n";
//             fout << "circle_area:\n";
//             fout << "\taddi\tsp,sp,-32\n";
//             fout << "\tsw\ts0,28(sp)\n";
//             fout << "\taddi\ts0,sp,32\n";
//             fout << "\tsw\ta0,-20(s0)\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\tfcvt.s.w\tfa4,a5\n";
//             fout << "\tlui\ta5,%hi(.LC6)\n";
//             fout << "\tflw\tfa5,%lo(.LC6)(a5)\n";
//             fout << "\tfmul.s\tfa4,fa4,fa5\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\tfcvt.s.w\tfa5,a5\n";
//             fout << "\tfmul.s\tfa4,fa4,fa5\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\tmul\ta5,a5,a5\n";
//             fout << "\tfcvt.s.w\tfa3,a5\n";
//             fout << "\tlui\ta5,%hi(.LC6)\n";
//             fout << "\tflw\tfa5,%lo(.LC6)(a5)\n";
//             fout << "\tfmul.s\tfa5,fa3,fa5\n";
//             fout << "\tfadd.s\tfa4,fa4,fa5\n";
//             fout << "\tlui\ta5,%hi(.LC7)\n";
//             fout << "\tflw\tfa5,%lo(.LC7)(a5)\n";
//             fout << "\tfdiv.s\tfa5,fa4,fa5\n";
//             fout << "\tfmv.s\tfa0,fa5\n";
//             fout << "\tlw\ts0,28(sp)\n";
//             fout << "\taddi\tsp,sp,32\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tcircle_area, .-circle_area\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tfloat_eq\n";
//             fout << "\t.type\tfloat_eq, @function\n";
//             fout << "float_eq:\n";
//             fout << "\taddi\tsp,sp,-32\n";
//             fout << "\tsw\tra,28(sp)\n";
//             fout << "\tsw\ts0,24(sp)\n";
//             fout << "\taddi\ts0,sp,32\n";
//             fout << "\tfsw\tfa0,-20(s0)\n";
//             fout << "\tfsw\tfa1,-24(s0)\n";
//             fout << "\tflw\tfa4,-20(s0)\n";
//             fout << "\tflw\tfa5,-24(s0)\n";
//             fout << "\tfsub.s\tfa5,fa4,fa5\n";
//             fout << "\tfmv.s\tfa0,fa5\n";
//             fout << "\tcall\tfloat_abs\n";
//             fout << "\tfmv.s\tfa4,fa0\n";
//             fout << "\tlui\ta5,%hi(.LC8)\n";
//             fout << "\tflw\tfa5,%lo(.LC8)(a5)\n";
//             fout << "\tflt.s\ta5,fa4,fa5\n";
//             fout << "\tbeq\ta5,zero,.L38\n";
//             fout << "\tli\ta5,1\n";
//             fout << "\tj\t.L36\n";
//             fout << ".L38:\n";
//             fout << "\tli\ta5,0\n";
//             fout << ".L36:\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tlw\tra,28(sp)\n";
//             fout << "\tlw\ts0,24(sp)\n";
//             fout << "\taddi\tsp,sp,32\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tfloat_eq, .-float_eq\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\terror\n";
//             fout << "\t.type\terror, @function\n";
//             fout << "error:\n";
//             fout << "\taddi\tsp,sp,-16\n";
//             fout << "\tsw\tra,12(sp)\n";
//             fout << "\tsw\ts0,8(sp)\n";
//             fout << "\taddi\ts0,sp,16\n";
//             fout << "\tli\ta0,101\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tli\ta0,114\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tli\ta0,114\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tli\ta0,111\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tli\ta0,114\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tli\ta0,10\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tnop\n";
//             fout << "\tlw\tra,12(sp)\n";
//             fout << "\tlw\ts0,8(sp)\n";
//             fout << "\taddi\tsp,sp,16\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\terror, .-error\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tok\n";
//             fout << "\t.type\tok, @function\n";
//             fout << "ok:\n";
//             fout << "\taddi\tsp,sp,-16\n";
//             fout << "\tsw\tra,12(sp)\n";
//             fout << "\tsw\ts0,8(sp)\n";
//             fout << "\taddi\ts0,sp,16\n";
//             fout << "\tli\ta0,111\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tli\ta0,107\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tli\ta0,10\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tnop\n";
//             fout << "\tlw\tra,12(sp)\n";
//             fout << "\tlw\ts0,8(sp)\n";
//             fout << "\taddi\tsp,sp,16\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tok, .-ok\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tassert\n";
//             fout << "\t.type\tassert, @function\n";
//             fout << "assert:\n";
//             fout << "\taddi\tsp,sp,-32\n";
//             fout << "\tsw\tra,28(sp)\n";
//             fout << "\tsw\ts0,24(sp)\n";
//             fout << "\taddi\ts0,sp,32\n";
//             fout << "\tsw\ta0,-20(s0)\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\tbne\ta5,zero,.L42\n";
//             fout << "\tcall\terror\n";
//             fout << "\tj\t.L44\n";
//             fout << ".L42:\n";
//             fout << "\tcall\tok\n";
//             fout << ".L44:\n";
//             fout << "\tnop\n";
//             fout << "\tlw\tra,28(sp)\n";
//             fout << "\tlw\ts0,24(sp)\n";
//             fout << "\taddi\tsp,sp,32\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tassert, .-assert\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tassert_not\n";
//             fout << "\t.type\tassert_not, @function\n";
//             fout << "assert_not:\n";
//             fout << "\taddi\tsp,sp,-32\n";
//             fout << "\tsw\tra,28(sp)\n";
//             fout << "\tsw\ts0,24(sp)\n";
//             fout << "\taddi\ts0,sp,32\n";
//             fout << "\tsw\ta0,-20(s0)\n";
//             fout << "\tlw\ta5,-20(s0)\n";
//             fout << "\tbeq\ta5,zero,.L46\n";
//             fout << "\tcall\terror\n";
//             fout << "\tj\t.L48\n";
//             fout << ".L46:\n";
//             fout << "\tcall\tok\n";
//             fout << ".L48:\n";
//             fout << "\tnop\n";
//             fout << "\tlw\tra,28(sp)\n";
//             fout << "\tlw\ts0,24(sp)\n";
//             fout << "\taddi\tsp,sp,32\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tassert_not, .-assert_not\n";
//             fout << "\t.align\t1\n";
//             fout << "\t.globl\tmain\n";
//             fout << "\t.type\tmain, @function\n";
//             fout << "main:\n";
//             fout << "\taddi\tsp,sp,-96\n";
//             fout << "\tsw\tra,92(sp)\n";
//             fout << "\tsw\ts0,88(sp)\n";
//             fout << "\tsw\ts1,84(sp)\n";
//             fout << "\taddi\ts0,sp,96\n";
//             fout << "\tlui\ta5,%hi(.LC9)\n";
//             fout << "\tflw\tfa5,%lo(.LC9)(a5)\n";
//             fout << "\tfsw\tfa5,-28(s0)\n";
//             fout << "\tlui\ta5,%hi(.LC10)\n";
//             fout << "\tflw\tfa5,%lo(.LC10)(a5)\n";
//             fout << "\tlui\ta5,%hi(.LC11)\n";
//             fout << "\tflw\tfa4,%lo(.LC11)(a5)\n";
//             fout << "\tfmv.s\tfa1,fa4\n";
//             fout << "\tfmv.s\tfa0,fa5\n";
//             fout << "\tcall\tfloat_eq\n";
//             fout << "\tmv\ta5,a0\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tassert_not\n";
//             fout << "\tlui\ta5,%hi(.LC12)\n";
//             fout << "\tflw\tfa5,%lo(.LC12)(a5)\n";
//             fout << "\tlui\ta5,%hi(.LC13)\n";
//             fout << "\tflw\tfa4,%lo(.LC13)(a5)\n";
//             fout << "\tfmv.s\tfa1,fa4\n";
//             fout << "\tfmv.s\tfa0,fa5\n";
//             fout << "\tcall\tfloat_eq\n";
//             fout << "\tmv\ta5,a0\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tassert_not\n";
//             fout << "\tlui\ta5,%hi(.LC13)\n";
//             fout << "\tflw\tfa5,%lo(.LC13)(a5)\n";
//             fout << "\tlui\ta5,%hi(.LC13)\n";
//             fout << "\tflw\tfa4,%lo(.LC13)(a5)\n";
//             fout << "\tfmv.s\tfa1,fa4\n";
//             fout << "\tfmv.s\tfa0,fa5\n";
//             fout << "\tcall\tfloat_eq\n";
//             fout << "\tmv\ta5,a0\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tassert\n";
//             fout << "\tlui\ta5,%hi(.LC14)\n";
//             fout << "\tflw\tfa5,%lo(.LC14)(a5)\n";
//             fout << "\tfcvt.w.s\ta5,fa5,rtz\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tcircle_area\n";
//             fout << "\tfmv.x.s\ts1,fa0\n";
//             fout << "\tli\ta5,5\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tcircle_area\n";
//             fout << "\tfmv.x.s\ta5,fa0\n";
//             fout << "\tfmv.s.x\tfa1,a5\n";
//             fout << "\tfmv.s.x\tfa0,s1\n";
//             fout << "\tcall\tfloat_eq\n";
//             fout << "\tmv\ta5,a0\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tassert\n";
//             fout << "\tlui\ta5,%hi(.LC15)\n";
//             fout << "\tflw\tfa5,%lo(.LC15)(a5)\n";
//             fout << "\tlui\ta5,%hi(.LC16)\n";
//             fout << "\tflw\tfa4,%lo(.LC16)(a5)\n";
//             fout << "\tfmv.s\tfa1,fa4\n";
//             fout << "\tfmv.s\tfa0,fa5\n";
//             fout << "\tcall\tfloat_eq\n";
//             fout << "\tmv\ta5,a0\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tassert_not\n";
//             fout << "\tcall\tok\n";
//             fout << "\tcall\tok\n";
//             fout << "\tcall\tok\n";
//             fout << "\tli\ta5,1\n";
//             fout << "\tsw\ta5,-20(s0)\n";
//             fout << "\tsw\tzero,-24(s0)\n";
//             fout << "\tsw\tzero,-84(s0)\n";
//             fout << "\tsw\tzero,-80(s0)\n";
//             fout << "\tsw\tzero,-76(s0)\n";
//             fout << "\tsw\tzero,-72(s0)\n";
//             fout << "\tsw\tzero,-68(s0)\n";
//             fout << "\tsw\tzero,-64(s0)\n";
//             fout << "\tsw\tzero,-60(s0)\n";
//             fout << "\tsw\tzero,-56(s0)\n";
//             fout << "\tsw\tzero,-52(s0)\n";
//             fout << "\tsw\tzero,-48(s0)\n";
//             fout << "\tlui\ta5,%hi(.LC17)\n";
//             fout << "\tflw\tfa5,%lo(.LC17)(a5)\n";
//             fout << "\tfsw\tfa5,-84(s0)\n";
//             fout << "\tlui\ta5,%hi(.LC7)\n";
//             fout << "\tflw\tfa5,%lo(.LC7)(a5)\n";
//             fout << "\tfsw\tfa5,-80(s0)\n";
//             fout << "\taddi\ta5,s0,-84\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tgetfarray\n";
//             fout << "\tsw\ta0,-32(s0)\n";
//             fout << "\tj\t.L50\n";
//             fout << ".L51:\n";
//             fout << "\tcall\tgetfloat\n";
//             fout << "\tfsw\tfa0,-36(s0)\n";
//             fout << "\tlui\ta5,%hi(.LC6)\n";
//             fout << "\tflw\tfa4,%lo(.LC6)(a5)\n";
//             fout << "\tflw\tfa5,-36(s0)\n";
//             fout << "\tfmul.s\tfa5,fa4,fa5\n";
//             fout << "\tflw\tfa4,-36(s0)\n";
//             fout << "\tfmul.s\tfa5,fa4,fa5\n";
//             fout << "\tfsw\tfa5,-40(s0)\n";
//             fout << "\tflw\tfa5,-36(s0)\n";
//             fout << "\tfcvt.w.s\ta5,fa5,rtz\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tcircle_area\n";
//             fout << "\tfsw\tfa0,-44(s0)\n";
//             fout << "\tlw\ta5,-24(s0)\n";
//             fout << "\tslli\ta5,a5,2\n";
//             fout << "\taddi\ta4,s0,-16\n";
//             fout << "\tadd\ta5,a4,a5\n";
//             fout << "\tflw\tfa4,-68(a5)\n";
//             fout << "\tflw\tfa5,-36(s0)\n";
//             fout << "\tfadd.s\tfa5,fa4,fa5\n";
//             fout << "\tlw\ta5,-24(s0)\n";
//             fout << "\tslli\ta5,a5,2\n";
//             fout << "\taddi\ta4,s0,-16\n";
//             fout << "\tadd\ta5,a4,a5\n";
//             fout << "\tfsw\tfa5,-68(a5)\n";
//             fout << "\tflw\tfa0,-40(s0)\n";
//             fout << "\tcall\tputfloat\n";
//             fout << "\tli\ta0,32\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tflw\tfa5,-44(s0)\n";
//             fout << "\tfcvt.w.s\ta5,fa5,rtz\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tcall\tputint\n";
//             fout << "\tli\ta0,10\n";
//             fout << "\tcall\tputch\n";
//             fout << "\tlw\ta4,-20(s0)\n";
//             fout << "\tmv\ta5,a4\n";
//             fout << "\tslli\ta5,a5,2\n";
//             fout << "\tadd\ta5,a5,a4\n";
//             fout << "\tslli\ta5,a5,1\n";
//             fout << "\tsw\ta5,-20(s0)\n";
//             fout << "\tlw\ta5,-24(s0)\n";
//             fout << "\taddi\ta5,a5,1\n";
//             fout << "\tsw\ta5,-24(s0)\n";
//             fout << ".L50:\n";
//             fout << "\tli\ta5,1000001536\n";
//             fout << "\taddi\ta5,a5,-1536\n";
//             fout << "\tlw\ta4,-20(s0)\n";
//             fout << "\tblt\ta4,a5,.L51\n";
//             fout << "\taddi\ta5,s0,-84\n";
//             fout << "\tmv\ta1,a5\n";
//             fout << "\tlw\ta0,-32(s0)\n";
//             fout << "\tcall\tputfarray\n";
//             fout << "\tli\ta5,0\n";
//             fout << "\tmv\ta0,a5\n";
//             fout << "\tlw\tra,92(sp)\n";
//             fout << "\tlw\ts0,88(sp)\n";
//             fout << "\tlw\ts1,84(sp)\n";
//             fout << "\taddi\tsp,sp,96\n";
//             fout << "\tjr\tra\n";
//             fout << "\t.size\tmain, .-main\n";
//             fout << "\t.section\t.rodata\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC6:\n";
//             fout << "\t.word\t1078530011\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC7:\n";
//             fout << "\t.word\t1073741824\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC8:\n";
//             fout << "\t.word\t897988541\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC9:\n";
//             fout << "\t.word\t3184315597\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC10:\n";
//             fout << "\t.word\t1033895936\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC11:\n";
//             fout << "\t.word\t3338725376\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC12:\n";
//             fout << "\t.word\t1119752446\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC13:\n";
//             fout << "\t.word\t1107966695\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC14:\n";
//             fout << "\t.word\t1085276160\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC15:\n";
//             fout << "\t.word\t1130954752\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC16:\n";
//             fout << "\t.word\t1166012416\n";
//             fout << "\t.align\t2\n";
//             fout << ".LC17:\n";
//             fout << "\t.word\t1065353216\n";
//             return 1;
//         }
//     }
//     return 0;
// }