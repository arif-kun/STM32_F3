#include "stm32f3xx.h"                  // Device header
#include "CAN.h"

CAN_msg       CAN_TxMsg;                          // CAN messge for sending
CAN_msg       CAN_RxMsg;                          // CAN message for receiving                                

unsigned int  CAN_TxRdy = 0;                      // CAN HW ready to transmit a message
unsigned int  CAN_RxRdy = 0;                      // CAN HW received a message

void CAN_init()
{
	RCC->APB1ENR |= RCC_APB1ENR_CANEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER &=~(3<<22 | 3<<24);
	GPIOA->MODER |= 2<<22 | 2<<24;
	
	GPIOA->AFR[1] &=~ (0xF<<12 | 0xF<<16);
	GPIOA->AFR[1] |= 9<<12 | 9<<16;					//AF9 = CAN tx and rx

	NVIC_EnableIRQ(USB_HP_CAN_TX_IRQn);
	NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
	
	CAN->MCR = (CAN_MCR_NART | CAN_MCR_INRQ);       // init mode, disable auto. retransmission
                                                  // Note: only FIFO 0, transmit mailbox 0 used
  CAN->IER = (CAN_IER_FMPIE0 | CAN_IER_TMEIE);    // FIFO 0 msg pending, Transmit mbx empty
	
	CAN->BTR = 0x001C0000;			//500 kbps, 87.5% sampling point
}

void CAN_start(void)
{
	CAN->MCR &=~ (CAN_MCR_INRQ);			//leave initialization mode
	while(CAN->MSR & CAN_MCR_INRQ);
}


void CAN_testmode(unsigned int testmode)
{
	CAN->BTR &=~(CAN_BTR_SILM | CAN_BTR_LBKM);		//self test mode loop back
	CAN->BTR |= (testmode & (CAN_BTR_SILM | CAN_BTR_LBKM));
}

void CAN_waitReady (void)  {

  while ((CAN->TSR & CAN_TSR_TME0) == 0);         // Transmit mailbox 0 is empty
  CAN_TxRdy = 1;
 
}


void CAN_wrMsg(CAN_msg *msg)
{
	CAN->sTxMailBox[0].TIR = 0;		//reset identifier register before sending next msg
	//dot operator on TIR since it's a direct member of sTxMailbox, while sTxMailBox is referenced by CAN pointer
	
	
	//check format of message(standard or extended)
	if(msg->format == STD_FORMAT) 
	{
		CAN->sTxMailBox[0].TIR |= msg->id<<21;
	}
	else{
		CAN->sTxMailBox[0].TIR |= msg->id<<3;
	}
	
	
	//check msg being sent is data or remote frame
	
	if(msg->type == DATA_FRAME){
		CAN->sTxMailBox[0].TIR &=~(CAN_TI0R_RTR);	//reset RTR bit
	}
	else{
		CAN->sTxMailBox[0].TIR |= CAN_TI0R_RTR;
	}
	
	//store message into TDLR and TDHR
	CAN->sTxMailBox[0].TDLR = msg->data[0] | msg->data[1]<<8 | msg->data[2]<<16 | msg->data[3]<<24;
	
	CAN->sTxMailBox[0].TDHR = msg->data[4] | msg->data[5]<<8 | msg->data[6]<<16 | msg->data[7]<<24;
	
	//set Data length in TDTR (transmit time stamp register)
	CAN->sTxMailBox[0].TDTR &=~(CAN_TDT0R_DLC);	//reset data length to 0
	
	CAN->sTxMailBox[0].TDTR |= msg->len;
	
	//CAN->IER |= CAN_IER_TMEIE;			//transmit mailbox interrupt enable
	
	CAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;			//request transmission for mailbox 0
	

	
}


void CAN_rdMsg(CAN_msg *msg)
{
	//check if received message has standard identifier
	if(!(CAN->sFIFOMailBox[0].RIR & CAN_RI0R_IDE))
	{
		msg->format = STD_FORMAT;
		msg->id = CAN->sFIFOMailBox[0].RIR>>21 & (uint32_t)0x7FF;			//11 bit identifier
	}
	else{
		msg->format = EXT_FORMAT;
		msg->id = CAN->sFIFOMailBox[0].RIR>>3 & (uint32_t)0x3FFFF;		//29 bit identifier
	}
	
	//check message type
	if(!(CAN->sFIFOMailBox[0].RIR & CAN_RI0R_RTR))
	{
		msg->type = DATA_FRAME;
	}
	else{
		msg->type = REMOTE_FRAME;
	}
	
	//check msg len
	msg->len = CAN->sFIFOMailBox[0].RDTR & (uint32_t)0xF;
	
	//store recieved message in msg
	msg->data[0] = (unsigned int)0x000000FF & (CAN->sFIFOMailBox[0].RDLR);
  msg->data[1] = (unsigned int)0x000000FF & (CAN->sFIFOMailBox[0].RDLR >> 8);
  msg->data[2] = (unsigned int)0x000000FF & (CAN->sFIFOMailBox[0].RDLR >> 16);
  msg->data[3] = (unsigned int)0x000000FF & (CAN->sFIFOMailBox[0].RDLR >> 24);

  msg->data[4] = (unsigned int)0x000000FF & (CAN->sFIFOMailBox[0].RDHR);
  msg->data[5] = (unsigned int)0x000000FF & (CAN->sFIFOMailBox[0].RDHR >> 8);
  msg->data[6] = (unsigned int)0x000000FF & (CAN->sFIFOMailBox[0].RDHR >> 16);
  msg->data[7] = (unsigned int)0x000000FF & (CAN->sFIFOMailBox[0].RDHR >> 24);
	
	CAN->RF0R |= CAN_RF0R_RFOM0;		//release receiver FIFO 0
}


void CAN_Filter(unsigned int id, char format, int mode)
{
	static unsigned int filter_id = 0;
	unsigned int msg_id = 0;
	
	if(filter_id > 13) return;			//if filter memory is full, return
	
	//if(format == STD_FORMAT)
	//{
	
	
	//}
	
	CAN->FMR |= CAN_FMR_FINIT;		//initialization mode for filter
	CAN->FA1R &=~ (1<<filter_id);		//deactive filter through filter activation register
	
	CAN->FS1R |= 1<<filter_id;		//32 bit scale config
	CAN->FM1R |= 1<<filter_id;		//
	
	CAN->sFilterRegister[filter_id].FR1 = msg_id;
}



void USB_HP_CAN_TX_IRQHandler (void)
{
	//check if last request for transmit mailbox has been completed
	
	if(CAN->TSR & CAN_TSR_RQCP0)
	{
		CAN->TSR |= CAN_TSR_RQCP0;		//reset request complete
		CAN->IER &=~(CAN_IER_TMEIE);		//disable TME interrupt
		
		CAN_TxRdy = 1;
		
	}
}

void USB_LP_CAN_RX0_IRQHandler(void)
{
	//check if a message is pending to be read
	if(CAN->RF0R & CAN_RF0R_FMP0)
	{
		CAN_rdMsg(&CAN_RxMsg);
		
		CAN_RxRdy = 1;				//set read ready flag 
	}
	
}