/*
 * commeinclude.h
 *
 *  Created on: May 26, 2017
 *      Author: firefly
 */

#ifndef COMMEINCLUDE_H_
#define COMMEINCLUDE_H_
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <math.h>
#include "mqttclient.h"
#include <stdbool.h>
#include "comm_stm4.h"
#include"mqttclient.h"
#include "MQTTAsync.h"
#include "cJSON.h"
#include "uart.h"

#define UART_COM 1
#define MQTT_COM 2

#define InitDef {"\0","\0","\0","\0","\0","\0","\0"};
#define BUFFER_SIZE_BEFORE_SEND 100
#define UART_TAIL_SIGNAL '*'
#define UART_HEAD_SIGNAL '$'
#define STRING_TAIL '\0'
extern pthread_mutex_t mutex;
enum SegCount
{
	EMPTY = 0,
	LONGITUDEFLAG = 2,
	LATITUDEFLAG = 3,
	SPEEDFLAG = 4,
	HEADFLAG = 5,
	ACCELFLAG = 6
};
typedef unsigned char uchar;
typedef unsigned int uint;
#endif /* COMMEINCLUDE_H_ */

