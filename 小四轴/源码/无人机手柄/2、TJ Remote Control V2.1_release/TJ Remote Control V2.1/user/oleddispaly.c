/***************************************************************************************
                                      声  明
    本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现
其他不可估量的BUG，天际智联不负任何责任。请勿商用！

程序版本：2.1
更新日期：2018-1-23
程序作者：愤怒的小孩
版权所有：西安天际智联信息技术有限公司
****************************************************************************************/
#include "oleddispaly.h"
#include "oled.h" 
#include "systick.h"
#include "deal_datapacket.h"
#include "stdio.h"

extern float TX_ERROR_PERCENT;         //遥控数据丢包率
// 校(0) 准(1) 成(2) 功(3)
const char JZCG[][16]=
{
	{0x10,0xD0,0xFF,0x90,0x10,0x80,0x48,0x28,0x09,0x0E,0x08,0x28,0x48,0x88,0x00,0x00},
	{0x03,0x00,0xFF,0x00,0x01,0x80,0x80,0x43,0x2C,0x10,0x2C,0x43,0x80,0x80,0x00,0x00},/*"校",0*/

	{0x02,0x0C,0xE0,0x40,0x20,0xF8,0x4F,0x48,0x49,0xFE,0x48,0x48,0x48,0x08,0x00,0x00},
	{0x04,0x7F,0x00,0x00,0x00,0xFF,0x22,0x22,0x22,0x3F,0x22,0x22,0x22,0x20,0x00,0x00},/*"准",1*/

	{0x00,0xF8,0x88,0x88,0x88,0x88,0x08,0x08,0xFF,0x08,0x09,0x0A,0xC8,0x08,0x00,0x00},
	{0x60,0x1F,0x00,0x10,0x20,0x1F,0x80,0x40,0x21,0x16,0x18,0x26,0x41,0xF8,0x00,0x00},/*"成",2*/

	{0x08,0x08,0xF8,0x08,0x08,0x08,0x10,0x10,0xFF,0x10,0x10,0x10,0xF0,0x00,0x00,0x00},
	{0x30,0x10,0x1F,0x08,0x88,0x48,0x30,0x0E,0x01,0x40,0x80,0x40,0x3F,0x00,0x00,0x00},/*"功",3*/
};

// 加(0) 速(1) 度(2)
const char JaSuDu[][16]=
{
	{0x10,0x10,0xFF,0x10,0x10,0xF0,0x00,0x00,0xF8,0x08,0x08,0x08,0xF8,0x00,0x00,0x00},
	{0x40,0x30,0x0F,0x40,0x80,0x7F,0x00,0x00,0x7F,0x20,0x20,0x20,0x7F,0x00,0x00,0x00},/*"加",0*/

	{0x40,0x42,0xCC,0x00,0x04,0xF4,0x94,0x94,0xFF,0x94,0x94,0xF4,0x04,0x00,0x00,0x00},
	{0x40,0x20,0x1F,0x20,0x48,0x44,0x42,0x41,0x5F,0x41,0x42,0x44,0x48,0x40,0x00,0x00},/*"速",1*/

	{0x00,0xFC,0x24,0x24,0x24,0xFC,0x25,0x26,0x24,0xFC,0x24,0x24,0x24,0x04,0x00,0x00},
	{0x30,0x8F,0x80,0x84,0x4C,0x55,0x25,0x25,0x25,0x55,0x4C,0x80,0x80,0x80,0x00,0x00},/*"度",2*/
};

//陀(0) 螺(1) 仪(2)
const char TuoLuoYi[][16]=
{
	{0xFE,0x02,0x22,0xDA,0x06,0x20,0xD8,0x08,0x09,0x0E,0x08,0x88,0x28,0x18,0x00,0x00},
	{0xFF,0x08,0x10,0x08,0x07,0x00,0x3F,0x44,0x44,0x42,0x41,0x40,0x78,0x00,0x00,0x00},/*"陀",0*/

	{0xF8,0x08,0xFF,0x08,0xF8,0x00,0x3E,0xAA,0x6A,0x3E,0x2A,0xAA,0x3E,0x00,0x00,0x00},
	{0x63,0x21,0x1F,0x11,0x39,0x48,0x29,0x09,0x8D,0xFB,0x09,0x08,0x2C,0x58,0x00,0x00},/*"螺",1*/

	{0x80,0x60,0xF8,0x07,0x00,0x1C,0xE0,0x01,0x06,0x00,0xE0,0x1E,0x00,0x00,0x00,0x00},
	{0x00,0x00,0xFF,0x00,0x80,0x40,0x20,0x13,0x0C,0x13,0x20,0x40,0x80,0x80,0x00,0x00},/*"仪",2*/
};

