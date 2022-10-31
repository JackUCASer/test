# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.0.0)

# 项目信息
project (Demo3)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 添加子目录
add_subdirectory(math)      # math子目录
add_subdirectory(filter)    # filter子目录
add_subdirectory(fft)       # fft子目录

# 指定生成目标 将名为 main.c 的源文件编译成一个名称为 ${PROJECT_NAME} 的可执行文件,即exe文件。
add_executable(${PROJECT_NAME} main.c) 

# 添加链接库:使用命令 target_link_libraries 指明可执行文件 ${PROJECT_NAME} 需要连接一个名为 MathFunctions 的链接库 
target_link_libraries(${PROJECT_NAME} MathFunctions DSP_FILTERS DSP_FFT)
