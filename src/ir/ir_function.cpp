#include "ir/ir_function.h"

#include <utility>
#include <vector>
#include <string>

ir::Function::Function() : name("null"), returnType(Type::null), ParameterList(std::vector<ir::Operand>()), InstVec(std::vector<ir::Instruction *>()) {}

ir::Function::Function(const std::string &n, const ir::Type &type) : name(n), returnType(type), ParameterList(std::vector<ir::Operand>()), InstVec(std::vector<ir::Instruction *>()) {}

ir::Function::Function(const std::string &n, const std::vector<Operand> &pl, const ir::Type &type) : name(n), returnType(type), ParameterList(pl), InstVec(std::vector<ir::Instruction *>()) {}

/**
 * @brief 向ir函数中添加ir指令
 * @param inst
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
void ir::Function::addInst(ir::Instruction *inst)
{
    InstVec.push_back(inst);
}

/**
 * @brief 打印ir程序中的函数
 * @return std::string
 * @author LeeYanyu1234 (343820386@qq.com)
 * @date 2024-05-31
 */
std::string ir::Function::draw()
{
    std::string res = toString(returnType) + " " + name + "(";
    for (const auto &it : ParameterList)
        res += toString(it.type) + " " + it.name + ",";
    if (ParameterList.size())
        res = res.substr(0, res.size() - 1);
    res += ")\n";
    for (size_t i = 0; i < InstVec.size(); i++)
        res += "\t" + std::to_string(i) + ": " + InstVec[i]->draw() + "\n";
    res += "end\n\n";
    return res;
}