// 气(0) 压(1) 计(2)
const char QiYaJi[][16]=
{
	{0x10,0x4C,0x47,0x54,0x54,0x54,0x54,0x54,0x54,0x54,0xD4,0x04,0x04,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x30,0x40,0xF0,0x00,0x00},/*"气",0*/

	{0x00,0xFE,0x02,0x82,0x82,0x82,0x82,0xFA,0x82,0x82,0x82,0x82,0x82,0x02,0x00,0x00},
	{0x60,0x1F,0x40,0x40,0x40,0x40,0x40,0x7F,0x40,0x40,0x44,0x58,0x40,0x40,0x00,0x00},/*"压",1*/

	{0x40,0x42,0xCC,0x00,0x40,0x40,0x40,0x40,0xFF,0x40,0x40,0x40,0x40,0x40,0x00,0x00},
	{0x00,0x00,0x7F,0x20,0x10,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"计",2*/
};

//磁(0) 力(1) 计(2)
const char CiLiJi[][16]=
{
	{0xE4,0x5C,0xC4,0x00,0x08,0xC8,0x39,0x8E,0x08,0x08,0xCC,0x3B,0x88,0x08,0x00,0x00},
	{0x3F,0x10,0x3F,0x00,0x63,0x5A,0x46,0xE1,0x00,0x63,0x5A,0x46,0xE1,0x00,0x00,0x00},/*"磁",0*/

	{0x10,0x10,0x10,0x10,0x10,0xFF,0x10,0x10,0x10,0x10,0x10,0xF0,0x00,0x00,0x00,0x00},
	{0x80,0x40,0x20,0x18,0x06,0x01,0x00,0x20,0x40,0x80,0x40,0x3F,0x00,0x00,0x00,0x00},/*"力",1*/

	{0x40,0x42,0xCC,0x00,0x40,0x40,0x40,0x40,0xFF,0x40,0x40,0x40,0x40,0x40,0x00,0x00},
	{0x00,0x00,0x7F,0x20,0x10,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"计",2*/
};

// 已(0) 开(1) 启(2)
const char KaiQi[][16]=
{
	{0x00,0x00,0xE2,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0xFE,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x3F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x78,0x00,0x00},/*"已",0*/

	{0x80,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x80,0x00},
	{0x00,0x80,0x40,0x30,0x0F,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00},/*"开",1*/

	{0x00,0x00,0x00,0xFC,0x44,0x44,0x44,0x45,0x46,0x44,0x44,0x44,0x44,0x7C,0x00,0x00},
	{0x40,0x20,0x18,0x07,0x00,0xFC,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0xFC,0x00,0x00},/*"启",2*/
};

// 已(0) 关(1) 闭(2)
const char GuanBi[][16]=
{
	{0x00,0x00,0xE2,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0xFE,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x3F,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x78,0x00,0x00},/*"已",0*/

	{0x00,0x10,0x11,0x16,0x10,0x10,0xF0,0x10,0x10,0x14,0x13,0x10,0x00,0x00,0x00,0x00},
	{0x81,0x41,0x41,0x21,0x11,0x0D,0x03,0x0D,0x11,0x21,0x41,0x41,0x81,0x81,0x00,0x00},/*"关",1*/

	{0xF8,0x01,0x22,0x20,0x22,0x22,0xA2,0xFA,0x22,0x22,0x22,0x02,0xFE,0x00,0x00,0x00},
	{0xFF,0x00,0x08,0x04,0x02,0x11,0x20,0x1F,0x00,0x00,0x40,0x80,0x7F,0x00,0x00,0x00},/*"闭",2*/
};

// 有(0) 头(1) 模(2) 式(3)
const char YouTouMoShi[][16]=
{
	{0x04,0x04,0x04,0x84,0xE4,0x3C,0x27,0x24,0x24,0x24,0x24,0xE4,0x04,0x04,0x04,0x00},
	{0x04,0x02,0x01,0x00,0xFF,0x09,0x09,0x09,0x09,0x49,0x89,0x7F,0x00,0x00,0x00,0x00},/*"有",0*/

	{0x00,0x00,0x10,0x60,0x04,0x18,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x81,0x81,0x41,0x41,0x21,0x11,0x09,0x07,0x01,0x05,0x09,0x11,0x21,0xC1,0x01,0x00},/*"头",1*/

	{0x10,0x10,0xD0,0xFF,0x90,0x14,0xE4,0xAF,0xA4,0xA4,0xA4,0xAF,0xE4,0x04,0x00,0x00},
	{0x04,0x03,0x00,0xFF,0x00,0x89,0x4B,0x2A,0x1A,0x0E,0x1A,0x2A,0x4B,0x88,0x80,0x00},/*"模",2*/

	{0x10,0x10,0x90,0x90,0x90,0x90,0x90,0x10,0x10,0xFF,0x10,0x10,0x11,0x16,0x10,0x00},
	{0x00,0x20,0x60,0x20,0x3F,0x10,0x10,0x10,0x00,0x03,0x0C,0x10,0x20,0x40,0xF8,0x00},/*"式",3*/
};

// 无(0) 头(1) 模(2) 式(3)
const char WuTouMoShi[][16]=
{
	{0x00,0x40,0x42,0x42,0x42,0xC2,0x7E,0x42,0xC2,0x42,0x42,0x42,0x40,0x40,0x00,0x00},
	{0x80,0x40,0x20,0x10,0x0C,0x03,0x00,0x00,0x3F,0x40,0x40,0x40,0x40,0x70,0x00,0x00},/*"无",0*/

	{0x00,0x00,0x10,0x60,0x04,0x18,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x81,0x81,0x41,0x41,0x21,0x11,0x09,0x07,0x01,0x05,0x09,0x11,0x21,0xC1,0x01,0x00},/*"头",1*/

	{0x10,0x10,0xD0,0xFF,0x90,0x14,0xE4,0xAF,0xA4,0xA4,0xA4,0xAF,0xE4,0x04,0x00,0x00},
	{0x04,0x03,0x00,0xFF,0x00,0x89,0x4B,0x2A,0x1A,0x0E,0x1A,0x2A,0x4B,0x88,0x80,0x00},/*"模",2*/

	{0x10,0x10,0x90,0x90,0x90,0x90,0x90,0x10,0x10,0xFF,0x10,0x10,0x11,0x16,0x10,0x00},
	{0x00,0x20,0x60,0x20,0x3F,0x10,0x10,0x10,0x00,0x03,0x0C,0x10,0x20,0x40,0xF8,0x00},/*"式",3*/
};


const char BATT[][16]=
{
	{0xC0,0x40,0x70,0x50,0x70,0x40,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x7F,0x55,0x40,0x55,0x40,0x55,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"电池图标",0*/
};

const char WiFi[][16]=
{
	{0x04,0x0C,0x14,0xFC,0x14,0x0C,0x04,0x00,0x00,0x00,0x00,0x80,0x00,0xE0,0x00,0x00},
	{0x00,0x00,0x00,0x7F,0x00,0x60,0x00,0x78,0x00,0x7E,0x00,0x7F,0x00,0x7F,0x00,0x00},/*"信号5格",0*/

	{0x04,0x0C,0x14,0xFC,0x14,0x0C,0x04,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x7F,0x00,0x60,0x00,0x78,0x00,0x7E,0x00,0x7F,0x00,0x00,0x00,0x00},/*"信号4格",0*/

	{0x04,0x0C,0x14,0xFC,0x14,0x0C,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x7F,0x00,0x60,0x00,0x78,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00},/*"信号3格",0*/

	{0x04,0x0C,0x14,0xFC,0x14,0x0C,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x7F,0x00,0x60,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"信号2格",0*/

	{0x04,0x0C,0x14,0xFC,0x14,0x0C,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x00,0x7F,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"信号1格",0*/

	{0x04,0x0C,0x14,0xFC,0x14,0x0C,0x44,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x40,0x00},
	{0x00,0x00,0x00,0x7F,0x00,0x00,0x40,0x20,0x11,0x0A,0x04,0x0A,0x11,0x20,0x40,0x00},/*"断开连接",0*/
};

void ButtonCMDDisplay(void);

void Display_init(void)
{
	OLED_Clear();//清屏
	
	OLED_ShowCHinese(1,line1,WiFi,0); //信号强度
	OLED_ShowString(byte(3)+4,line1," RoboFly",8); //小四轴名称
	OLED_ShowCHinese7x7(121,line1,BATT,0); //电池图标
	OLED_ShowString(111,line1,"%",6); //%
	OLED_ShowNum(108,line2,99,2,6); //电池电压比例
	
	OLED_ShowString(byte(0),line4,"THR:",6);
	OLED_ShowString(byte(8),line4,"MOD:",6);
	OLED_ShowString(byte(0),line6,"ROL:",6);
	OLED_ShowString(byte(8),line6,"PIT:",6);
	OLED_ShowString(byte(0),line8,"ALT:",6);
	OLED_ShowString(byte(8),line8,"YAW:",6);

}

void SignalIntensityDisplay(void)
{
	if(TX_ERROR_PERCENT<0.2f)
	{
		OLED_ShowCHinese(1,line1,WiFi,0); //信号强度5格
	}else if(TX_ERROR_PERCENT<0.4f)
	{
		OLED_ShowCHinese(1,line1,WiFi,1); //信号强度4格
	}else if(TX_ERROR_PERCENT<0.6f)
	{
		OLED_ShowCHinese(1,line1,WiFi,2); //信号强度3格
	}else if(TX_ERROR_PERCENT<0.8f)
	{
		OLED_ShowCHinese(1,line1,WiFi,3); //信号强度2格
	}else if(TX_ERROR_PERCENT<1.0f)
	{
		OLED_ShowCHinese(1,line1,WiFi,4); //信号强度1格
	}else
	{
		OLED_ShowCHinese(1,line1,WiFi,5); //信号强度0格(断开连接)
	}
}

extern int8_t FLYDataRx_OK;

void Display_Update(void)
{
	float percent =0;
	if(FLYDataRx_OK)
	{
		ReceiveDataAnalysis();
		FLYDataRx_OK = 0; 
		if(FLY.Yaw<0)
		{
			OLED_ShowString(byte(8),line8,"YAW:-",6);
			FLY.Yaw = -1*FLY.Yaw;
		}else
		{
			OLED_ShowString(byte(8),line8,"YAW: ",6);
		}

		if(FLY.Rol<0)
		{
			OLED_ShowString(byte(0),line6,"ROL:-",6);
			FLY.Rol = -1*FLY.Rol;
		}else
		{
			OLED_ShowString(byte(0),line6,"ROL: ",6);
		}

		if(FLY.Pit<0)
		{
			OLED_ShowString(byte(8),line6,"PIT:-",6);
			FLY.Pit = -1*FLY.Pit;
		}else
		{
			OLED_ShowString(byte(8),line6,"PIT: ",6);
		}

	SignalIntensityDisplay(); //信号强度更新显示
	ButtonCMDDisplay(); //
	percent = (FLY.BattV-300.0f)/120.f*100; //电池百分比
		
	OLED_ShowNum(byte(3),line4,FLY.Thr,4,6);  //油门
//	OLED_ShowNum(byte(11),line4,1500,4,6);
	OLED_ShowNum(byte(4),line6,FLY.Rol,2,6);  //横滚角
	OLED_ShowNum(byte(12),line6,FLY.Pit,2,6); //俯仰角
	OLED_ShowNum(byte(12),line8,FLY.Yaw,2,6); //航向角
	OLED_ShowNum(byte(4),line8,FLY.Alt,2,6);  //高度
	OLED_ShowNum(108,line2,percent,2,6); //电池电压比例
	}
}

//传感器校准显示
void SenserOffsetDisplay(void)
{
	//陀螺仪校准
	if(GET_FLAG(GYRO_OFFSET))
	{
		OLED_ClearBlue();
		while(GET_FLAG(GYRO_OFFSET))
		{
			ReceiveDataAnalysis();//继续接收遥控数据
			OLED_ShowCHineseString(byte(5),line4,TuoLuoYi,3); //陀螺仪
			OLED_ShowCHineseString(byte(4),line6,JZCG,4); //校准成功
		}
		Display_init();
	}
	//加速度校准
	if(GET_FLAG(ACC_OFFSET))
	{
		OLED_ClearBlue();
		while(GET_FLAG(ACC_OFFSET))
		{
			ReceiveDataAnalysis();//继续接收遥控数据
			OLED_ShowCHineseString(byte(5),line4,JaSuDu,3); //加速度
			OLED_ShowCHineseString(byte(4),line6,JZCG,4); //校准成功
		}
		Display_init();
	}
	//气压计校准
	if(GET_FLAG(BAR_OFFSET))
	{
		OLED_ClearBlue();
		while(GET_FLAG(BAR_OFFSET))
		{
			ReceiveDataAnalysis();//继续接收遥控数据
			OLED_ShowCHineseString(byte(5),line4,QiYaJi,3); //气压计
			OLED_ShowCHineseString(byte(4),line6,JZCG,4); //校准成功
		}
		Display_init();
	}
}

//WiFi开关旋转显示
void WiFiOnOffDisplay(void)
{
	static u8 WiFi_flag = 0;
	u16 cnt = 0;
	//WiFi开关
	if(GET_FLAG(WiFi_ONOFF))
	{
		if(!WiFi_flag)
		{
			OLED_ClearBlue();
			while(cnt++<20)
			{
				OLED_ShowString(byte(6),line4,"WiFi",8);
				OLED_ShowCHineseString(byte(5),line6,KaiQi,3); //已开启
				delay_ms(60);
			}
			WiFi_flag = 1;
			Display_init();
	 }
	}else
	{
		if(WiFi_flag)
		{
			OLED_ClearBlue();
			while(cnt++<20)
			{
				OLED_ShowString(byte(6),line4,"WiFi",8);
				OLED_ShowCHineseString(byte(5),line6,GuanBi,3); //已关闭
				delay_ms(60);
			}
			WiFi_flag = 0;
			Display_init();
		}
	}
}

//模式选择显示显示
void ModeSelectDisplay(void)
{
	static u8 Mode_flag = 0;
	u16 cnt = 0;
	//模式选择
	if(GET_FLAG(FLY_MODE))
	{
		if(Mode_flag)
		{
			OLED_ClearBlue();
			while(cnt++<20)
			{
				OLED_ShowCHineseString(byte(4),line5,WuTouMoShi,4); //无头模式
				delay_ms(60);
			}
			Mode_flag = 0;
			Display_init();
		}
	}else
	{
		if(!Mode_flag)
		{
			OLED_ClearBlue();
			while(cnt++<20)
			{
				OLED_ShowCHineseString(byte(4),line5,YouTouMoShi,4); //有头模式
				delay_ms(60);
			}
			Mode_flag = 1;
			Display_init();
		}
	}
}

void ButtonCMDDisplay(void)
{
	//飞机解锁还是上锁
	if(GET_FLAG(FLY_ENABLE))
	{
		OLED_ShowString(byte(11),line4,"Unlock",6);
	}else
	{
		OLED_ShowString(byte(11),line4,"Lock  ",6);
	}
	SenserOffsetDisplay();
	WiFiOnOffDisplay();
	ModeSelectDisplay();
}

void ConnectingDisplay(void)
{
	OLED_Clear();
	OLED_ShowCHinese(1,line1,WiFi,0); //信号强度
	OLED_ShowString(byte(3)+4,line1," RoboFly",8); //小四轴名称
	OLED_ShowCHinese7x7(121,line1,BATT,0); //电池图标
	OLED_ShowString(111,line1,"%",6); //%
	OLED_ShowNum(108,line2,0,2,6); //电池电压比例
	OLED_ShowString(byte(2),line4,"Connecting...",8);
}


