#ifndef PC_COMM_H
#define PC_COMM_H

void PC_Comm_init(int baudrate);

int send_char(char c);

int rec_char(void);




#endif