#ifndef __PRESSURE_ORIGIN_SEARCH_H
#define __PRESSURE_ORIGIN_SEARCH_H
#include <stdint.h>
#include <stdbool.h>

/*	液压零点寻找：
	目标：在丝杆回退过程中，出现负压时立刻停止电机，此刻的位置作为液压原点；
	机制：在整个丝杆运动过程中，最后100圈时启动液压原点寻找程序；
		  采用液压前后差分值与设定的阈值做比较（注意，液压数据应是单调递减）：
		  若差分值超过设定阈值，认为到达液压原点，返回true,
		  若差分值未超过设定阈值，丝杆继续回退，返回false	*/
bool pressure_origin_search(uint16_t current_pressure, int16_t left_distance);

#endif
