#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure3;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); 

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    
    GPIO_InitStructure3.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
    GPIO_InitStructure3.GPIO_Mode = GPIO_Mode_IPU;   
    GPIO_InitStructure3.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOB, &GPIO_InitStructure3);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure2;
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure2);
	
	

}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0);
		Delay_ms(20);
		KeyNum = 2;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
		Delay_ms(20);
		KeyNum = 3;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0);
		Delay_ms(20);
		KeyNum = 4;
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0);
		Delay_ms(20);
		KeyNum = 5;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0);
		Delay_ms(20);
		KeyNum = 6;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0);
		Delay_ms(20);
		KeyNum = 7;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0);
		Delay_ms(20);
		KeyNum = 8;
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0);
		Delay_ms(20);
		KeyNum = 9;
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0);
		Delay_ms(20);
		KeyNum = 10;
	}
	return KeyNum;
}
uint8_t Key_GetState(void)
{
	uint8_t KeyNumber = 0;
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
    {
        KeyNumber = 1;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
    {
        KeyNumber = 2;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
    {
        KeyNumber = 3;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
    {
        KeyNumber = 4;
    }
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)
    {
        KeyNumber = 5;
    }
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) == 0)
    {
        KeyNumber = 6;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
    {
        KeyNumber = 7;
    }
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) == 0)
    {
        KeyNumber = 8;
    }
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12) == 0)
    {
        KeyNumber = 9;
    }
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_13) == 0)
    {
        KeyNumber = 10;
    }
    return KeyNumber;
}
uint8_t KeyNumber_Timer;

uint8_t T_GetKey(void)
{
    uint8_t Temp = 0;
    Temp = KeyNumber_Timer;
    KeyNumber_Timer = 0;
    return Temp;
}
void Key_Loop(void)
{
    static uint8_t NowKey, LastKey;
    LastKey = NowKey;
    NowKey = Key_GetState();
    if (LastKey == 1 && NowKey == 0)
    {
        KeyNumber_Timer = 1;
    }
    if (LastKey == 2 && NowKey == 0)
    {
        KeyNumber_Timer = 2;
    }
    if (LastKey == 3 && NowKey == 0)
    {
        KeyNumber_Timer = 3;
    }
    if (LastKey == 4 && NowKey == 0)
    {
        KeyNumber_Timer = 4;
    }
    if (LastKey == 5 && NowKey == 0)
    {
        KeyNumber_Timer = 5;
    }
		if (LastKey == 6 && NowKey == 0)
    {
        KeyNumber_Timer = 6;
    }
    if (LastKey == 7 && NowKey == 0)
    {
        KeyNumber_Timer = 7;
    }
    if (LastKey == 8 && NowKey == 0)
    {
        KeyNumber_Timer = 8;
    }
    if (LastKey == 9 && NowKey == 0)
    {
        KeyNumber_Timer = 9;
    }
    if (LastKey == 10 && NowKey == 0)
    {
        KeyNumber_Timer = 10;
    }
}
