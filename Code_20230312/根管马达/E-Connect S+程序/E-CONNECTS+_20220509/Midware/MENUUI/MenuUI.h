#ifndef __MENUUI_H
#define __MENUUI_H
 

void UI_setmemory();
  void ApexDispMode_1(uint8_t num);
void ApexDispBG();
	void ResLast_x();
	void ResLast_apex_x();
	void DispApexBar(uint8_t num );
	
	void RunDisp();
/****************************************************/
//�������ý���
void DrawGird(uint8_t x,uint8_t y,uint16_t c);
void DispTorqueBar(uint8_t tq ,uint8_t running);
void UI_set_opmode();
void UI_set_opmode_init();
void UI_set_speed();
void UI_set_torque();
void UI_set_angle();
void UI_set_runapex();
void UI_set_autostart();
void UI_set_apaction();
void UI_set_apreference();
/********************************************************/
//ϵͳ���ý���
void UI_set_version();
void UI_set_poweroff();
void UI_set_volume();
void UI_set_light();
void UI_set_hand();
void UI_set_startmem();
void UI_set_cal();
void UI_set_res();
/***********************************************************/
//�ϵͳ����
void UI_filesys();

#endif