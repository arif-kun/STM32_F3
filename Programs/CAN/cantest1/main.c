#include "stm32f3xx.h"                  // Device header
#include "CAN.h"

//PA11 - CAN_RX
//PA12 - CAN_TX


int main()
{
	CAN_init();
	CAN_ST
	
	
	
	CAN_TxMsg.id = 33;
	for (int i = 0; i < 8; i++) CAN_TxMsg.data[i] = 0;
  CAN_TxMsg.len = 1;
  CAN_TxMsg.format = STD_FORMAT;
  CAN_TxMsg.type = DATA_FRAME;
	
}

