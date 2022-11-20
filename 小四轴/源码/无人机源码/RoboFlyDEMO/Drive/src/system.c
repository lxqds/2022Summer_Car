/*******************************************************************************************
										    Éù Ã÷
    ±¾ÏîÄ¿´úÂë½ö¹©¸öÈËÑ§Ï°Ê¹ÓÃ£¬¿ÉÒÔ×ÔÓÉÒÆÖ²ĞŞ¸Ä£¬µ«±ØĞë±£Áô´ËÉùÃ÷ĞÅÏ¢¡£ÒÆÖ²¹ı³ÌÖĞ³öÏÖÆäËû
	
²»¿É¹ÀÁ¿µÄBUG£¬Ìì¼ÊÖÇÁª²»¸ºÈÎºÎÔğÈÎ¡£ÇëÎğÉÌÓÃ£¡

* ³ÌĞò°æ±¾£ºV1.01
* ³ÌĞòÈÕÆÚ£º2018-8-18
* ³ÌĞò×÷Õß£º·ßÅ­µÄĞ¡º¢
* °æÈ¨ËùÓĞ£ºÎ÷°²Ìì¼ÊÖÇÁªĞÅÏ¢¼¼ÊõÓĞÏŞ¹«Ë¾
*******************************************************************************************/
#include "main.h"

void Clock_Init()
{
	/* ??HSI ??????? */
	RCC->CR |= (uint32_t)0x00000001; 
 
	/*??HSI?PLL????HSI??2???PLL*/
	RCC->CFGR |= (uint32_t)RCC_CFGR_PLLSRC_HSI_Div2; 
 
	/*PLLCLK=8/2*12=48MHz   ?????????PLL???*/
    //?????????64MHz??,??????????,??????
	RCC->CFGR |= (uint32_t)RCC_CFGR_PLLMULL12;
 
	/* PLL?????  */
	RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
 
	/* ?? PLL?? */
	RCC->CR |= RCC_CR_PLLON;
 
	/* ??PLL????*/
	while((RCC->CR & RCC_CR_PLLRDY) == 0)
	{
	}
 
	/* ??PLL????????? */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    
 
	/* ??PLL??????????*/
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
	{
	}	

}
void System_Init(void)
{
	//Clock_Init();
	NvicConfig(); //ÏµÍ³ÖĞ¶ÏÓÅÏÈ¼¶¹ÜÀí
	LED_Init();	//ÓÃ»§Ö¸Ê¾µÆ³õÊ¼»¯
	
	Delay_Init(); //ÏµÍ³ÑÓÊ±³õÊ¼»¯
	
	USART_init(460800);	//µ÷ÊÔ´®¿Ú³õÊ¼»¯
	
	IIC_GPIO_Init(); //Ä£ÄâIIC³õÊ¼»¯
	
	TIM_Init();	//ÏµÍ³Ê±»ù³õÊ¼»¯
	
	Exit_Init(); //Íâ²¿ÖĞ¶Ï³õÊ¼»¯
	
	SI24R1_Init(); //SI24R1(2.4G)³õÊ¼»¯£¨ºì£
	
	MPU6050_Init();//MPU6050³õÊ¼»¯£¨ÂÌ£
	
	FBM320_Init(); //FBM320³õÊ¼»¯(ÆøÑ¹¼ÆÀ¶)
		
	MOTOR_Init(); //µç»úÊä³ö³õÊ¼»¯
	
	BATT_Init(); //µç³ØµçÑ¹¼ì²â³õÊ¼»¯
	
	PID_ReadFlash(); //FlashÖĞµÄÊı¾İ¶ÁÈ¡
	
	PidParameter_init(); //PID²ÎÊı³õÊ¼»¯
	RGB_LED_Off();	//³õÊ¼»¯¹Ø±ÕËùÓĞRGBµÆ
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	
	printf("System Init Finish\n");
}

void Task_Schedule(void)
{
	if(ANO_Scan) //500Hz
		{
			ANO_Scan = 0;
			ANO_DT_Data_Exchange(); //¸üĞÂÊı¾İµ½ÉÏÎ»»ú
		}
		if(IMU_Scan) //100Hz
		{
			IMU_Scan  = 0;
			Prepare_Data(); //»ñÈ¡×ËÌ¬½âËãËùĞèÊı¾İ
			IMUupdate(&Gyr_rad,&Acc_filt,&Att_Angle); //ËÄÔªÊı×ËÌ¬½âËã
			Control(&Att_Angle,&Gyr_rad,&RC_Control,Airplane_Enable); //×ËÌ¬¿ØÖÆ
		}
		if(LED_Scan) //10Hz
		{
			LED_Scan = 0;
			LED_Run();
			if(!Airplane_Enable&&Run_flag)
			{
				RGB_LED_Runing(); //·É»úÉÏËø×´Ì¬µÆ
			}
			BATT_Alarm_LED(); //µç³ØµÍµçÑ¹±¨¾¯	  
		}
		if(IRQ_Scan) //5Hz
		{
			IRQ_Scan = 0;
			SI24R1_SingalCheck(); //2.4GÍ¨ĞÅ¼ì²â
			SendToRemote(); //·¢ËÍÊı¾İ¸øÒ£¿ØÆ÷
		}
		if(Batt_Scan) //2.5Hz
		{
			Batt_Scan = 0;
			SI24R1_GetAddr(); //·ÖÅä2.4GµØÖ·
			LowVoltage_Alarm();	//µÍµçÁ¿±¨¾¯
		}
}


