#ifndef IROP_H
#define IROP_H

#include <string>

namespace ir
{
    //? 这里的操作符之间的顺序是不能改变的，一旦改变，本地不会出错，但是在线平台会出错
    //? 究其原因在于枚举变量实际上是绑定了整数，每一个枚举变量都有一个固定的整数与之对应
    //? 在ir评测机中，用到了这里的枚举变量，本地的ir评测机是每一次编译时生成的，所以会重新生成整数
    //? 但是在线的评测机已经生成好了，所以本地的顺序如果和在线不一致的话会出错
    enum class Operator
    {
        //* 调用返回ir
        _return, // return   op1
        _goto,   // goto     [op1=cond_var/null],    des = offset
        call,    // call     op1 = func_name,    des = retval  /* func.name = function, func.type = return type*/

        //* 访存于指针运算ir
        // alloc [arr_size]*4 byte space on stack for array named [arr_name], do not use this for global arrays
        alloc,  // alloc    op1 = arr_size,     des = arr_name
        store,  // store    des,    op1,    op2    op2为下标 -> 偏移量  op1为 store 的数组名, des 为被存储的变量
        load,   // load     des,    op1,    op2    op2为下标 -> 偏移量  op1为 load 的数组名, des 为被赋值变量
        getptr, // op1: arr_name, op2: arr_off

        //* 变量定义ir
        //* 此处的变量是指在程序中定义的变量，在后面的程序描述中变量包括程序中定义的变量和为生成ir而产生的临时变量
        //* 在ir测评机中，认为一个出现在des位置且未分配空间的变量即为一个新的变量，会自动为其分配空间
        def,
        fdef,

        //* 变量赋值ir
        mov,
        fmov,

        //* 类型转换ir
        cvt_i2f, // convert [Int]op1 to [Float]des
        cvt_f2i, // convert [Float]op1 to [Int]des

        //* 算术运算ir
        add,
        addi,
        fadd,
        sub,
        subi,
        fsub,
        mul,
        fmul,
        div,
        fdiv,
        mod,

        //* 逻辑运算ir
        //* 逻辑运算的结果是1/0，整型与浮点型之间不能直接做逻辑运算
        lss,
        flss,
        leq,
        fleq,
        gtr,
        fgtr,
        geq,
        fgeq,
        eq,
        feq,
        neq,
        fneq,
        _not,
        _and,
        _or,

        //* 空ir
        __unuse__,
    };

    std::string toString(Operator t);

}
#endif
