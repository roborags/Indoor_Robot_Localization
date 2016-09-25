#include "htc.h" 
#include "include_CC2500.h"
#include "CC2500.h"
#include "SPI_Functions.c"

char Tx_Dat[15] = "0000000";
bit Flag_Tx = 0;
bit Rx_Start_Flag = 0;
uchar Rx_Inc = 0;
//---------------------------------------
// Serial Initialization function
//---------------------------------------
void interrupt ISR_Route(void)
{
uchar Dummy;
	if(RCIF)	
	{
		Dummy = RCREG;		//Store from Serial Rc Buffer
		if(Rx_Start_Flag == 1)
		{
			if(Dummy != '#')
			{
				Tx_Dat[Rx_Inc++] = Dummy;	
			}
			else
			{
				Tx_Dat[Rx_Inc++] = 0;
				Tx_Dat[9] = 0;
				Rx_Start_Flag = 0;
				Flag_Tx = 1;
				Rx_Inc = 0;
			}
		}
		else
		{
			if(Dummy == '$')
			{
				Rx_Start_Flag = 1;
				Flag_Tx = 0;
				Rx_Inc = 2;
			}
		}
	}
}

void Serial_Init(void)
{
	TRISC 	= (TRISC & 0XBF) | 0X80;	// Make RC6 as Output(TXD) and RC7 as Input(RXD)
	BRGH 	= 1;		// high speed serial mode
	SPBRG 	= 25;		// Set 9600 baud for 4 MHz oscillator
	SYNC 	= 0;		// Clear SYNC bit -> Set ASYNC Mode
	SPEN 	= 1;		// Set serial port enable
	TX9 	= 0;		          // 8-bit transmissions
	TXEN 	= 1;	        // Enable transmission
	RX9 	= 0;			// 8-bit receptions
	CREN 	= 1;		// Enable reception	
	RCIF 	= 0;		// Clear reception	Flag
	RCIE 	= 1;		// Disable reception Interrupt
	PEIE    = 1;		// Disable Peripheral Interrupt
	GIE		= 1;		// Disable Global Interrupt
}

void Txt_String(const uchar *Tx_Str)
{
uchar Inc1 = 0,Inc2 = 0;;
	do{
		while(!TXIF);
		TXREG = Tx_Str[Inc1];
		for(Inc2=0;Inc2<10;Inc2++);
	}while(Tx_Str[++Inc1] != 0);
}

void Calc_TR433(uchar Read_Val)
{
		uchar Vers[5];
		Vers[2] = ((Read_Val % 10) + 0x30);
		Read_Val /= 10;
		Vers[1] = ((Read_Val % 10) + 0x30);
		Read_Val /= 10;
		Vers[0] = ((Read_Val % 10) + 0x30);
		Vers[3] = 0;
		
		Txt_String(Vers);		
}

void TR433_Check(void)
{
uchar Read_Bur[3];
			Txt_String("\r\nRead TR433 Module's Version and Part Number");
			Txt_String("\r\nExpected Value is");
			Txt_String("\r\nPart Number    = 128");
			Txt_String("\r\nVersion Number = 003");

			Read_Bur[0] = SPI_ReadReg(0XF0);
			Read_Bur[1] = SPI_ReadReg(0XF1);			
			Txt_String("\r\n\r\nRead Value is");
			Delay(100);
			Txt_String("\r\nPart Number    = ");
			Calc_TR433(Read_Bur[0]);
			Txt_String("\r\nVersion Number = ");
			Calc_TR433(Read_Bur[1]);		
}

void Delay(unsigned int uc_Limit)
{
	unsigned int Del_ui_Count1,Del_ui_Count2;
	for(Del_ui_Count1 = 0;Del_ui_Count1 < uc_Limit; Del_ui_Count1++)
	for(Del_ui_Count2 = 0;Del_ui_Count2 < 1000; Del_ui_Count2++);
}

uint Char2Int(uchar *Vers)
{
char Inc = 0;
uint Read_Val = 0;
	for(Inc = 0;Vers[Inc] != 0;Inc++)
	{
		Read_Val = ((Read_Val * 10) + (Vers[Inc] - 0X30));
	}
return Read_Val;			
}


void main()
{
	__CONFIG(0X193A);
	uchar len = 60,Inc2;
	uchar rfstate;
	uint BookID;
 	Serial_Init();
	Txt_String("\f\r\nWELCOME TO CC2500 WIRELESS WORLD\r\n");
	TR433_Init();
	Txt_String("\r\n");
	TR433_Check();
	Delay(10);	
	while(1)	
	{
		rfstate=SPI_ReadStatus(TI_CCxxx0_MARCSTATE);	//check sleep state and other
		if((rfstate& 0x1f)==0x00)
		{ 	
			PowerUpReset();
			writeRFSettings();	 			
		}
		len = 60;
		if(RFReceivePacket(&Tx_Dat[0], &len))
		{
			if(Tx_Dat[1] == 'N' && Tx_Dat[2] == '1')
			{
				Txt_String("\r\nPoint 1");
			}
			else if(Tx_Dat[1] == 'N' && Tx_Dat[2] == '2')
			{
				Txt_String("\r\nPoint 2");
			}
			else if(Tx_Dat[1] == 'N' && Tx_Dat[2] == '3')
			{
				Txt_String("\r\nPoint 3");
			}
			else if(Tx_Dat[1] == 'N' && Tx_Dat[2] == '4')
			{
				Txt_String("\r\nPoint 4");
			}
			Delay(5);
		}	
	}
}
