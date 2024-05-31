#include "ir/ir_program.h"

#include <vector>
#include <string>
#include <iostream>

ir::Program::Program() : functions(std::vector<ir::Function>()) {}

ir::GlobalVal::GlobalVal(ir::Operand va) { this->val = va; }

ir::GlobalVal::GlobalVal(ir::Operand va, int len)
{
    this->val = va;
    this->maxlen = len;
}

/**
 * @brief 向ir程序中添加函数
 * @param proc
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void ir::Program::addFunction(const ir::Function &proc)
{
    functions.push_back(proc);
}

/**
 * @brief 打印ir程序
 * @return std::string
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
std::string ir::Program::draw()
{
    std::string ret;
    for (auto i : functions) // 打印函数
    {
        ret += i.draw();
    }
    ret += "GVT:\n"; // 打印全局变量
    for (auto i : this->globalVal)
    {
        ret += "\t" + i.val.name + " " + toString(i.val.type) + " " + std::to_string(i.maxlen) + "\n";
    }
    return ret;
}