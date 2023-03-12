#ifndef __KEY_H
#define __KEY_H



typedef enum {
    none=0,
    key_on,
    key_sel,
    key_up,
    key_down,
    key_up_long,
   	key_down_long,
    key_sel_long,
    key_sel_long_on,

} Key_e;
/**********************按键名与端口映射************************************/
#define KEY_ON_PORT HAL_GPIO_ReadPin(KEY_ON_GPIO_Port,KEY_ON_Pin)
#define KEY_SEL_PORT HAL_GPIO_ReadPin(KEY_SEL_GPIO_Port,KEY_SEL_Pin)
#define KEY_UP_PORT HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,KEY_UP_Pin)
#define KEY_DOWN_PORT HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port,KEY_DOWN_Pin)


/**********************按键参数定义****************************************/
#define KEY_TRUE 0
#define KEY_FAULT (!KEY_TRUE)
#define KEY_DELAY 4
#define KEY_LONG 100
void Key_Scan(void);
Key_e Get_KeyVaule();



#endif