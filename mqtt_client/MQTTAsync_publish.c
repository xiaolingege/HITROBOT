/*******************************************************************************
 * Copyright (c) 2012, 2013 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include "commeinclude.h"

#define MQTTRCVSUCCESSFLAG 1

static SendUartStruct unjson_from_server_buffer(char *text);
static bool mqttReceiveBufferFullFlag = false;
static char* format_json_to_save(SendUartStruct data);
DList *pSendServerDList = NULL;

#if !defined(WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif



#define ADDRESS     "tcp://10.1.16.43:1883"
#define CLIENTID    "002"
#define TOPIC_SUB_MOVETO    "/IOV/Robot/001/EXEC/MoveTo"
#define TOPIC_PUB_MOVESTATUS     "/IOV/Robot/001/MoveStatus"
#define QOS         1
#define MQTT_WAIT_TIME_US 10000L
static void onSubscribe(void* context, MQTTAsync_successData* response);
static void onSubscribeFailure(void* context, MQTTAsync_failureData* response);
static int msg_arrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message);
static void connlost(void *context, char *cause);
static void onConnectFailure(void* context, MQTTAsync_failureData* response);
static void onConnectSuccess(void* context, MQTTAsync_successData* response);
static void send_msg_to_server(void);
static void subscribe(void);
/*---------------------------------*/

MQTTAsync client;
volatile MQTTAsync_token deliveredtoken;
static bool waitSubscribedFlag = false;
static bool waitConnectFlag = false;
int connStatus = 0;



int finished = 0;
#define TIMEOUT     10000L

int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    int i;
    char* payloadptr;

    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");

    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}


void connlost(void *context, char *cause)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	int rc;

	printf("\nConnection lost\n");
	connStatus=0;
}


void onDisconnect(void* context, MQTTAsync_successData* response)
{
	printf("Successful disconnection\n");
	finished = 1;
}


void onSend(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
	int rc;

	printf("Message with token value %d delivery confirmed\n", response->token);

	opts.onSuccess = onDisconnect;
	opts.context = client;

	if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start sendMessage, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
}


void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	printf("Connect failed, rc %d\n", response ? response->code : 0);
	finished = 1;
	connStatus=0;
}

void onSubscribe(void* context, MQTTAsync_successData* response)
{
	printf("Subscribe succeeded\n");

	connStatus = 1;
}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
	printf("Subscribe failed, rc %d\n", response ? response->code : 0);
	finished = 1;
	connStatus = 0;
}

void onConnect(void* context, MQTTAsync_successData* response)
{

	printf("Successful connection\n");



	deliveredtoken = 0;
	subscribe();
}



void* thread_mqtt(void* arg){
		MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
		MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
		MQTTAsync_token token;
		int rc;
		MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
		MQTTAsync_setCallbacks(client, NULL, connlost, msg_arrvd, NULL);
		conn_opts.keepAliveInterval = 20;
		conn_opts.cleansession = 1;
		conn_opts.onSuccess = onConnect;
		conn_opts.onFailure = onConnectFailure;
		conn_opts.context = client;
		while(1)
			{
				if(connStatus == 0)
				{
					if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
						{
							printf("Failed to start connect, return code %d\n", rc);
							exit(EXIT_FAILURE);
						}
					else
					{

						printf("start connect success\n");
						usleep(1000000);
					}
				}
				else
				{
					if (isCheckSendTimer(MQTT_COM)) {
							send_msg_to_server();
					}
					//usleep(MQTT_WAIT_TIME_US);
					usleep(10000);
				//	printf("communicating----\n");
				}

			}

		while (!finished)
			#if defined(WIN32)
				Sleep(100);
			#else
				usleep(10000L);
			#endif

		MQTTAsync_destroy(&client);
}



/*


void* thread_mqtt(void* arg)
{


	while (1) {
		static struct timeval tpstart;
		double timeuse;
		double timelost = 0;
		if(connStatus == 0)
		{
			if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
				{
					printf("Failed to start connect, return code %d\n", rc);
					exit(EXIT_FAILURE);
				}
			else
			{
				printf("start connect success\n");
				usleep(1000000);
			}
		}
		else if (isCheckSendTimer(MQTT_COM)) {
			send_msg_to_server();
			timelost = (tpstart.tv_sec * 1000000 + tpstart.tv_usec) / 1000.0;
			gettimeofday(&tpstart, NULL);
			timeuse = (tpstart.tv_sec * 1000000 + tpstart.tv_usec) / 1000.0 - timelost;
			//			printf("*************process time is %lf ms\n", timeuse);
		}
		usleep(MQTT_WAIT_TIME_US);
	}
	MQTTAsync_destroy(&client);
	return (void *)0;
}


*/



