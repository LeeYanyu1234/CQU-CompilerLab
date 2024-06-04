/**
 * @file lexical.cpp
 * @brief 词法分析，包括预处理器和词法分析器。前者处理注释，后者转换为token。
 * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
 * @version 1.0.1
 * @date 2024-05-14
 *
 * @copyright Copyright (c) 2024  重庆大学
 *
 */
#include "front/lexical.h"

#include <map>
#include <cassert>
#include <string>

#define TODO assert(0 && "todo")

// #define DEBUG_DFA
// #define DEBUG_SCANNER

std::string frontend::toString(State s)
{
    switch (s)
    {
    case State::Empty:
        return "Empty";
    case State::Ident:
        return "Ident";
    case State::IntLiteral:
        return "IntLiteral";
    case State::FloatLiteral:
        return "FloatLiteral";
    case State::op:
        return "op";
    default:
        assert(0 && "invalid State");
    }
    return "";
}

std::set<std::string> frontend::keywords = {"const", "int", "float", "if", "else", "while", "continue", "break", "return", "void"};

/**
 * @brief 预处理器，去除注释，单行和多行
 * @param  fin
 * @return std::string
 * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
 * @date 2024-05-14
 */
std::string preproccess(std::ifstream &fin)
{
    // TODO; lab1todo2
    std::string res;
    std::string curLine;     // 当前处理行
    bool isInMulCom = false; // 是否在多行注释内部
    while (std::getline(fin, curLine))
    {
        int mulEndPos = 0; // 记录多行注释结束符位置
        if (isInMulCom)    // 如果在多行注释内部
        {
            mulEndPos = curLine.find("*/");     // 寻找该行是否存在多行注释结束符
            if (mulEndPos != std::string::npos) // 如果存在
            {                                   // 删除结束符之前包括结束符的内容
                isInMulCom = false;
                curLine.erase(0, mulEndPos + 2);
            }
            else
                continue; // 如果不存在，忽视当前行
        }

        while (curLine.find("//") != std::string::npos || curLine.find("/*") != std::string::npos)
        { // 如果这一行还存在单行注释或者多行注释起始符
            int sinPos = curLine.find("//");
            int mulStartPos = curLine.find("/*");
            if (sinPos != std::string::npos && mulStartPos == std::string::npos)      // 如果只存在单行注释
                curLine.erase(sinPos);                                                // 删除单行注释后的所有内容
            else if (sinPos == std::string::npos && mulStartPos != std::string::npos) // 如果只存在多行注释起始符
            {
                //? 最后发现问题出在这里，如果我们的多行注释是连续的，会出现/*/也被识别为多行注释的情况
                //? 所以这里需要从多行注释起始符结束的地方开始查询多行注释结束符
                //? 所以要从mulStartPos+2的位置开始查找起始符
                //? debug的时候误打误撞改了不对的逻辑，但是获得了正确的结果 (-_-||)
                mulEndPos = curLine.find("*/", mulStartPos + 2);
                if (mulEndPos != std::string::npos) // 同时本行存在多行注释终止符
                    //? 这里感觉有一点问题，对应样例45，/*/为什么会被识别为多行注释呢？我这里的写法本身有一点问题
                    //? 我是在整行查找多行注释的结束符，而这里按理来说应该在多行注释起始符之后才开始查找
                    //? 但是样例45要求需要把/*/识别为多行注释，十分神奇
                    curLine.erase(mulStartPos, mulEndPos - mulStartPos + 2); // 删除多行注释
                else                                                         // 本行不存在多行注释终止符
                {
                    isInMulCom = true;          // 标记多行注释状态
                    curLine.erase(mulStartPos); // 删除多行注释起始符
                }
            }
            else // 如果多行注释和单行注释同时存在
            {
                if (sinPos < mulStartPos) // 单行注释在前
                    curLine.erase(sinPos);
                else
                {
                    mulEndPos = curLine.find("*/");
                    if (mulEndPos != std::string::npos)                          // 同时本行存在多行注释终止符
                        curLine.erase(mulStartPos, mulEndPos - mulStartPos + 2); // 删除多行注释
                    else                                                         // 本行不存在多行注释终止符
                    {
                        isInMulCom = true;          // 标记多行注释状态
                        curLine.erase(mulStartPos); // 删除多行注释起始符
                    }
                }
            }
        }
        res += curLine + "\n";
    }
    return res;
}

/**
 * @brief 判断字符是不是操作符
 * @param c
 * @return bool
 * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
 * @date 2024-05-14
 */
bool isoperator(char c)
{
    // TODO; lab1todo4
    return c == '+' || c == '-' || c == '*' || c == '/' ||
           c == '%' || c == '<' || c == '>' || c == '=' ||
           c == ':' || c == ';' || c == '(' || c == ')' ||
           c == '[' || c == ']' || c == '{' || c == '}' ||
           c == '!' || c == '&' || c == '|' || c == ',';
}

