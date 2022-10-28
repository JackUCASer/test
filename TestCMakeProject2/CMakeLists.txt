# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.0.0)

# 项目信息
project (Demo1)

# 指定生成目标 将名为 main.cpp 的源文件编译成一个名称为 ${PROJECT_NAME} 的可执行文件,即exe文件。
add_executable(${PROJECT_NAME} main.c) 