#include <htc.h>
#include "PortDef.h"
#include "include_CC2500.h"
#include "CC2500.h"
#include "Delay.c"
#include "General.c"
#include "Serial_Route.c"
#include "SPI_Functions.c"

void TR433_Check(void)
{
	uchar Read_Bur[5];
	const uchar PartNum = 128;
	const uchar VersNum = 3;
	
	Read_Bur[0] = SPI_ReadReg(0XF0);
	Read_Bur[1] = SPI_ReadReg(0XF1);
}
void MCU_Init(void)
{
	
	ADCON1 = 0X06;	//Make PortA & PortE as Digital I/O
	TRISA  = 0X00;	//Make PortA as Digital Output
	TRISB  = 0X00;	//Make PortB as Digital Output
	TRISC  = 0XB0; //Make PortC lower nibble as Digital Output and   OthersDigital Input                 
	TRISD  = 0x04; 		//Configure  SOMI as Input and Others are Output
	TRISE  = 0X00;	//Make PortE as Digital Output
	
	Wheel_Motors_Off();
	BUZZER = BUZZER_OF;
	
	Delay_10us(10);
	
	Serial_Init();
	TR433_Init();
	TR433_Check();	
	
}


void main(void)
{
	__CONFIG(0X193A);
	uint RFID_Val = 0;
	uchar TxBuff[8];
	uchar Comp_SRB = 0;
	uchar Inc = 0;
	
	MCU_Init();
	while(1)
	{		
		if(Rx_End_Flag == 1)
		{
			RFID_Val = Char2Int(G_RFID_TTL_Val);
			Int2Char(RFID_Val,&TxBuff[2]);
			Comp_SRB = CompareSelf(RFID_Val);
			Rx_End_Flag = 0;
			if(SENSOR == SEN_DET)
			{	
				Wheel_Motors_Off();
				Delay_10us(20);
				"BUZZER ON");
				Delay_10us(50);
				BUZZER = BUZZER_ON;
				Delay_10us(10);
			}
			else
			{
				BUZZER = BUZZER_OF;
				Delay_10us(10);
				switch(Comp_SRB)
				{
					case 0:
						TxBuff[0] = 0X04;
						TxBuff[1] = 0X01;
						TxBuff[2] = 'N';
						TxBuff[3] = '1';
						TxBuff[4] = 0X00;
						RFSendPacket(TxBuff,0X05);
						Forward_Movement();
						break;
					case 1:
						TxBuff[0] = 0X04;
						TxBuff[1] = 0X01;
						TxBuff[2] = 'N';
						TxBuff[3] = '2';
						TxBuff[4] = 0X00;
						RFSendPacket(TxBuff,0X05);
						RightTurn_Movement();
						Delay_10us(100);
						Wheel_Motors_Off();
						Forward_Movement();
						Delay_10us(200);
						break;
					case 2:
						TxBuff[0] = 0X04;
						TxBuff[1] = 0X01;
						TxBuff[2] = 'N';
						TxBuff[3] = '3';
						TxBuff[4] = 0X00;
						RFSendPacket(TxBuff,0X05);
						RightTurn_Movement();
						Delay_10us(100);
						Wheel_Motors_Off();
						Forward_Movement();
						Delay_10us(200);
						break;
					case 3:
						TxBuff[0] = 0X04;
						TxBuff[1] = 0X01;
						TxBuff[2] = 'N';
						TxBuff[3] = '4';
						TxBuff[4] = 0X00;
						RFSendPacket(TxBuff,0X05);
						Wheel_Motors_Off();
						Delay_10us(10);
						break;
					default:		
						Delay_10us(100);
						break;
				}
			}
		}	
	}
}