void subscribe(void)
{
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	int rc;
	printf("Successful connection\n");
	printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
		"Press Q<Enter> to quit\n\n", TOPIC_SUB_MOVETO, CLIENTID, QOS);
	opts.onSuccess = onSubscribe;
	opts.onFailure = onSubscribeFailure;
	opts.context = client;
	deliveredtoken = 0;
	if ((rc = MQTTAsync_subscribe(client, TOPIC_SUB_MOVETO, QOS, &opts)) != MQTTASYNC_SUCCESS)
	{
		printf("Failed to start subscribe, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
}

void sendFailure(){
	static int  send_failure_count = 0;
	send_failure_count++;
	printf("send failure\n");
	if(send_failure_count>2){
		send_failure_count = 0;
		connStatus = 0;
	}
}
void send_msg_to_server()
{
	char* smts_send_buf;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	int rc;
	opts.onFailure = sendFailure;
	smts_send_buf = obtain_car_status_from_buffer();
	deliveredtoken = 0;
	if (smts_send_buf != NULL) {
		pubmsg.payload = smts_send_buf;
		pubmsg.payloadlen = strlen(smts_send_buf);
		pubmsg.qos = QOS;
		pubmsg.retained = 0;
//		printf("send to server %s\n", smts_send_buf);
		if ((rc = MQTTAsync_sendMessage(client, TOPIC_PUB_MOVESTATUS, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
		{
			printf("Failed to start sendMessage, return code %d\n", rc);
			exit(EXIT_FAILURE);
		}
		free(smts_send_buf);
	}
}



static SendUartStruct unjson_from_server_buffer(char *text) {
	SendUartStruct ufsb_unjson_struct_buffer = InitDef;
	cJSON *ufsb_json_object;
	char *check;
	ufsb_json_object = cJSON_Parse(text);
	if (!ufsb_json_object) {
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
	}
	else {
		check = cJSON_GetObjectItem(ufsb_json_object, "time")->valuestring;
		strcat(ufsb_unjson_struct_buffer.time, check);
		check = cJSON_GetObjectItem(ufsb_json_object, "heading")->valuestring;
		strcat(ufsb_unjson_struct_buffer.heading, check);
		check = cJSON_GetObjectItem(ufsb_json_object, "latitude")->valuestring;
		strcat(ufsb_unjson_struct_buffer.laittude, check);
		check = cJSON_GetObjectItem(ufsb_json_object, "longitude")->valuestring;
		strcat(ufsb_unjson_struct_buffer.longitude, check);
		check = "00\0";
		strcat(ufsb_unjson_struct_buffer.accel, check);
		check = cJSON_GetObjectItem(ufsb_json_object, "speed")->valuestring;
		strcat(ufsb_unjson_struct_buffer.speed, check);
		check = "00\0";
		strcat(ufsb_unjson_struct_buffer. scheduleTime, check);
		cJSON_Delete(ufsb_json_object);
	}
	return ufsb_unjson_struct_buffer;
}

static	char* format_json_to_save(SendUartStruct data) {
	static uchar fjts_save_string[300] = "\0";
	uchar fjts_string_head[] = "$MOVETO,";
	uchar fjts_string_division[] = ",";
	uchar fjts_string_tail[] = "*";
	uchar end[] = "\0";
	strcat(fjts_save_string, fjts_string_head);
	strcat(fjts_save_string, data.time);
	strcat(fjts_save_string, fjts_string_division);
	strcat(fjts_save_string, data.longitude);
	strcat(fjts_save_string, fjts_string_division);
	strcat(fjts_save_string, data.laittude);
	strcat(fjts_save_string, fjts_string_division);
	strcat(fjts_save_string, data.speed);
	strcat(fjts_save_string, fjts_string_division);
	strcat(fjts_save_string, data.heading);
	strcat(fjts_save_string, fjts_string_tail);
	strcat(fjts_save_string, data.time);
	strcat(end, data.time);

	return fjts_save_string;

}

bool isMqttRcv100Packs(void) {
	return mqttReceiveBufferFullFlag;
}

DList *read_buffer_of_server(void) {
	if (pSendServerDList->size > 1) {
		return pSendServerDList;
	}
	else {
		return NULL;
	}
}

int msg_arrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
	char* payloadptr;
	uchar ma_json_txt_from_server[200] = { STRING_TAIL };
	uchar *ma_jason_to_uart_txt;
	SendUartStruct loadJsonToStruct;
	PNode pNewNode;
	printf("Message arrived\n");
	printf("     topic: %s\n", topicName);
	payloadptr = message->payload;

	if (strcmp(TOPIC_SUB_MOVETO, topicName) == 0) {
		strncat(ma_json_txt_from_server, payloadptr, message->payloadlen);
	}
	loadJsonToStruct = unjson_from_server_buffer(ma_json_txt_from_server);
	ma_jason_to_uart_txt = format_json_to_save(loadJsonToStruct);

//	printf("received %s \n", ma_jason_to_uart_txt);
	if (NULL == pSendServerDList) {
		pthread_mutex_lock(&mutex);
		pSendServerDList = InitList(ma_jason_to_uart_txt);
		pthread_mutex_unlock(&mutex);
	}
	else {
		pthread_mutex_lock(&mutex);
		pNewNode = MakeNode(ma_jason_to_uart_txt);

		InsAfterTail(pSendServerDList, pNewNode);
	//	printf("sace data is %s\n",pSendServerDList->head->data);
		pthread_mutex_unlock(&mutex);
		printf("size is :%d\n", pSendServerDList->size);
	}
	if (pSendServerDList->size >= BUFFER_SIZE_BEFORE_SEND) {
		mqttReceiveBufferFullFlag = true;
	}
	ma_jason_to_uart_txt[0] = '\0';
	return MQTTRCVSUCCESSFLAG;
}



