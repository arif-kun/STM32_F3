#include "stm32f3xx.h"                  // Device header
#include "CAN.h"
#include <stdint.h>

CAN_msg       CAN_TxMsg;                          // CAN messge for sending
CAN_msg       CAN_RxMsg;                          // CAN message for receiving                                

unsigned int  CAN_TxRdy = 0;                      // CAN HW ready to transmit a message
unsigned int  CAN_RxRdy = 0;                      // CAN HW received a message

void CAN_setup()
{
	RCC->APB1ENR |= RCC_APB1ENR_CANEN;
	RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	
	GPIOD->MODER &=~(3<<0 | 3<<2);
	GPIOD->MODER |= 2<<0 | 2<<2;			//PD0 and PD1 AFs
	
	GPIOD->AFR[0] &=~ (0xF<<0 | 0xF<<4);
	GPIOD->AFR[0] |= 0x7<<0 | 0x7<<4;					//AF7. PD0 - CAN_RX, PD1 - CAN_TX

	NVIC_EnableIRQ(USB_HP_CAN_TX_IRQn);
	NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
	
	CAN->MCR = (CAN_MCR_NART | CAN_MCR_INRQ);       // init mode, disable auto. retransmission
                                                  // Note: only FIFO 0, transmit mailbox 0 used
  CAN->IER = (CAN_IER_FMPIE0 | CAN_IER_TMEIE);    // FIFO 0 msg pending, Transmit mbx empty
	
	CAN->BTR &= ~(((0x03) << 24) | ((0x07) << 20) | ((0x0F) << 16) | (0x1FF));
	CAN->BTR |= 0x001C0000;			//500 kbps, 87.5% sampling point
}

//*********************************************************************************************************************

void CAN_start(void)
{
	CAN->MCR &=~ (CAN_MCR_INRQ);			//leave initialization mode
	while(CAN->MSR & CAN_MCR_INRQ);
}

//*********************************************************************************************************************

void CAN_testmode(unsigned int testmode)
{
	CAN->BTR &=~(CAN_BTR_SILM | CAN_BTR_LBKM);		//self test mode loop back
	CAN->BTR |= (testmode & (CAN_BTR_SILM | CAN_BTR_LBKM));
}

//*********************************************************************************************************************

void CAN_waitReady (void)  {

  while ((CAN->TSR & CAN_TSR_TME0) == 0);         // Transmit mailbox 0 is empty
  CAN_TxRdy = 1;
 
}

//**************************************************************************************************************************
void CAN_wrMsg(CAN_msg *msg)
{
	CAN->sTxMailBox[0].TIR = 0;		//reset identifier register before sending next msg
	//dot operator on TIR since it's a direct member of sTxMailbox, while sTxMailBox is referenced by CAN pointer
	
	
	//check format of message(standard or extended)
	if(msg->format == STD_FORMAT) 
	{
		CAN->sTxMailBox[0].TIR |= (uint32_t) msg->id<<21 ;		//set ID as STD and push msg id to STDID bit field
	}
	else{
		CAN->sTxMailBox[0].TIR |= (uint32_t) msg->id<<3 | (1<<2);			//set ID as EXT and push msg id to EXTID bit field
	}
	
	
	//check msg being sent is data or remote frame
	
	if(msg->type == DATA_FRAME){
		CAN->sTxMailBox[0].TIR &=~(1<<1);	//reset RTR bit implying Data bit being sent
	}
	else{
		CAN->sTxMailBox[0].TIR |= 1<<1;
	}
	
	//store message into TDLR and TDHR
	CAN->sTxMailBox[0].TDLR = (((unsigned int)msg->data[3] << 24) | 
                             ((unsigned int)msg->data[2] << 16) |
                             ((unsigned int)msg->data[1] <<  8) | 
                             ((unsigned int)msg->data[0])        );
	
	CAN->sTxMailBox[0].TDHR = (((unsigned int)msg->data[7] << 24) | 
                             ((unsigned int)msg->data[6] << 16) |
                             ((unsigned int)msg->data[5] <<  8) |
                             ((unsigned int)msg->data[4])        );
	
	
	//set Data length in TDTR (transmit time stamp register)
	CAN->sTxMailBox[0].TDTR &=~(CAN_TDT0R_DLC);	//reset data length to 0
	
	CAN->sTxMailBox[0].TDTR |= msg->len;
	
	CAN->IER |= CAN_IER_TMEIE;			//transmit mailbox interrupt enable
	
	CAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;			//request transmission for mailbox 0
	
	
}

//************************************************************************************************************************
void CAN_rdMsg(CAN_msg *msg)
{
	//check if received message has standard identifier
	if(!(CAN->sFIFOMailBox[0].RIR & 1<<2))
	{
		msg->format = STD_FORMAT;
		msg->id = (uint32_t) CAN->sFIFOMailBox[0].RIR>>21 & (uint32_t)0x7FF;			//11 bit identifier
	}
	else{
		msg->format = EXT_FORMAT;
		msg->id = (uint32_t) CAN->sFIFOMailBox[0].RIR>>3 & (uint32_t)0x3FFFF;		//29 bit identifier
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
	msg->len = (uint32_t)0xF & CAN->sFIFOMailBox[0].RDTR ;
	
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

//*********************************************************************************************************************




void CAN_Filter(unsigned int id, unsigned char format)
{
	static unsigned int filter_id = 0;
	unsigned int msg_id = 0;
	
	if(filter_id > 13) return;			//if filter memory is full, return
	
	if(format == STD_FORMAT)
	{
		msg_id |= (unsigned int) (id<<21);
		msg_id &=~ (unsigned int)(1<<2);
	}
	else
	{
		msg_id |= (unsigned int) (id<<3);
		msg_id |= (unsigned int) (1<<2);
	}
	
	CAN->FMR |= CAN_FMR_FINIT;		//initialization mode for filter
	CAN->FA1R &=~ (1<<filter_id);		//deactive filter through filter activation register
	
	CAN->FS1R |= 1<<filter_id;		//32 bit scale config
	CAN->FM1R |= 1<<filter_id;		//identifier list mode
	
	CAN->sFilterRegister[filter_id].FR1 = msg_id;
	CAN->sFilterRegister[filter_id].FR2 = msg_id;
	
	CAN->FFA1R &=~(1<<filter_id);
	CAN->FA1R |= 1<<filter_id;
	
	CAN->FMR &=~ (CAN_FMR_FINIT);
	
	filter_id+=1;
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