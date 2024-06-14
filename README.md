<h1><center>编译原理课程实验</center></h1>

## 一、实验目的

以系统能力提升为目标，通过实验逐步构建一个将类C语言翻译至汇编的编译器，最终生成的汇编代码通过GCC的汇编器转化为二进制可执行文件，并在物理机或模拟器上运行。帮助掌握编译原理的各个环节：词法分析、语法分析、语义分析、代码生成，以及实现所需的数据结构：语法树、符号表等。通过这样的实验，提高学生协作编程的能力，加深对编译技术的理解，编译原理是一门综合各个学科知识的课程，编译系统设计让学生在实践中综合理解计算机学科知识。



## 二、实验方案

本次实验将实现一个由SysY(精简版C语言，来自[CSC / CSC-Compiler / Compiler2022 · GitLab (eduxiji.net)](https://gitlab.eduxiji.net/csc1/nscscc/compiler2022/-/tree/master))翻译至RISC-V汇编的编译器，生成的汇编通过GCC的汇编器翻译至二进制，最终运行在模拟器qemu-riscv上实验至少包含四个部分:词法和语法分析、语义分析和中间代码生成、以及目标代码生成，每个部分都依赖前一个部分的结果，逐步构建一个完整编译器。

1. 实验一：词法分析和语法分析，将读取源文件中代码并进行分析，输出一颗语法树。
2. 实验二：接受一颗语法树，进行语义分析、中间代码生成，输出中间表示IR(Intermediate Representation)
3. 实验三：根据IR翻译成为汇编
4. 实验四(可选)：IR和汇编层面的优化



## 三、实验环境搭建

实验开始的第一步便是搭建实验环境，一个好的实验环境可以让你事半功倍，但是可惜的是实验指导书提供的内容大概率是远远不够的，所以还需要大家自行摸索。这里我仅展示我配置环境的过程，对于一些细节和更多功能还需要同学们STFW(Search The Fucking Web)。

### 3.1 下载实验框架

[重庆大学在希冀平台上的项目](https://course.educg.net/indexcs/simple.jsp?loginErr=0)一共提供了三个版本的实验框架，这里建议直接下载实验3或者实验4的代码框架，实验1和实验2的框架都只包含自己实验需要的部分，不包括后面的内容。这样可以一直延续使用，不用中途更换代码。

>  [!warning]
>
> 实验2的代码框架存在问题，如果是通过docker配置的运行环境，那么会出现Cmake版本不匹配的问题，这里需要修改CMakeLists.txt文件的第一行代码为：`cmake_minimum_required(VERSION 3.10)`。docker环境中和其他代码框架提供的版本都是3.10版本，而实验2代码框架是3.22版本。



### 3.2 准备Linux环境

这里不建议配置Windows实验环境，因为后面的qemu只能在Linux环境下运行。有两种方式可以配置Linux环境，一种是通过VMware虚拟机，另一种是通过WSL(Windows Subsystem for Linux)。这里建议选择后者，原因有两点，第一个原因是性能更好，在代码量比较大的情况下，可能打开Vscode阅读代码都会出现卡顿，使用WSL可以显著提升性能，因为它是直接运行在硬件上。第二个原因是可以更方便地通过远程连接Vscode，这样就可以更加方便地配置调试环境。

具体的配置方法这里不做具体讲解，因为你大概率会遇到自己的问题，所以这里仅仅给出一些步骤，具体的操作还是请STFW。

1. 开启Windows系统的Linux子系统功能和虚拟机平台功能。
2. 安装WSL，可以升级到WSL2，据说性能更好。
3. 在WSL中安装Ubuntu22.04系统。



### 3.3 准备docker环境

完成3.2后你已经获得了自己的Linux系统，下面需要连接到Vscode。首先在Vscode中安装WSL插件，然后点击右下角的那个双箭头就可以连接到WSL。连接好后你就可以通过Vscode的终端直接操作Linux系统，而且之后也可以直接通过Vscode打开WSL，而不再需要通过Windows的命令行或者终端。

为了获得实验环境还需要下载docker，docker的下载安装方法也请STFW。这里也可能会遇到很多问题，自己摸索吧，不遇到问题怎么知道自己在配置环境呢。

安装好docker后就需要拉取实验环境的镜像：

```bash
docker pull frankd35/demo:v3
```

哦，对了，第一步其实是启动docker：

```bash
sudo service docker start
```

拉取镜像的意思其实和下载Windows或者Linux操作系统的安装包差不多，只不过这个安装包里面已经帮你配好了适配于我们实验框架的环境，通过挂载就可以使用。

然后就是挂载目录：

```bash
docker run -it -v {你的代码框架目录}:/coursegrader frankd35/demo:v3
```

挂载目录其实也有前置步骤，你首先需要把已经下载好的代码框架复制到Linux系统中，如果你使用的是WSL，那么可以通过`cp`指令复制压缩包。至于为什么可以直接通过`cp`指令复制，这是因为WSL会自动将我们的硬盘挂载到`/mnt`目录下，这样就可以直接访问Windows目录下的文件。然后在自己需要的目录下解压代码框架就可以了，解压的目录就对应上面那条指令里面的“你的代码框架目录”。

完成这一步后就创建了一个docker容器，如果你在Vscode安装了docker的插件那么在你的右边菜单栏里面应该可以看到一个鲸鱼标志，点击标志可以通过Vscode提供的图形化界面来使用docker。如果点击后显示×，那么说明你的docker没有启动。

进入过后应该有三栏，第一栏是CONTAINERS，也就是容器，就是我们前面挂载目录所创建的东西。第二栏是IMAGES，也就是镜像，是我们拉取的实验环境。第三栏没用到，不用管。刚进入docker后如果容器显示的是一个橙红色的方框，说明容器没有启动，可以右键这个容器选择Start，然后记得刷新。启动后容器会变成绿色的箭头，然后再次右键，这里建议选择附加Vscode，这样才可以继续配置gdb并连接到Vscode。一个镜像可以通过挂载不同的目录创建多个不同的容器，这样就可以同时运行多个代码。

> [!tip]
>
> 以下是给粥粥人MuMu模拟器用户的一些建议。在你开启WSL之后有没有发现每次进入模拟器开始在98%卡住并且玩舟的时候也开始卡顿了？或者说如果你通过模拟器关闭Hyper-V后WSL用不了了？
>
> 不用担心，这些都是正常现象，都是Windows不够努力导致的。Windows提供的Hyper-V会和模拟器的虚拟机冲突，正所谓鱼和熊掌不可兼得，如果你希望模拟器不卡顿，那就点击你的模拟器的菜单栏，打开问题诊断进行优化，模拟器会自动为你关闭Hyper-V。哈哈，然后你就会发现你的WSL用不了了。
>
> 不过也别急，如果你想要重新打开WSL也很简单，第一步以管理员身份运行终端，执行以下命令：`bcdedit /set hypervisorlaunchtype auto`；第二步就是通过Windows->搜索“启用或关闭Windows功能”->开启Hyper-V。如果没有这一项，还需要通过批处理加入这一项。批处理的指令就自己上网搜索吧。
>
> 你问我为什么一个tips写的和正文差不多，那当然是因为粥粥人心疼粥粥人😋。



### 3.4 编译项目



## 常用指令

### 启动docker

```bash
sudo service docker start
docker ps -a
docker start 6ed7c6e378fd
```



### 拉取镜像

```bash
docker pull frankd35/demo:v3
```



### 挂载容器

```bash
docker run -it -v ~/compilerLab:/coursegrader frankd35/demo:v3
```



### 修改过源代码，需要重新编译执行

```bash
cd /coursegrader
rm -rf build/
mkdir build
cd ./build
cmake ..
make
cd /coursegrader
./bin/compiler ./test/testcase/basic/00_main.sy S -o ./myTest/test.out
```



### 使用python脚本测试（词法分析测试）

```bash
cd /coursegrader/test
python3 build.py
python3 run.py s0
python3 test.py s0
```



### 使用python脚本测试（语法分析测试）

```bash
cd /coursegrader/test
python3 build.py
python3 run.py s1
python3 test.py s1
```



### 使用python脚本测试（语义分析测试）

```bash
cd /coursegrader/test
python3 build.py
python3 run.py s2
python3 test.py s2
```



### 使用python脚本测试（目标代码生成测试）

```c++
cd /coursegrader/test
python3 build.py
python3 run.py S
python3 test.py S
```



## 实验1



## 实验2



## 实验3



### 单个测试点测试

```bash
cd /coursegrader
rm -rf build/
mkdir build
cd ./build
cmake ..
make
cd /coursegrader
./bin/compiler ./test/testcase/function/70_dijkstra.sy -S -o ./myTest/test.s
cd /coursegrader/myTest
riscv32-unknown-linux-gnu-gcc test.s sylib-riscv-linux.a -o test.out
qemu-riscv32.sh test.out
```



```bash
cd /coursegrader/myTest
riscv32-unknown-linux-gnu-gcc -S testref.c -o testref.s
riscv32-unknown-linux-gnu-gcc testref.s sylib-riscv-linux.a -o testref.out
qemu-riscv32.sh testref.out
cd /coursegrader/test

```

```
10
0x1.999999999999ap-4 0x1.999999999999ap-3 0x1.3333333333333p-2 0x1.999999999999ap-2 0x1.0000000000000p-1
0x1.3333333333333p-1 0x1.6666666666666p-1 0x1.999999999999ap-1 0x1.ccccccccccccdp-1 0x1.0000000000000p+0
0x1.199999999999ap+0
0x1.199999999999ap+1
0x1.a666666666666p+1
0x1.199999999999ap+2
0x1.6000000000000p+2
0x1.a666666666666p+2
0x1.ecccccccccccdp+2
0x1.199999999999ap+3
0x1.3cccccccccccdp+3
0x1.4333333333333p+3


arr_Scp1 起始地址44(sp)
input_Scp2 地址152(sp)
t44 168(sp)



cd /coursegrader/myTest
riscv32-unknown-linux-gnu-gcc test.s sylib-riscv-linux.a -o test.out
qemu-riscv32.sh test.out
```
