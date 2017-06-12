/*
 * uart.h
 *
 *  Created on: May 14, 2017
 *      Author: firefly
 */

#ifndef UART_H_
#define UART_H_

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <semaphore.h>
#include <sys/ioctl.h>

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);
int open_port(int fd,int comport);
int uart_recv( char *rcv_buf,int data_len);
int uart_send( char *send_buf,int data_len);
extern int SetUartSate(void);
#endif /* UART_H_ */
