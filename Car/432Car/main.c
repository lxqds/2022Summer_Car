/*
 * @Description: 
 * @Author: Liang xiaoqi
 * @Date: 2022-07-25 14:47:54
 * @LastEditTime: 2022-08-18 16:00:11
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
#include "Control.h"
#include "PID.h"
#include "main.h"



static uint8_t ble_com=0;
extern int MotorA_speed[4];

SensorData 	SensorData1;
float Turn_Velocity ,Distance_Velocity,Left_Velocity,Right_Velocity;
void All_init(void)
{
  SysInit();    
  delay_init(); 
  OLED_Init(); 
	LED_Init();
	TimA0_Int_Init(29999,48);//50Hz
	motor_Init();
	uart_init(115200);
	uart1_init(115200);
	uart2_Init(115200);
	Motor_Init_bianmaqi();
	KEY_Init();
}

Struct_Int Num1;
Struct_Int Dispart_Float(float *Float_Num)
{
	Struct_Int Temp;
	Temp.Z = (int)*Float_Num;
	Temp.a = (*Float_Num-(int)*Float_Num)*1000;
	return Temp;
}


void OLED_Show_Data()
{
	Num1 = Dispart_Float(&SensorData1.D.Float_Data);		
	OLED_ShowNum(0,0,Num1.Z,3,16);
	OLED_ShowChar(24,0,'.',16);
	OLED_ShowNum(32,0,Num1.a,3,16);
		
	OLED_ShowNum(64,0,SensorData1.X,3,16);
	OLED_ShowNum(64,2,SensorData1.Y,3,16);
	OLED_ShowNum(0,2,MotorA_speed[0],3,16);
	OLED_ShowNum(0,4,MotorA_speed[3],3,16);	
	if(Distance_Velocity < 0)
		{
			OLED_ShowChar(48,4,'D',16);
			OLED_ShowChar(56,4,'-',16);
			OLED_ShowNum(64,4,-Distance_Velocity,3,16);
		}
		else
		{
			OLED_ShowChar(48,4,'D',16);
			OLED_ShowChar(56,4,'+',16);
			OLED_ShowNum(64,4,Distance_Velocity,3,16);
		}
		if(Turn_Velocity < 0)
		{
			OLED_ShowChar(48,6,'T',16);
			OLED_ShowChar(56,6,'-',16);
			OLED_ShowNum(64,6,-Turn_Velocity,3,16);
		}
		else
		{
			OLED_ShowChar(48,6,'T',16);
			OLED_ShowChar(56,6,'+',16);
			OLED_ShowNum(64,6,Turn_Velocity,3,16);
		}
}

float My_Abs(float Sign_Data)
{
	if(Sign_Data <0)
	{
		Sign_Data = -Sign_Data;
	}
	else 
	{
		Sign_Data = Sign_Data;
	}
	return Sign_Data;
}
_PID1 Distance_PID;
_PID1 Turn_PID;

uint8_t TIM20MS_Flag=0;
uint8_t Stop_Flag =1;//停止还是运动
uint8_t Key = 0;
void TIM20MS_HandleData(void);
void Data_Handle(void);

Car_Data  Car_Data1;
Uart_Car_Data Uart_Car_Data1;
uint8_t Stop_Time_Flag=1;
int main(void)
{
	Motor_stop_all();
	All_init();
	LED_G_On();

	Motor_stop_all();
	PID1_Param_Init(&Distance_PID);
	PID1_Param_Init(&Turn_PID);
	PID1_Param_Input(&Distance_PID,-2,-0.25,-3);//30 1 0 //6,0.5,6 //3,0.25,3 /2 //1.5,0.25,1.5 -3,-0.01,-3 //-2.2 -0.25 -3
	PID1_Param_Input(&Turn_PID,1.5,0.1,2);//2,0.05,4 //1,0.1,5//2,0.01,1 //4,0.3,6 //2,0.01,3 -2,-0.01,-3 //1.8 0.1 2
    while (1)
    {
			if(MotorA_speed[0] == 0 && MotorA_speed[3]==0)
			{
				Stop_Time_Flag = 1;
			}
		Key = KEY_Scan(0);
		OLED_Show_Data();	
		if(Key == 1)
		{
			Stop_Flag=!Stop_Flag;
		}
		Uart_Car_Data1.Car_Data1.State = 0x01 & Stop_Flag;//只取最后一个状态位
		Uart_Car_Data1.Car_Data1.State2 = 0x01 ;//只取最后一个状态位
		Uart_Car_Data1.Car_Data1.State3 = 0x01 ;//只取最后一个状态位
		Uart_Car_Data1.Car_Data1.D.Float_Data = SensorData1.D.Float_Data;//把浮点型数据传给共用体，便于收发字符型数据
		Uart_Car_Data1.Car_Data1.Data_Hand1 = 0xAA;
		Uart_Car_Data1.Car_Data1.Data_Tail = 0x55;
		for(uint8_t i = 0;i < 9; i++)
		{
			MAP_UART_transmitData(EUSCI_A2_BASE,Uart_Car_Data1.Byte[i]);//会发送9个字节
		}
		
		switch(Stop_Flag)
		{
			case 0:
			{
				LED_G_On();
				

				if(TIM20MS_Flag==1)
				{
					TIM20MS_Flag = 0;
					Data_Handle();
					TIM20MS_HandleData();
				}
			}break;
			case 1:
			{
				LED_G_Off();
				Motor_stop_all();
			}break;
		}	
	}
}

void Data_Handle(void)
{
		static float Last_Bias_D=0;
		float Bias_T=0,Bias_D=0;
		Bias_T=160,Bias_D=180;
		if(SensorData1.X<150)//固定在中间
		{
			Bias_T =(float) (SensorData1.X);
		}	
		else if(SensorData1.X>170)
		{
			Bias_T =(float) (SensorData1.X);
		}
		else
		{
			Bias_T =160;
		}
		if(SensorData1.D.Float_Data<17)
			{
				Bias_D = SensorData1.D.Float_Data*10;
			}
			else if(SensorData1.D.Float_Data>19)
			{
				Bias_D = SensorData1.D.Float_Data*10;
			}
			else
			{
				Bias_D = 180;
			}
		if(SensorData1.Y>200)//如果球在底下，传上一次偏差
				Bias_D = Last_Bias_D;
		Last_Bias_D = Bias_D;
	PID1_Input_Data(&Distance_PID,Bias_D,180);	
	PID1_Input_Data(&Turn_PID,Bias_T,160);
	
}



//设置了标志位，每40ms处理一次
void TIM20MS_HandleData(void)
{
	Distance_Velocity = PID1_Run(&Distance_PID,Distance_PID.Actual_Val,Distance_PID.Target_Val);
	Turn_Velocity  = PID1_Run(&Turn_PID,Turn_PID.Actual_Val,Turn_PID.Target_Val);
	//限幅
	if(Distance_Velocity >300)
		Distance_Velocity = 300;
	if(Distance_Velocity <-300)
		Distance_Velocity =-300;
	if(Turn_Velocity >400)
		Turn_Velocity =400;
	if(Turn_Velocity<-400)
		Turn_Velocity =-400;
	
	if(Distance_PID.Actual_Val == -1||SensorData1.D.Float_Data==-1||SensorData1.X==-1)
	{
		Car_Run(0,0);//第一个为左轮
	}
	else
	{

		
		Car_Run(Distance_Velocity-Turn_Velocity,Distance_Velocity+Turn_Velocity);//可以修改正负改变转
	}
}

void EUSCIA0_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) //接收中断
    {
			ble_com = MAP_UART_receiveData(EUSCI_A0_BASE);
        MAP_UART_transmitData(EUSCI_A0_BASE, ble_com); //发送数据
    }
}

void Get_Remote_Data(uint8_t ByteData);
void EUSCIA2_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
		
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) //接收中断
    {
			ble_com = MAP_UART_receiveData(EUSCI_A2_BASE);
			Get_Remote_Data(ble_com);
    }
}

void Get_Data_To_Array(uint8_t ByteData);
uint8_t TempData;
void EUSCIA3_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A3_BASE);
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) //接收中断
    {
			TempData = MAP_UART_receiveData(EUSCI_A3_BASE);
			Get_Data_To_Array(TempData);
    }
}

void Get_Remote_Data(uint8_t ByteData)
{
	static uint8_t Rec_State = 0;
	static uint8_t Rec_Buff[3]= {0};
	Rec_Buff[Rec_State] = ByteData;
	switch(Rec_State)
	{
		case 0:if(ByteData == 0xAA) Rec_State = 1;else Rec_State = 0;break;
		case 1:if(ByteData == 0x55) Rec_State = 2;else Rec_State = 0;break;
		case 2:Stop_Flag = Rec_Buff[2]&0x01;Rec_State = 0;break;//可拓展，接收更多信号
	}
}

//接受数据处理
void Get_Data_To_Array(uint8_t ByteData)
{
	static uint8_t Rec_State =0;
	static uint8_t Rec_Buff[12]={0};
	Rec_Buff[Rec_State] = ByteData;
	switch(Rec_State)
	{
		case 0:if(ByteData ==0x23) Rec_State =1;else Rec_State = 0;break;//判断是否为数据包的头部1
		case 1:if(ByteData ==0x66) Rec_State =2;else Rec_State = 0;break;//判断是否为数据包的头部2
		case 2:Rec_State = 3;break;											//存储一个数据
		case 3:SensorData1.X = Rec_Buff[3]<<8|Rec_Buff[2];Rec_State = 4;break;//把字符型数据转换成int16_t
		case 4:Rec_State = 5;break;											//存储一个数据
		case 5:SensorData1.Y = Rec_Buff[5]<<8|Rec_Buff[4];Rec_State =6;break;//把字符型数据转换成int16_t
		case 6:Rec_State = 7;break;											//存储一个数据
		case 7:Rec_State = 8;break;											//存储一个数据
		case 8:Rec_State = 9;break;											//存储一个数据
		case 9:SensorData1.D.byte[3] = Rec_Buff[9];SensorData1.D.byte[2] = Rec_Buff[8];SensorData1.D.byte[1] = Rec_Buff[7];SensorData1.D.byte[0]=Rec_Buff[6];Rec_State = 10;break;//把字符型数据转换成float
		case 10:if(ByteData == 0x11) Rec_State = 11;else Rec_State = 0;break;//判断是否为数据包的尾部2
		case 11:Rec_State =0;break;
	}	
}		
	
