#ifndef RV_DEF_H
#define RV_DEF_H

#include <string>

namespace rv
{

    // rv interger registers
    enum class rvREG
    {
        /* Xn   ABI name   Description                          Saver */
        X0,  // zero       hardwired to 0, ignores writes       n/a
        X1,  // ra         return address for jumps             Caller
        X2,  // sp         stack pointer                        Callee
        X3,  // gp         global pointer                       n/a
        X4,  // tp         thread pointer                       n/a
        X5,  // t0         temporary register 0                 Caller
        X6,  // t1         temporary register 1                 Caller
        X7,  // t2         temporary register 2                 Caller 栈传参指针
        X8,  // s0/fp      saved register 0 / frame pointer     Callee
        X9,  // s1         saved register 1                     Callee
        X10, // a0         return value / function argument 0   Caller 参数1
        X11, // a1         return value / function argument 1   Caller
        X12, // a2         function argument 2                  Caller
        X13, // a3         function argument 3                  Caller
        X14, // a4         function argument 4                  Caller
        X15, // a5         function argument 5                  Caller
        X16, // a6         function argument 6                  Caller
        X17, // a7         function argument 7                  Caller
        X18, // s2         saved register 2                     Callee
        X19, // s3         saved register 3                     Callee
        X20, // s4         saved register 4                     Callee
        X21, // s5         saved register 5                     Callee
        X22, // s6         saved register 6                     Callee
        X23, // s7         saved register 7                     Callee
        X24, // s8         saved register 8                     Callee
        X25, // s9         saved register 9                     Callee
        X26, // s10        saved register 10                    Callee
        X27, // s11        saved register 11                    Callee
        X28, // t3         temporary register 3                 Caller 用于计算数组地址和全局变量取址
        X29, // t4         temporary register 4                 Caller op2
        X30, // t5         temporary register 5                 Caller op1/des
        X31, // t6         temporary register 6                 Caller 用于加载立即数
    };

    std::string toString(rvREG r); // implement this in ur own way

    // rv float registers
    enum class rvFREG
    {
        /* Fn    ABI name  Description                          Saver */
        F0,  // ft0        floating-point temporary 0           Caller
        F1,  // ft1        floating-point temporary 1           Caller
        F2,  // ft2        floating-point temporary 2           Caller
        F3,  // ft3        floating-point temporary 3           Caller
        F4,  // ft4        floating-point temporary 4           Caller // fop2
        F5,  // ft5        floating-point temporary 5           Caller // fop1/fdes
        F6,  // ft6        floating-point temporary 6           Caller // 临时变量
        F7,  // ft7        floating-point temporary 7           Caller
        F8,  // fs0        floating-point saved register 0      Callee
        F9,  // fs1        floating-point saved register 1      Callee
        F10, // fa0        floating-point argument 0 / return value 0 Caller
        F11, // fa1        floating-point argument 1 / return value 1 Caller
        F12, // fa2        floating-point argument 2            Caller
        F13, // fa3        floating-point argument 3            Caller
        F14, // fa4        floating-point argument 4            Caller
        F15, // fa5        floating-point argument 5            Caller
        F16, // fa6        floating-point argument 6            Caller
        F17, // fa7        floating-point argument 7            Caller
        F18, // fs2        floating-point saved register 2      Callee
        F19, // fs3        floating-point saved register 3      Callee
        F20, // fs4        floating-point saved register 4      Callee
        F21, // fs5        floating-point saved register 5      Callee
        F22, // fs6        floating-point saved register 6      Callee
        F23, // fs7        floating-point saved register 7      Callee
        F24, // fs8        floating-point saved register 8      Callee
        F25, // fs9        floating-point saved register 9      Callee
        F26, // fs10       floating-point saved register 10     Callee
        F27, // fs11       floating-point saved register 11     Callee
        F28, // ft8        floating-point temporary 8           Caller
        F29, // ft9        floating-point temporary 9           Caller
        F30, // ft10       floating-point temporary 10          Caller
        F31, // ft11       floating-point temporary 11          Caller
    };
    std::string toString(rvFREG r); // implement this in ur own way

    // rv32i instructions
    // add instruction u need here!
    enum class rvOPCODE
    {
        // RV32I Base Integer Instructions
        ADD,
        SUB,
        XOR,
        OR,
        AND,
        SLL,
        SRL,
        SRA,
        SLT,
        SLTU, // arithmetic & logic
        ADDI,
        XORI,
        ORI,
        ANDI,
        SLLI,
        SRLI,
        SRAI,
        SLTI,
        SLTIU, // immediate
        LW,
        SW, // load & store
        BEQ,
        BNE,
        BLT,
        BGE,
        BLTU,
        BGEU, // conditional branch
        JAL,
        JALR, // jump

        // RV32M Multiply Extension

        // RV32F / D Floating-Point Extensions

        // Pseudo Instructions
        LA,
        LI,
        MOV,
        J, // ...
    };
    std::string toString(rvOPCODE r); // implement this in ur own way

} // namespace rv

#endif