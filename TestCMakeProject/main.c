#include <stdio.h>
#include <stdint.h>
#include "add.h"
#include "Hampel_filter.h"

int main()
{
    int16_t a = 32;
    int16_t b = -99;
    printf("a + b = %d\r\n", add_function(a, b));
    Hampel_Filter_Unit_Test_by_Simulate_Data();
    printf("Unit test 2:----------------------------\r\n");
    Hampel_Filter_Unit_Test_by_Collect_Data();
    while(1);
    return 0;
}