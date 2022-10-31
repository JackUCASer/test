// 这是一个多个目录，多个源文件编译示例
#include <stdio.h>
#include <stdlib.h>
#include "./math/MathFunctions.h"
#include "./filter/Hampel_filter.h"
#include "./fft/FFT_MATH.h"


int main()
{
    double base = 3;
    int exponent = 5;
    double result = power(base, exponent);
    printf("%g ^ %d is %g\n", base, exponent, result);
    
    printf("--------------------------Hampel Unit test-----------------------\r\n");
    Hampel_Filter_Unit_Test_by_Collect_Data();
    
    printf("--------------------------FFT Unit test-----------------------\r\n");
    FFT_Unit_Test_by_Simulate_Data();
    printf("\r\n");
    IFFT_Unit_Test_by_Simulate_Data();
    printf("\r\n");
    FFTR_Unit_Test_by_Simulate_Data();
    printf("\r\n");
    IFFTR_Unit_Test_by_Simulate_Data();
    return 0;
}