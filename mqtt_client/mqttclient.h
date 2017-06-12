/*
 * mqttclient.h
 *
 *  Created on: May 16, 2017
 *      Author: firefly
 */

#ifndef MQTTCLIENT_H_
#define MQTTCLIENT_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include "Dlist.h"
#include <stdbool.h>

extern void* thread_mqtt(void* arg);
extern pthread_mutex_t mutex;
extern bool isCheckSendTimer(char comm);
extern bool isMqttRcv100Packs(void);
extern DList *read_buffer_of_server(void);

#endif /* MQTTCLIENT_H_ */
