/*
 * comm_stm4.h
 *
 *  Created on: 2017-5-19
 *      Author: firefly
 */

#ifndef COMM_STM4_H_
#define COMM_STM4_H_
//#include "Dlist.h"
#include <stdbool.h>
extern bool isCheckSendTimer(char comm);
extern char *obtain_car_status_from_buffer();
extern void* thread_uart(void* arg) ;


#endif /* COMM_STM4_H_ */
