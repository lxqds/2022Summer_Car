/*******************************************************************************************
										    �� ��
    ����Ŀ�����������ѧϰʹ�ã�����������ֲ�޸ģ������뱣����������Ϣ����ֲ�����г�������
	
���ɹ�����BUG��������������κ����Ρ��������ã�

* ����汾��V1.01
* �������ڣ�2018-8-18
* �������ߣ���ŭ��С��
* ��Ȩ���У��������������Ϣ�������޹�˾
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
	NvicConfig(); //ϵͳ�ж����ȼ�����
	LED_Init();	//�û�ָʾ�Ƴ�ʼ��
	
	Delay_Init(); //ϵͳ��ʱ��ʼ��
	
	USART_init(460800);	//���Դ��ڳ�ʼ��
	
	IIC_GPIO_Init(); //ģ��IIC��ʼ��
	
	TIM_Init();	//ϵͳʱ����ʼ��
	
	Exit_Init(); //�ⲿ�жϳ�ʼ��
	
	SI24R1_Init(); //SI24R1(2.4G)��ʼ������
	
	MPU6050_Init();//MPU6050��ʼ�����̣
	
	FBM320_Init(); //FBM320��ʼ��(��ѹ����)
		
	MOTOR_Init(); //��������ʼ��
	
	BATT_Init(); //��ص�ѹ����ʼ��
	
	PID_ReadFlash(); //Flash�е����ݶ�ȡ
	
	PidParameter_init(); //PID������ʼ��
	RGB_LED_Off();	//��ʼ���ر�����RGB��
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	
	printf("System Init Finish\n");
}

void Task_Schedule(void)
{
	if(ANO_Scan) //500Hz
		{
			ANO_Scan = 0;
			ANO_DT_Data_Exchange(); //�������ݵ���λ��
		}
		if(IMU_Scan) //100Hz
		{
			IMU_Scan  = 0;
			Prepare_Data(); //��ȡ��̬������������
			IMUupdate(&Gyr_rad,&Acc_filt,&Att_Angle); //��Ԫ����̬����
			Control(&Att_Angle,&Gyr_rad,&RC_Control,Airplane_Enable); //��̬����
		}
		if(LED_Scan) //10Hz
		{
			LED_Scan = 0;
			LED_Run();
			if(!Airplane_Enable&&Run_flag)
			{
				RGB_LED_Runing(); //�ɻ�����״̬��
			}
			BATT_Alarm_LED(); //��ص͵�ѹ����	  
		}
		if(IRQ_Scan) //5Hz
		{
			IRQ_Scan = 0;
			SI24R1_SingalCheck(); //2.4Gͨ�ż��
			SendToRemote(); //�������ݸ�ң����
		}
		if(Batt_Scan) //2.5Hz
		{
			Batt_Scan = 0;
			SI24R1_GetAddr(); //����2.4G��ַ
			LowVoltage_Alarm();	//�͵�������
		}
}


