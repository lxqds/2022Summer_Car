#include "stm32f10x.h"
#include "OLED_Font.h"
uint8_t OLED_GRAM[144][8];
/*引脚配置*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)(x))
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)(x))

/*引脚初始化*/
void OLED_I2C_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置低4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置高4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}
//void Oled_Point(unsigned char page,uint8_t y){
//	OLED_WR_Byte(0x20,OLED_CMD);
//	OLED_WR_Byte(0x02,OLED_CMD);
//	OLED_WR_Byte(0xb0+page,OLED_CMD);
//	OLED_WR_Byte(((y&0xf0)>>4)|0x10,OLED_CMD);
//	OLED_WR_Byte((y&0x0f)|0x00,OLED_CMD);
//}
//void Oled_Write(uint8_t data){
//	OLED_WR_Byte(data,OLED_DATA);
//}
//void UI_Init(void){//界面初始化
//	uint8_t i=0;
//	Oled_Point(0,40);
//	for(i=0;i<88;i++)
//		Oled_Write(0x07);
//	Oled_Point(5,40);	
//		for(i=0;i<88;i++)
//		Oled_Write(0xE0);
//	for(i=0;i<6;i++){
//		Oled_Point(i,41);
//		Oled_Write(0xff);
//		Oled_Point(i,40);
//		Oled_Write(0xff);
//		Oled_Point(i,42);
//		Oled_Write(0xff);
//		
//		Oled_Point(i,125);
//		Oled_Write(0xff);
//		Oled_Point(i,126);
//		Oled_Write(0xff);
//		Oled_Point(i,127);
//		Oled_Write(0xff);
//	}
//	for(i=0;i<7;i++){
//		Oled_Point(0,i);
//		Oled_Write(word[0][i]);
//		Oled_Point(1,i);
//		Oled_Write(word[1][i]);
//		Oled_Point(2,i);
//		Oled_Write(word[2][i]);
//		Oled_Point(3,i);
//		Oled_Write(word[3][i]);
//		Oled_Point(4,i);
//		Oled_Write(word[4][i]);
//	}
//	for(i=0;i<9;i++){
//		Oled_Point(0,i+23);
//		Oled_Write(word2[0][i]);
//		Oled_Point(1,i+23);
//		Oled_Write(word2[1][i]);
//		Oled_Point(2,i+23);
//		Oled_Write(word2[2][i]);
//		Oled_Point(3,i+23);
//		Oled_Write(word2[3][i]);
//	}
//	for(i=0;i<16;i++){
//		Oled_Point(0,i+7);
//		Oled_Write(number[0][i]);
//		Oled_Point(1,i+7);
//		Oled_Write(number[0][i]);
//		Oled_Point(2,i+7);
//		Oled_Write(number[0][i]);
//		Oled_Point(3,i+7);
//		Oled_Write(number[0][i]);
//	}
//}
//int score=0;
//uint8_t Block_P[21][12];
//struct posion{//各种坐标
//	char x;
//	char y;
//}orgin,get,move,curBk;
//void Game_Init(){//初始化游戏数据
//	uint8_t i=0,j=0;
//	score=0;
//	for(i=0;i<21;i++)
//		for(j=0;j<12;j++)
//			Block_P[i][j]=0;
//	for(i=0;i<21;i++){
//		Block_P[i][0]=1;
//		Block_P[i][11]=1;
//	}
//	for(i=0;i<12;i++)
//		Block_P[20][i]=1;
//	move.x=4;
//	move.y=0;
//}
//uint8_t nextblock;
//uint8_t Teblock[7][4]={//方块种类“”很巧妙的方法不是我原创的
//0,2,4,6, // I
//0,2,3,5, // Z 1 型
//1,2,3,4, // Z 2 型
//1,2,3,5, // T
//1,3,4,5, // L
//0,1,3,5, // J
//0,1,2,3, // 田
//};
//unsigned int count,rand;
//uint8_t Blockkind=0,spintime=0,nextblock;
//uint8_t sin_[4] = { 0,1,0,-1 };//旋转的角度值
//uint8_t cos_[4] = { 1,0,-1,0 };
//void nextappear(){//预显示下一个方块
//	uint8_t x,y,i;
//	for(i=0;i<16;i++){
//		Oled_Point(6,i+10);
//		Oled_Write(0x00);
//		Oled_Point(7,i+10);
//		Oled_Write(0x00);
//	}
//	for(i=0;i<4;i++){
//		x=Teblock[nextblock][i]%2;
//		y=Teblock[nextblock][i]/2;
//		Oled_Point(6+x,10+y*4);
//		if(x){Oled_Write(0x0f);}else{Oled_Write(0xf0);}
//		if(x){Oled_Write(0x09);}else{Oled_Write(0x90);}
//		if(x){Oled_Write(0x09);}else{Oled_Write(0x90);}
//		if(x){Oled_Write(0x0f);}else{Oled_Write(0xf0);}	
//	}
//}
//void Blockappear(uint8_t x ,uint8_t y,uint8_t sign){//在想要的位置显示单独的方块
//	uint8_t segl=x/2,acj;
//	if(sign){
//		acj=0xff;
//		Block_P[y][x]=2;
//	}else{
//		if(x%2){
//			acj=0x0f;
//		}else{
//			acj=0xf0;
//		}
//		Block_P[y][x]=0;
//	}
//	if(x==1){
//		Oled_Point(0,44+y*4);
//		Oled_Write(0xf7&acj);
//		Oled_Write(0x97&acj);
//		Oled_Write(0x97&acj);
//		Oled_Write(0xf7&acj);	
//	}else if(x<=9){
//	if(x%2==0){
//		if(Block_P[y][x+1]){
//			Oled_Point(segl,44+y*4);
//			Oled_Write(0xff&acj);
//			Oled_Write(0x99&acj);
//			Oled_Write(0x99&acj);
//			Oled_Write(0xff&acj);	
//		}else{
//			Oled_Point(segl,44+y*4);
//			Oled_Write(0x0f&acj);
//			Oled_Write(0x09&acj);
//			Oled_Write(0x09&acj);
//			Oled_Write(0x0f&acj);	
//			}			
//	}else{
//		if(Block_P[y][x-1]){
//			Oled_Point(segl,44+y*4);
//			Oled_Write(0xff&acj);
//			Oled_Write(0x99&acj);
//			Oled_Write(0x99&acj);
//			Oled_Write(0xff&acj);	
//		}else{			
//			Oled_Point(segl,44+y*4);
//			Oled_Write(0xf0&acj);
//			Oled_Write(0x90&acj);
//			Oled_Write(0x90&acj);
//			Oled_Write(0xf0&acj);	
//			}
//		}	
//	}else{
//		Oled_Point(segl,44+y*4);
//		Oled_Write(0xef&acj);
//		Oled_Write(0xe9&acj);
//		Oled_Write(0xe9&acj);
//		Oled_Write(0xef&acj);
//	}
//	
//}
//void Teappear(uint8_t acj){//俄罗斯方块显示
//	uint8_t i;
//	if(Blockkind==0||Blockkind==1||Blockkind==2||Blockkind==4){orgin.x=Teblock[Blockkind][1]%2+move.x+1;orgin.y=Teblock[Blockkind][1]/2+move.y;}
//	if(Blockkind==3||Blockkind==5){orgin.x=Teblock[Blockkind][2]%2+move.x+1;orgin.y=Teblock[Blockkind][2]/2+move.y;}
//	for(i=0;i<4;i++){
//		get.x=Teblock[Blockkind][i]%2+move.x+1;
//		get.y=Teblock[Blockkind][i]/2+move.y;
//		curBk.x=(get.x -orgin.x)*cos_[spintime]-(get.y - orgin.y)*sin_[spintime]+orgin.x;
//		curBk.y=(get.x-orgin.x)*sin_[spintime]+(get.y - orgin.y)*cos_[spintime]+orgin.y;
//		Blockappear(curBk.x,curBk.y,acj);
//	}
//}
//uint8_t Blockbox(uint8_t check){//碰撞箱检测
//	uint8_t i;
//	if(check==0){for(i=0;i<4;i++){
//		get.x=Teblock[Blockkind][i]%2+move.x+1;
//		get.y=Teblock[Blockkind][i]/2+move.y;
//		curBk.x=(get.x -orgin.x)*cos_[spintime]-(get.y - orgin.y)*sin_[spintime]+orgin.x;
//		curBk.y=(get.x-orgin.x)*sin_[spintime]+(get.y - orgin.y)*cos_[spintime]+orgin.y;
//	if(Block_P[curBk.y+1][curBk.x]==1) return 0;
//	}}//地面检测
//	if(check==1){for(i=0;i<4;i++){
//		get.x=Teblock[Blockkind][i]%2+move.x+1;
//		get.y=Teblock[Blockkind][i]/2+move.y;
//		curBk.x=(get.x -orgin.x)*cos_[spintime]-(get.y - orgin.y)*sin_[spintime]+orgin.x;
//		curBk.y=(get.x-orgin.x)*sin_[spintime]+(get.y - orgin.y)*cos_[spintime]+orgin.y;
//	if(Block_P[curBk.y][curBk.x+1]==1) return 0;
//	}}//r移动检测
//	if(check==2){for(i=0;i<4;i++){
//		get.x=Teblock[Blockkind][i]%2+move.x+1;
//		get.y=Teblock[Blockkind][i]/2+move.y;
//		curBk.x=(get.x -orgin.x)*cos_[spintime]-(get.y - orgin.y)*sin_[spintime]+orgin.x;
//		curBk.y=(get.x-orgin.x)*sin_[spintime]+(get.y - orgin.y)*cos_[spintime]+orgin.y;
//	if(Block_P[curBk.y][curBk.x-1]==1) return 0;
//	}}//l移动检测
//	return 1;
//}
//void Fallen(){//快速下落
//	uint8_t i,j,miny=20,dy;
//	for(i=0;i<4;i++){
//		get.x=Teblock[Blockkind][i]%2+move.x+1;
//		get.y=Teblock[Blockkind][i]/2+move.y;
//		curBk.x=(get.x -orgin.x)*cos_[spintime]-(get.y - orgin.y)*sin_[spintime]+orgin.x;
//		curBk.y=(get.x-orgin.x)*sin_[spintime]+(get.y - orgin.y)*cos_[spintime]+orgin.y;
//		j=curBk.y;
//		while(Block_P[j][curBk.x]!=1){
//			j++;
//		}
//		j--;
//		dy=j-curBk.y;
//		if(miny>dy){miny=dy;}
//	}
//	move.y=move.y+miny;
//}
//void spin(){//方块的旋转
//	spintime++;
//	switch (Blockkind) {
//	case 0:
//		if (spintime == 2)spintime = 0;
//		break;
//	case 1:
//		if (spintime == 2)spintime = 0;
//		break;
//	case 2:
//		if (spintime == 2)spintime = 0;
//		break;
//	case 3:
//		if (spintime == 4)spintime = 0;
//		break;
//	case 4:
//		if (spintime == 4)spintime = 0;
//		break;
//	case 5:
//		if (spintime == 4)spintime = 0;
//		break;
//	case 6:
//		spintime = 0;
//		break;
//	}
//}
//uint8_t Spinchect(){//旋转检查
//	uint8_t i,cx,cy,tx,ty,stime=spintime;
//	stime++;
//	switch (Blockkind) {
//	case 0:
//		if (stime == 2)stime = 0;
//		break;
//	case 1:
//		if (stime == 2)stime = 0;
//		break;
//	case 2:
//		if (stime == 2)stime = 0;
//		break;
//	case 3:
//		if (stime == 4)stime = 0;
//		break;
//	case 4:
//		if (stime == 4)stime = 0;
//		break;
//	case 5:
//		if (stime == 4)stime = 0;
//		break;
//	case 6:
//		stime = 0;
//		break;
//	}
//	for(i=0;i<4;i++){
//		tx=Teblock[Blockkind][i]%2+move.x+1;
//		ty=Teblock[Blockkind][i]/2+move.y;
//		cx=(tx-orgin.x)*cos_[stime]-(ty - orgin.y)*sin_[stime]+orgin.x;
//		cy=(tx-orgin.x)*sin_[stime]+(ty - orgin.y)*cos_[stime]+orgin.y;
//		if(Block_P[cy][cx]==1){return 0;}
//	}
//	return 1;
//}
//void TeBmove(){//方块的移动
//	uint8_t KeyNum,Blue;
//	
//	KeyNum=T_GetKey();
//	if(KeyNum==2||Blue=='a'){Blue=0;if(Blockbox(2)){Teappear(0);move.x--;Teappear(1);}}//left
//	if(KeyNum==1||Blue=='d'){Blue=0;if(Blockbox(1)){Teappear(0);move.x++;Teappear(1);}}//right
//	if(KeyNum==4||Blue=='s'){Blue=0;if(Blockbox(0)){Teappear(0);Fallen();Teappear(1);}}//down
//	if(KeyNum==3||Blue=='w'){Blue=0;if(Spinchect()){Teappear(0);spin();Teappear(1);}}//up
//}
//void Bk_drop(uint8_t p){//方块的自动下落
//	uint8_t i,j;
//	for(i=p-1;i>0;i--){
//		for(j=1;j<11;j++){
//			if(Block_P[i][j]==1){
//				Blockappear(j,i,0);
//				Block_P[i][j]=0;
//				Blockappear(j,i+1,1);
//				Block_P[i+1][j]=1;
//			}
//		}
//	}
//}
//void numappear(){//得分显示
//	
//	uint8_t thu,hund,ten,nb,i;
//	nb=score%10;
//	ten=(score%100-nb)/10;
//	thu=score/1000;
//	hund=score/100-thu*10;
//	for(i=0;i<16;i++){
//		Oled_Point(0,i+7);
//		Oled_Write(number[thu][i]);
//		Oled_Point(1,i+7);
//		Oled_Write(number[hund][i]);
//		Oled_Point(2,i+7);
//		Oled_Write(number[ten][i]);
//		Oled_Point(3,i+7);
//		Oled_Write(number[nb][i]);
//	}
//}
//void BK_Row_Clear(){//消除行加计算得分
//	uint8_t i,j,k=0,rsum=0,c[4]={50,50,50,50};
//	for(i=0;i<20;i++){
//		for(j=1;j<11;j++){
//			if(Block_P[i][j]==1){
//				rsum++;
//			}
//		}
//		if(rsum==10){
//			c[k]=i;
//			k++;
//		}
//		rsum=0;
//	}
//	if(c[0]!=50){for(i=1;i<11;i++){Blockappear(i,c[0],0);}Bk_drop(c[0]);c[0]=50;score+=1;numappear();}
//	if(c[1]!=50){for(i=1;i<11;i++){Blockappear(i,c[1],0);}Bk_drop(c[1]);c[1]=50;score+=2;numappear();}
//	if(c[2]!=50){for(i=1;i<11;i++){Blockappear(i,c[2],0);}Bk_drop(c[2]);c[2]=50;score+=3;numappear();}
//	if(c[3]!=50){for(i=1;i<11;i++){Blockappear(i,c[3],0);}Bk_drop(c[3]);c[3]=50;score+=4;numappear();}
//	k=0;
//}
//uint8_t Check_Lose(){//检擦方块溢出
//	uint8_t i,tx,ty;
//	for(i=0;i<4;i++){
//		tx=Teblock[Blockkind][i]%2+move.x+1;
//		ty=Teblock[Blockkind][i]/2+move.y;
//		if(Block_P[ty][tx]==1){return 1;}
//	}
//	return 0;
//}
//void Start_Game(void){
//	UI_Init();
//	uint8_t KeyNum;
//	uint32_t i;
//	Game_Init();
//	nextblock=rand%7;//0~6控制方块种类
//	nextappear();
//	Blockkind=rand%7;
//	Teappear(1);
//	while(1){
//		KeyNum=Key_GetNum();
//		if(KeyNum == 5){
//			break;
//		}
//		TeBmove();
//		if(count>=500){
//			count=0;
//			if(Blockbox(0)){Teappear(0);move.y++;Teappear(1);}
//			else{
//				for(i=0;i<4;i++){
//					get.x=Teblock[Blockkind][i]%2+move.x+1;
//					get.y=Teblock[Blockkind][i]/2+move.y;
//					curBk.x=(get.x -orgin.x)*cos_[spintime]-(get.y - orgin.y)*sin_[spintime]+orgin.x;
//					curBk.y=(get.x-orgin.x)*sin_[spintime]+(get.y - orgin.y)*cos_[spintime]+orgin.y;//旋转后的坐标
//					Block_P[curBk.y][curBk.x]=1;
//				}
//				BK_Row_Clear();
//				Blockkind=nextblock;
//				nextblock=rand%7;
//				nextappear();
//				spintime=0;
//				move.x=4;
//				move.y=0;
//				if(Check_Lose()){				//检验是否溢出方块
//					score=0;
//					break;
//				}
//				Teappear(1);
//			}
//		}
//	}
//}
