/*
 * @Description: 
 * @Author: Liang xiaoqi
 * @Date: 2022-07-25 14:47:54
 * @LastEditTime: 2022-07-27 16:31:48
 * @LastEditors: Liang xiaoqi
 */
#include "sysinit.h"
#include "delay.h"
#include "oled.h"
#include "usart.h"
#include "key.h"
#include "led.h"
#include "bsp_motor.h"
#include "bsp_exti.h"
#include "timA.h"
#include "Reflectance.h"
#define HONG BITBAND_PERI(P7IN, 0) //读取按键2
uint8_t SWJ_buffer[20] = {0};

uint8_t ble_com=0;
extern int MotorA_speed[4];


uint16_t XUNJIData;
int16_t XUNJIBias;
void All_init(void){
    SysInit();    
    delay_init(); 
    OLED_Init(); 
	LED_Init();
	TimA0_Int_Init(19999,48);//50Hz
	motor_Init();
	uart_init(115200);
	Motor_Init_bianmaqi();
	xvanxian();
	Reflectance_Init();
	KEY_Init();
	// MAP_SysCtl_setWDTPasswordViolationResetType(SYSCTL_SOFT_RESET);
	// MAP_WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_SMCLK,WDT_A_CLOCKITERATIONS_8192K);
	// MAP_WDT_A_startTimer();
}
uint8_t StopFlag = 0;
uint8_t OneMeter_Flag = 0;
uint8_t Key =0;
uint8_t Mode = 0;
uint8_t Next_Mode =0;

static uint16_t Last_XUNJIBias=0,LL_Bias=0;

int main(void)
{
	All_init();
	// motor_c_control(0,(uint16_t)1000);
	// motor_d_control(0,(uint16_t)1000);
	
	
	Motor_stop_all();
	printf("hello\r\n");

	Motor_forward();
    while (1)
    {
			OLED_ShowNum(32,4,Mode,3,16);
			OLED_ShowNum(64,4,MotorA_speed[0],3,16);
			OLED_ShowNum(64,6,MotorA_speed[3],3,16);
			
			XUNJIData = Reflectance_Read(1000);
			OLED_ShowBin(32,0,XUNJIData,8,16);
//			OLED_ShowString(0,4,"Mode",16);
			if(Next_Mode != Mode )
			{
				delay_ms(3000);
				Mode = Next_Mode;
			}
				
			switch(Key)
			{
				case 0:break;
				case 1:Next_Mode = 1;break;
				case 2:Next_Mode = 2;break;
			}
			switch(Mode)
			{
				//0.3m/s下跑
				case 1:
				{
					switch(XUNJIData)
					{
						case 0x18:XUNJIBias = 0;break;
						
						case 0x00:XUNJIBias = 40;break;//0000 0000
						case 0x10:XUNJIBias = 0;break;//0001 0000
						case 0x08:XUNJIBias = 0;break;//0000 1000

						
						
						case 0x20:XUNJIBias = -20;break;//0010 0000
						case 0x70:XUNJIBias = -20;break;//1110 0000
						case 0x40:XUNJIBias = -55;break;//0100 0000右转
						case 0xC0:XUNJIBias = -60;break;//1100 0000
						case 0x80:XUNJIBias = -70;break;//1000 0000
						case 0x44:XUNJIBias = -20;break;//0100 0100
						case 0x84:XUNJIBias = -40;break;//1000 0100
						case 0x48:XUNJIBias = -20;break;//0100 1000
						case 0x88:XUNJIBias = -40;break;//1000 1000
						
						case 0x04:XUNJIBias = 20;break; //0000 0100
						case 0x0E:XUNJIBias = 30;break; //0000 0111左转
						case 0x02:XUNJIBias = 55;break; //0000 0010左转
						case 0x01:XUNJIBias = 60;break;//0000 0011
						case 0x03:XUNJIBias = 70;break;//0000 0001
						case 0x41:XUNJIBias = 20;break;//0100 1000
						case 0x11:XUNJIBias = 40;break;//1000 1000
						
						case 0x3C:StopFlag = 1;break;
						case 0xFF:StopFlag = 1;break;
						
						default:XUNJIBias = 40;break;
					
					}break;
				}
			case 2:
			{
				//1m/s下跑
				switch(XUNJIData)
					{
						
						case 0x18:XUNJIBias = 0;break;
						
						case 0x00:		
											if(Last_XUNJIBias > 50)XUNJIBias = 75;
											else XUNJIBias = 0;
											break;//0000 0000
						case 0x10:XUNJIBias = 0;break;//0001 0000
						case 0x08:XUNJIBias = 0;break;//0000 1000

						case 0x20:XUNJIBias = -20;break;//0010 0000
						case 0x70:XUNJIBias = -30;break;//1110 0000
						case 0x40:XUNJIBias = -55;break;//0100 0000右转
						case 0xC0:XUNJIBias = -60;break;//1100 0000
						case 0x80:XUNJIBias = -70;break;//1000 0000
						case 0x44:XUNJIBias = -30;break;//0100 0100
						case 0x84:XUNJIBias = -40;break;//1000 0100
						case 0x48:XUNJIBias = -30;break;//0100 1000
						case 0x88:XUNJIBias = -40;break;//1000 1000
						
						case 0x04:XUNJIBias = 20;break; //0000 0100
						case 0x0E:XUNJIBias = 30;break; //0000 0111左转
						case 0x02:XUNJIBias = 60;break; //0000 0010左转
						case 0x01:XUNJIBias = 65;break;//0000 0011
						case 0x03:XUNJIBias = 75;break;//0000 0001
						case 0x41:XUNJIBias = 50;break;//0100 1000
						case 0x11:XUNJIBias = 50;break;//1000 1000
						
						case 0x3C:StopFlag = 1;break;
						case 0xFF:StopFlag = 1;break;
						
						default:XUNJIBias = 50;break;
					}
						Last_XUNJIBias = XUNJIBias;
						LL_Bias = Last_XUNJIBias;
			}
			default:break;
									
		}	
	
	}

	
}
void EUSCIA3_IRQHandler(void){
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) //接收中断
    {
		ble_com = MAP_UART_receiveData(EUSCI_A2_BASE);
        UART_transmitData(EUSCI_A2_BASE, ble_com); //发送数据
    }

}


