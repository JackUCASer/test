#ifndef __PRESSURE_ORIGIN_SEARCH_H
#define __PRESSURE_ORIGIN_SEARCH_H
#include <stdint.h>
#include <stdbool.h>

/*	Һѹ���Ѱ�ң�
	Ŀ�꣺��˿�˻��˹����У����ָ�ѹʱ����ֹͣ������˿̵�λ����ΪҺѹԭ�㣻
	���ƣ�������˿���˶������У����100Ȧʱ����Һѹԭ��Ѱ�ҳ���
		  ����Һѹǰ����ֵ���趨����ֵ���Ƚϣ�ע�⣬Һѹ����Ӧ�ǵ����ݼ�����
		  �����ֵ�����趨��ֵ����Ϊ����Һѹԭ�㣬����true,
		  �����ֵδ�����趨��ֵ��˿�˼������ˣ�����false	*/
bool pressure_origin_search(uint16_t current_pressure, int16_t left_distance);

#endif
