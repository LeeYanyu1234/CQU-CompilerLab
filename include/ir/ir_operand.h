#ifndef IROPERAND_H
#define IROPERAND_H

#include <string>

namespace ir
{

    enum class Type
    {
        Int,
        Float,
        IntLiteral,
        FloatLiteral,
        IntPtr,
        FloatPtr,
        null
    };

    std::string toString(Type t);

    /**
     * @brief ir程序的操作数结构
     * @note 变量=变量名+类型，常量=值+类型
     * @note ("a", Int), ("1.0", FloatLiteral)
     * @author LeeYanyu1234 (343820386@qq.com)
     * @date 2024-05-31
     */
    struct Operand
    {
        std::string name;
        Type type;
        Operand(std::string = "null", Type = Type::null);
    };

}
#endif