frontend::TokenType getIdentType(std::string s) // 返回关键字类型
{
    // TODO; lab1todo5
    if (s == "const") // 先与10个关键字比较
        return frontend::TokenType::CONSTTK;
    else if (s == "int")
        return frontend::TokenType::INTTK;
    else if (s == "float")
        return frontend::TokenType::FLOATTK;
    else if (s == "if")
        return frontend::TokenType::IFTK;
    else if (s == "else")
        return frontend::TokenType::ELSETK;
    else if (s == "while")
        return frontend::TokenType::WHILETK;
    else if (s == "continue")
        return frontend::TokenType::CONTINUETK;
    else if (s == "break")
        return frontend::TokenType::BREAKTK;
    else if (s == "return")
        return frontend::TokenType::RETURNTK;
    else if (s == "void")
        return frontend::TokenType::VOIDTK;
    else // 如果都不是，说明是名称
        return frontend::TokenType::IDENFR;
}

/**
 * @brief 返回操作符类型
 * @param s
 * @return frontend::TokenType
 * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
 * @date 2024-05-14
 */
frontend::TokenType getOpType(std::string s)
{
    // TODO; lab1todo6
    if (s == "+")
        return frontend::TokenType::PLUS;
    else if (s == "-")
        return frontend::TokenType::MINU;
    else if (s == "*")
        return frontend::TokenType::MULT;
    else if (s == "/")
        return frontend::TokenType::DIV;
    else if (s == "%")
        return frontend::TokenType::MOD;
    else if (s == "<")
        return frontend::TokenType::LSS;
    else if (s == ">")
        return frontend::TokenType::GTR;
    else if (s == ":")
        return frontend::TokenType::COLON;
    else if (s == "=")
        return frontend::TokenType::ASSIGN;
    else if (s == ";")
        return frontend::TokenType::SEMICN;
    else if (s == ",")
        return frontend::TokenType::COMMA;
    else if (s == "(")
        return frontend::TokenType::LPARENT;
    else if (s == ")")
        return frontend::TokenType::RPARENT;
    else if (s == "[")
        return frontend::TokenType::LBRACK;
    else if (s == "]")
        return frontend::TokenType::RBRACK;
    else if (s == "{")
        return frontend::TokenType::LBRACE;
    else if (s == "}")
        return frontend::TokenType::RBRACE;
    else if (s == "!")
        return frontend::TokenType::NOT;
    else if (s == "<=")
        return frontend::TokenType::LEQ;
    else if (s == ">=")
        return frontend::TokenType::GEQ;
    else if (s == "==")
        return frontend::TokenType::EQL;
    else if (s == "!=")
        return frontend::TokenType::NEQ;
    else if (s == "&&")
        return frontend::TokenType::AND;
    else if (s == "||")
        return frontend::TokenType::OR;
    else
        assert(0 && "invalid operator!");
}

/**
 * @brief 判断是不是两字符宽的操作符
 * @param s
 * @return bool
 * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
 * @date 2024-05-14
 */
bool isDoubleWidthOperator(std::string s)
{
    // TODO; lab17
    return s == ">=" || s == "<=" || s == "==" ||
           s == "!=" || s == "&&" || s == "||";
}

frontend::DFA::DFA() : cur_state(frontend::State::Empty), cur_str() {}

frontend::DFA::~DFA() {}

/**
 * @brief 状态机，读入下一个字符
 * @param input
 * @param buf
 * @return bool
 * @author LiHaoxuan (leehaoxuan.1234@gmail.com)
 * @date 2024-05-14
 */
