/*
 * ATmega16 Interface with MPU-6050
 * http://www.electronicwings.com
 *
 */ 


#define F_CPU 16000000UL									/* Define CPU clock Frequency e.g. here its 16MHz */
#include <avr/io.h>										/* Include AVR std. library file */
#include <util/delay.h>									/* Include delay header file */
#include <inttypes.h>									/* Include integer type header file */
#include <stdlib.h>										/* Include standard library file */
#include <stdio.h>										/* Include standard library file */
#include "lcd.h"
#include "MPU6050_res_define.h"							/* Include MPU6050 register define file */
#include "I2C_Master_H_file.h"							/* Include I2C Master header file */
#include "USART_RS232_H_file.h"							/* Include USART header file */


float Acc_x,Acc_y,Acc_z,Temperature,Gyro_x,Gyro_y,Gyro_z;

void MPU6050_Init()										/* Gyro initialization function */
{
	_delay_ms(150);										/* Power up time >100ms */
	I2C_Start_Wait(0xD0);								/* Start with device write address */
	I2C_Write(SMPLRT_DIV);								/* Write to sample rate register */
	I2C_Write(0x07);									/* 1KHz sample rate */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(PWR_MGMT_1);								/* Write to power management register */
	I2C_Write(0x01);									/* X axis gyroscope reference frequency */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(CONFIG);									/* Write to Configuration register */
	I2C_Write(0x00);									/* Fs = 8KHz */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(GYRO_CONFIG);								/* Write to Gyro configuration register */
	I2C_Write(0x18);									/* Full scale range +/- 2000 degree/C */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(INT_ENABLE);								/* Write to interrupt enable register */
	I2C_Write(0x01);
	I2C_Stop();
}

void MPU_Start_Loc()
{
	I2C_Start_Wait(0xD0);								/* I2C start with device write address */
	I2C_Write(ACCEL_XOUT_H);							/* Write start location address from where to read */ 
	I2C_Repeated_Start(0xD1);							/* I2C start with device read address */
}

void Read_RawValue()
{
	MPU_Start_Loc();									/* Read Gyro values */
	Acc_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Temperature = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Nack());
	I2C_Stop();
}

void lcd_print_acel_grau(float Xa,float Ya,float Za,float Xg,float Yg,float Zg){
	char buffer[16],nums[6][4];
	//printa aceleracoes e angulo 
	dtostrf( Xa, 3, 1, nums[0]);nums[0][3]='\0';
	dtostrf( Ya, 3, 1, nums[1]);nums[1][3]='\0';
	dtostrf( Za, 3, 1, nums[2]);nums[2][3]='\0';
	dtostrf( Xg, 3, 1, nums[3]);nums[3][3]='\0';
	dtostrf( Yg, 3, 1, nums[4]);nums[4][3]='\0';
	dtostrf( Zg, 3, 1, nums[5]);nums[5][3]='\0';
	sprintf(buffer,"%s%c %s%c %s%c",nums[0],0x67,nums[1],0x67,nums[2],0x67);
	lcd_puts(buffer);
	sprintf(buffer,"%s %s %s",nums[3],nums[4],nums[5]);
	lcd_gotoxy(0,1);
	lcd_puts(buffer);
}

void lcd_print_rpy(float roll, float pitch,float yaw){
	//printa roll pitch e yaw roll-> eixo x, yaw->z , pitch->y
	char buffer[16], nums[6][5];
	dtostrf( roll, 4, 1, nums[0]);nums[0][4]='\0';
	dtostrf( pitch, 4, 1, nums[1]);nums[1][4]='\0';
	dtostrf( yaw, 4, 1, nums[2]);nums[2][4]='\0';
	sprintf(buffer,"R:%sP:%s ",nums[0],nums[1]);
	lcd_puts(buffer);
	sprintf(buffer,"YAW:%s ",nums[2]);
	lcd_gotoxy(0,1);
	lcd_puts(buffer);
}
void lcd_print_acel(float Xa, float Ya,float Za){
	//printa aceleração só
	char buffer[16], nums[6][5];
	dtostrf( Xa, 4, 1, nums[0]);nums[0][4]='\0';
	dtostrf( Ya, 4, 1, nums[1]);nums[1][4]='\0';
	dtostrf( Za, 4, 1, nums[2]);nums[2][4]='\0';
	sprintf(buffer,"Xa%s Ya%s",nums[0],nums[1]);
	lcd_puts(buffer);
	sprintf(buffer,"Za%s",nums[2]);
	lcd_gotoxy(0,1);
	lcd_puts(buffer);
}


float rad_2_deg(float rad){
	float deg=rad*360/(2*M_PI);
	while(deg>360){
		deg-=360;
	}
	while(deg<0){
		deg+=360;
	}
	return deg;
}


int main()
{
	char buffer[16], nums[6][5];
	float Xa=0,Ya=0,Za=0,t,roll=0,pitch=0,yaw=0;
	float Xg=0,Yg=0,Zg=0,Xgt=0,Ygt=0,Zgt=0; //_g graus _gt graus/s
	int contador=0, limite=3;
	I2C_Init();											/* Initialize I2C */
	MPU6050_Init();										/* Initialize MPU6050 */
	USART_Init(9600);									/* Initialize USART with 9600 baud rate */
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	_delay_ms(50);
	while(1)
	{
		Read_RawValue();
		lcd_clrscr();
		Xa = Acc_x/16384.0;								/* Divide raw value by sensitivity scale factor to get real values */
		Ya = Acc_y/16384.0;
		Za = Acc_z/16384.0;	
		//t = (Temperature/340.00)+36.53;					/* Convert temperature in °/c using formula */

		
		//gyro dividido por fator de sensitividade 
		Xgt=Gyro_x/16.4;
		Ygt=Gyro_y/16.4;
		Zgt=Gyro_z/16.4;
		lcd_print_rpy(Xgt,Ygt,Zgt);
		_delay_ms(500);
	}
}
