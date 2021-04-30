#include "stm32f3xx.h"                  // Device header
#include "CAN.h"

//PA11 - CAN_RX
//PA12 - CAN_TX

void delayms(int count);

int main()
{
	unsigned int x=0;
	unsigned int rec_val;
	CAN_init();

	CAN_TxMsg.id = 33;
	for (int i = 0; i < 8; i++) CAN_TxMsg.data[i] = 0;
  CAN_TxMsg.len = 1;
  CAN_TxMsg.format = STD_FORMAT;
  CAN_TxMsg.type = DATA_FRAME;
	
	while (1)
	{                                     // Loop forever
    if (CAN_TxRdy) 
		{
      CAN_TxRdy = 0;
      CAN_TxMsg.data[0] = x++;             // data[0] field 
      CAN_wrMsg (&CAN_TxMsg);                     // transmit message
			if(x>8) x=0;
		}
		
		delayms(1000);
		
		if (CAN_RxRdy) 
		{
      CAN_RxRdy = 0;
			rec_val = CAN_RxMsg.data[0];
    }
	}
	
}

void CAN_init()
{
	CAN_setup();
	
	CAN_Filter(29,STD_FORMAT);
	
	//CAN_testmode(CAN_BTR_SILM | CAN_BTR_LBKM);      // Loopback, Silent Mode (self-test)

  CAN_start ();                                   // leave init mode

  CAN_waitReady ();                               // wait til mbx is empty
	
}

void delayms(int count)
{
	int i;
	SysTick->LOAD = 8000-1;		//7999 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & 0x10000)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
}
