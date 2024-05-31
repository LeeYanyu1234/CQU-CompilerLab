/**
 * @file Token.h
 * @author Yuntao Dai (d1581209858@live.com)
 * @brief
 * definition of Token
 * a Token should has its type and its value(the string)
 * @version 0.1
 * @date 2022-12-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace frontend
{

    // Token类型枚举变量
    enum class TokenType
    {
        IDENFR,     // identifier，变量名
        INTLTR,     // int literal，整数字面量
        FLOATLTR,   // float literal，浮点数字面量
        CONSTTK,    // const
        VOIDTK,     // void
        INTTK,      // int
        FLOATTK,    // float
        IFTK,       // if
        ELSETK,     // else
        WHILETK,    // while
        CONTINUETK, // continue
        BREAKTK,    // break
        RETURNTK,   // return
        PLUS,       // +
        MINU,       // -
        MULT,       // *
        DIV,        // /
        MOD,        // %
        LSS,        // <
        GTR,        // >
        COLON,      // :
        ASSIGN,     // =
        SEMICN,     // ;
        COMMA,      // ,
        LPARENT,    // (
        RPARENT,    // )
        LBRACK,     // [
        RBRACK,     // ]
        LBRACE,     // {
        RBRACE,     // }
        NOT,        // !
        LEQ,        // <=
        GEQ,        // >=
        EQL,        // ==
        NEQ,        // !=
        AND,        // &&
        OR,         // ||
    };
    std::string toString(TokenType);

    /**
     * @brief 词法单元
     * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
     * @date 2024-05-17
     */
    struct Token
    {
        TokenType type;    // 词法单元的类型
        std::string value; // 词法单元的值
    };

} // namespace frontend

#endif