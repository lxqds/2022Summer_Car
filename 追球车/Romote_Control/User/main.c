#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "usart.h"
#include "stdio.h"
uint8_t Key=0;
uint8_t Stop_Flag=0;
char usart3Buf[64];
char usart3Len;
uint8_t ReceiveData;

//定义两个结构体
typedef union float_Data
{
	unsigned char byte[4];
	float Float_Data;
}Float_Byte;
typedef struct Sensor_Data
{
	int16_t X;
	int16_t Y;
	Float_Byte D;
}SensorData;
//整形结构体，为了转换浮点型数据
typedef struct Struct_Int
{
	int Z;
	int a;
	int b;
	int c;
}Struct_Int;

Struct_Int Num1;
Struct_Int Dispart_Float(float *Float_Num)
{
	Struct_Int Temp;
	Temp.Z = (int)*Float_Num;
	Temp.a = (*Float_Num-(int)*Float_Num)*1000;
	return Temp;
}

SensorData SensorData1;//方便移植

int main(void)
{
	OLED_Init();
	LED_Init();
	Key_Init();
	uart_init(115200);

	OLED_ShowString(4,5,"-Stop");
	while (1)
	{
		if(Key)
		OLED_ShowNum(4,2, Key, 2);
		OLED_ShowString(1,5,"Distance");
		OLED_ShowString(2,11,"CM");
		Num1 = Dispart_Float(&SensorData1.D.Float_Data);		
		OLED_ShowNum(2,5,Num1.Z,3);
		OLED_ShowChar(2,8,'.');
		OLED_ShowNum(2,9,Num1.a,2);
		Key=Key_GetNum();
		if(Key==3)
		{
			RGB_LED_Red();
			Stop_Flag= 1;
			OLED_ShowString(4,5,"-Stop");
			USART_SendData(USART1,0xAA);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
			USART_SendData(USART1,0x55);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
			USART_SendData(USART1,Stop_Flag);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
			//printf("%x%x%x",0xAA,0x55,Stop_Flag);
		}
		if(Key==4)
		{
			RGB_LED_green();
			Stop_Flag = 0;
			OLED_ShowString(4,5,"Start");
			USART_SendData(USART1,0xAA);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
			USART_SendData(USART1,0x55);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
			USART_SendData(USART1,Stop_Flag);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
			//printf("%x%x%x",0xAA,0x55,Stop_Flag);
		}
		switch(Stop_Flag)
		{
			case 0:RGB_LED_green();OLED_ShowString(4,5,"Start");break;
			case 1:RGB_LED_Red();OLED_ShowString(4,5,"-Stop");break;
		}
	}
}

void Get_Data_To_Array(uint8_t ByteData);
void Get_Remote_Data(uint8_t ByteData);
void USART1_IRQHandler(void)
{
				uint8_t ucTemp;
				if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
				{                
								ucTemp = USART_ReceiveData(USART1);
								Get_Data_To_Array(ucTemp);
								Get_Remote_Data(ucTemp);	
//								printf("串口发送数据：%d\n",Start_Flag);
				}         
}
uint8_t Error_State = 1;
void Get_Remote_Data(uint8_t ByteData)
{
	static uint8_t Rec_State = 0;
	static uint8_t Rec_Buff[9]= {0};
	Rec_Buff[Rec_State] = ByteData;
	switch(Rec_State)
	{
		case 0:if(ByteData == 0xAA) Rec_State = 1;else Rec_State = 0;break;
		case 1:Stop_Flag =  0x01 & Rec_Buff[1]; Rec_State = 2;break;
		case 2:Rec_State = 3;break;
		case 3:Rec_State = 4;break;
		case 4:Rec_State = 5;break;
		case 5:Rec_State = 6;break;
		case 6:Rec_State = 7;break;
		case 7:SensorData1.D.byte[3] = Rec_Buff[7];SensorData1.D.byte[2] = Rec_Buff[6];SensorData1.D.byte[1] = Rec_Buff[5];SensorData1.D.byte[0]=Rec_Buff[4];Rec_State = 8;break;
		case 8:if(ByteData ==0x55) Rec_State = 9;else Error_State =1;Rec_State = 0;break;
		case 9:Error_State = 0;Rec_State = 0;
	}
}

//接收处理函数
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