bool frontend::DFA::next(char input, Token &buf)
{
#ifdef DEBUG_DFA
#include <iostream>
    std::cout << "in state [" << toString(cur_state) << "], input = \'" << input << "\', str = " << cur_str << std::endl;
#endif
    // TODO; lab1todo3
    bool flag = false;
    switch (cur_state)
    {
    case frontend::State::Empty:
        if (isspace(input))                      // 读入一个空白字符
            reset();                             // 重置状态
        else if (isalpha(input) || input == '_') // 读入一个字母或下划线
        {                                        // 说明可能是标识符
            cur_state = frontend::State::Ident;  // 转换到Ident状态
            cur_str += input;                    // 并把这个字符存入缓冲区
        }
        else if (isdigit(input)) // 读入一个数字
        {
            cur_state = frontend::State::IntLiteral; // 转换到IntLiteral状态
            cur_str += input;
        }
        //? 在实验2中出现了这样的情况，可能会出现以小数点开头的小数
        else if (input == '.') // 读入一个小数点
        {
            cur_state = frontend::State::FloatLiteral; // 转换到FloatLiteral状态
            cur_str += input;
        }
        else if (isoperator(input)) // 读入一个操作符
        {
            cur_state = frontend::State::op; // 转换到op状态
            cur_str += input;
        }
        else // 读入其他字符都是非法输入
            assert(0 && "invalid input");
        break;
    case frontend::State::Ident:
        if (isspace(input))
        {
            buf.type = getIdentType(cur_str);
            buf.value = cur_str;
            reset();
            flag = true;
        }
        else if (isalpha(input) || isdigit(input) || input == '_')
        {
            cur_state = frontend::State::Ident;
            cur_str += input;
        }
        else if (isoperator(input))
        {
            buf.type = getIdentType(cur_str);
            buf.value = cur_str;
            cur_str = "";
            cur_state = frontend::State::op;
            cur_str += input;
            flag = true;
        }
        else
            assert(0 && "invalid input");
        break;
    case frontend::State::IntLiteral:
        if (isspace(input))
        {
            buf.type = frontend::TokenType::INTLTR;
            buf.value = cur_str;
            reset();
            flag = true;
        }
        else if (isdigit(input) || isalpha(input)) // 只有十进制输入，样例21出现0xb
        {
            cur_state = frontend::State::IntLiteral;
            cur_str += input;
        }
        else if (input == '.') // 出现小数点，说明是小数
        {
            cur_state = frontend::State::FloatLiteral;
            cur_str += input;
        }
        else if (isoperator(input))
        {
            buf.type = frontend::TokenType::INTLTR;
            buf.value = cur_str;
            cur_str = "";
            cur_state = frontend::State::op;
            cur_str += input;
            flag = true;
        }
        else
            assert(0 && "invalid input");
        break;
    case frontend::State::FloatLiteral:
        if (isspace(input))
        {
            buf.type = frontend::TokenType::FLOATLTR;
            buf.value = cur_str;
            reset();
            flag = true;
        }
        else if (isdigit(input))
        {
            cur_state = frontend::State::FloatLiteral;
            cur_str += input;
        }
        else if (isoperator(input))
        {
            buf.type = frontend::TokenType::FLOATLTR;
            buf.value = cur_str;
            cur_str = "";
            cur_state = frontend::State::op;
            cur_str += input;
            flag = true;
        }
        else
            assert(0 && "invalid input");
        break;
    case frontend::State::op:
        if (isspace(input))
        {
            buf.type = getOpType(cur_str);
            buf.value = cur_str;
            reset();
            flag = true;
        }
        else if (isalpha(input) || input == '_')
        {
            buf.type = getOpType(cur_str);
            buf.value = cur_str;
            cur_str = "";
            cur_state = frontend::State::Ident;
            cur_str += input;
            flag = true;
        }
        else if (isdigit(input))
        {
            buf.type = getOpType(cur_str);
            buf.value = cur_str;
            cur_str = "";
            cur_state = frontend::State::IntLiteral;
            cur_str += input;
            flag = true;
        }
        else if (isoperator(input))
        {
            if (isDoubleWidthOperator(cur_str + input))
                cur_str += input;
            else
            {
                buf.type = getOpType(cur_str);
                buf.value = cur_str;
                cur_str = "";
                cur_state = frontend::State::op;
                cur_str += input;
                flag = true;
            }
        }
        else if (input == '.')
        {
            buf.type = getOpType(cur_str);
            buf.value = cur_str;
            cur_str = "";
            cur_state = frontend::State::FloatLiteral;
            cur_str += input;
            flag = true;
        }
        else
            assert(0 && "invalid input");
        break;
    default:
        assert(0 && "invalid state");
        break;
    }

    // #ifdef DEBUG_DFA
    // std::cout << "next state is [" << toString(cur_state) << "], next str = " << cur_str << "\t, ret = " << ret << std::endl;
    // #endif
    return flag;
}

void frontend::DFA::reset()
{
    cur_state = State::Empty;
    cur_str = "";
}

frontend::Scanner::Scanner(std::string filename) : fin(filename)
{
    if (!fin.is_open())
    {
        assert(0 && "in Scanner constructor, input file cannot open");
    }
}

frontend::Scanner::~Scanner()
{
    fin.close();
}

std::vector<frontend::Token> frontend::Scanner::run()
{
    // TODO; lab1todo1
    std::vector<Token> tokens;
    Token tk;
    DFA dfa;
    std::string str = preproccess(fin); // fin是scanner读入的文件
    str += "\n";
    for (char c : str)
    {
        if (dfa.next(c, tk))
        {
            tokens.push_back(tk);
#ifdef DEBUG_SCANNER
#include <iostream>
            std::cout
                << "token: " << toString(tk.type) << "\t" << tk.value << std::endl;
#endif
        }
    }
    return tokens;
}