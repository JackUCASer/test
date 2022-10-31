# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_FFT_SRCS 变量
aux_source_directory(. DIR_FFT_SRCS)

# 生成链接库:使用命令 add_library 将 src 目录中的源文件编译为静态链接库
add_library (DSP_FFT ${DIR_FFT_SRCS})
