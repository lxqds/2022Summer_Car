#ifndef __MAIN_H
#define __MAIN_H

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

typedef struct Struct_Int
{
	int Z;
	int a;
	int b;
	int c;
}Struct_Int;


typedef struct TwoWay_Data
{
	uint8_t Data_Hand1;
	uint8_t State;
	uint8_t State2;
	uint8_t State3;
	Float_Byte D;
	uint8_t Data_Tail;
}Car_Data;

typedef union Uart_Data
{
	unsigned char Byte[8];
	Car_Data Car_Data1;
}Uart_Car_Data;
#endif
