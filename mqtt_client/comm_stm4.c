#include "commeinclude.h"

static void DebugShowSysTime(void);
static long get_schedule_time(void);
static char* json_car_status(SendServerStruct CarStatus);
static SendServerStruct uart_buffer_analysis(char * receivedata, struct timeval uba_rcv_time);
static int debugSendCount = 0;
static long scheduleTime = 0;
static char timeRcvServer[40] = { '0','\0' };
DList *dataBufferFromUart = NULL;
char  DebugUartBuffer[] = "$MOVETO,10.10,20.20,30.30,40.40,50.50*";

void* thread_uart(void* arg) {
	uint tu_uart_read_count_once;
	uchar tu_read_uart_file[5] = { 0 };
	uchar tu_uart_pack_buffer[200];
	uchar tu_read_buffer_index = 0;
	struct timeval tu_schedule_time = { 0, 0 };
	int tu_uart_fd;
	int reco;
	uint tu_uart_send_len_count = 0;
	uint tu_uart_time_count = 0;
	PNode tu_uart_creat_new_pnode;
	DList *tu_uart_receive_pbuffer = NULL;
	tu_uart_pack_buffer[tu_read_buffer_index] = STRING_TAIL;
	tu_uart_fd = SetUartSate();
	do {
		tu_uart_read_count_once = read(tu_uart_fd, tu_read_uart_file, sizeof(unsigned char));
		if (tu_uart_read_count_once > 0) {
			if (tu_read_uart_file[0] == UART_HEAD_SIGNAL) {
				tu_read_buffer_index = 0;
				tu_uart_pack_buffer[tu_read_buffer_index++] = tu_read_uart_file[0];
			}
			else if (tu_read_uart_file[0] == UART_TAIL_SIGNAL) {
				tu_uart_pack_buffer[tu_read_buffer_index++] = tu_read_uart_file[0];
				tu_uart_pack_buffer[tu_read_buffer_index] = '\0';
				pthread_mutex_lock(&mutex);
				dataBufferFromUart = InitList(tu_uart_pack_buffer);
				pthread_mutex_unlock(&mutex);
				tu_read_buffer_index = 0;
				printf("uart size is :%d\n", dataBufferFromUart->size);
			}
		}
		sleep(0.0003);
	} while (dataBufferFromUart == NULL);
	for (;;) {
		struct timeval DEBUG_TPSTART;
		double DEBUG_TIMEUSE;
		double DEBUG_TIMELOST = 0;
		static long sendcounttom4 = 0;
		static long reccountm4 = 0;
		char  send_buffer_uart[100];
		int i;
		tu_uart_read_count_once = read(tu_uart_fd, tu_read_uart_file, sizeof(tu_read_uart_file));
		if (tu_uart_read_count_once > 0) {
			if (tu_read_uart_file[0] == UART_HEAD_SIGNAL) {
				tu_read_buffer_index = 0;
				tu_uart_pack_buffer[tu_read_buffer_index++] = tu_read_uart_file[0];
			}
			else if (tu_read_uart_file[0] == UART_TAIL_SIGNAL) {
				tu_uart_pack_buffer[tu_read_buffer_index++] = tu_read_uart_file[0];
				tu_uart_pack_buffer[tu_read_buffer_index] = STRING_TAIL;
				tu_uart_creat_new_pnode = MakeNode(tu_uart_pack_buffer);
				pthread_mutex_lock(&mutex);
				InsAfterTail(dataBufferFromUart, tu_uart_creat_new_pnode);
				pthread_mutex_unlock(&mutex);
				tu_read_buffer_index = 0;
				reccountm4++;
				DebugShowSysTime();
			}
			else {
				tu_uart_pack_buffer[tu_read_buffer_index++] = tu_read_uart_file[0];
			}
		}
		if (isCheckSendTimer(UART_COM) && isMqttRcv100Packs()) {
			tu_uart_receive_pbuffer = read_buffer_of_server();
			if (NULL != tu_uart_receive_pbuffer) {
				tu_schedule_time.tv_sec = 0;
				tu_schedule_time.tv_usec = 0;
				tu_uart_time_count = 0;
				gettimeofday(&tu_schedule_time, 0);
				scheduleTime = (tu_schedule_time.tv_sec - tu_uart_receive_pbuffer->head->receivetime.tv_sec) * 1000\
					+ (tu_schedule_time.tv_usec - tu_uart_receive_pbuffer->head->receivetime.tv_usec) / 1000;
				tu_uart_send_len_count = 0;
				for (i = 0; tu_uart_receive_pbuffer->head->data[i] != '*'; i++) {
					tu_uart_send_len_count++;
				}
				for (i = 0; i < tu_uart_send_len_count + 1; i++) {
					send_buffer_uart[i] = tu_uart_receive_pbuffer->head->data[i];
				}
				write(tu_uart_fd, tu_uart_receive_pbuffer->head->data, tu_uart_send_len_count + 1);
				tu_uart_send_len_count += 1;
				for (; tu_uart_receive_pbuffer->head->data[tu_uart_send_len_count] != '\0'; tu_uart_send_len_count++) {
					timeRcvServer[tu_uart_time_count++] = tu_uart_receive_pbuffer->head->data[tu_uart_send_len_count];
				}
				timeRcvServer[tu_uart_time_count] = '\0';
				debugSendCount++;
				pthread_mutex_lock(&mutex);
				free(DelHeadNode(tu_uart_receive_pbuffer));
				pthread_mutex_unlock(&mutex);
				DEBUG_TIMELOST = (DEBUG_TPSTART.tv_sec * 1000000L + DEBUG_TPSTART.tv_usec) / 1000.0f;
				gettimeofday(&DEBUG_TPSTART, NULL);
				DEBUG_TIMEUSE = (DEBUG_TPSTART.tv_sec * 1000000L + DEBUG_TPSTART.tv_usec) / 1000.0f - DEBUG_TIMELOST;
			}
		}
		sleep(0.0003);
	}
	close(tu_uart_fd);
	return ((void *)0);
}

static void DebugShowSysTime(void) {
	struct timeval tv;
	gettimeofday(&tv, 0);
}

char * json_car_status(SendServerStruct CarStatus) {
	cJSON *jcs_json_object_buffer;
	char *jcs_json_text;
	long jcs_scheduletime = 0;
	char jcs_send[50];
	jcs_scheduletime = get_schedule_time();
	sprintf(jcs_send, "%d", jcs_scheduletime);
	jcs_json_object_buffer = cJSON_CreateObject();
	cJSON_AddStringToObject(jcs_json_object_buffer, "sendTime", timeRcvServer);
	cJSON_AddStringToObject(jcs_json_object_buffer, "longitude", CarStatus.longitude);
	cJSON_AddStringToObject(jcs_json_object_buffer, "latitude", CarStatus.laittude);
	cJSON_AddStringToObject(jcs_json_object_buffer, "speed", CarStatus.speed);
	cJSON_AddStringToObject(jcs_json_object_buffer, "heading", CarStatus.heading);
	cJSON_AddStringToObject(jcs_json_object_buffer, "accel", CarStatus.accel);
	cJSON_AddStringToObject(jcs_json_object_buffer, "scheduleTime", jcs_send);
	jcs_json_text = cJSON_Print(jcs_json_object_buffer);
	cJSON_Delete(jcs_json_object_buffer);
	return jcs_json_text;
}

static SendServerStruct uart_buffer_analysis(char * receivedata, struct timeval uba_rcv_time) {
	struct
	{
		unsigned char longitude;
		unsigned char latitude;
		unsigned char speed;
		unsigned char heading;
		unsigned char accel;
		unsigned char division;
		unsigned char index;
	} uba_count = { 0, 0, 0, 0, 0, 0, 0 };
	SendServerStruct uba_division_uart_buffer;
	uchar str = ',';
	for (uba_count.index = 0; receivedata[uba_count.index] != '*'; uba_count.index++) {
		if (receivedata[uba_count.index] == str) {
			uba_count.division++;
		}
		else {
			switch (uba_count.division) {
			case EMPTY:
				break;
			case LONGITUDEFLAG:
				uba_division_uart_buffer.longitude[uba_count.longitude++] = receivedata[uba_count.index];
				break;
			case LATITUDEFLAG:
				uba_division_uart_buffer.laittude[uba_count.latitude++] = receivedata[uba_count.index];
				break;
			case SPEEDFLAG:
				uba_division_uart_buffer.speed[uba_count.speed++] = receivedata[uba_count.index];
				break;
			case HEADFLAG:
				uba_division_uart_buffer.heading[uba_count.heading++] = receivedata[uba_count.index];
				break;
			case ACCELFLAG:
				uba_division_uart_buffer.accel[uba_count.accel++] = receivedata[uba_count.index];
				break;
			default:
				break;
			}
		}
	}
	uba_division_uart_buffer.longitude[uba_count.longitude] = STRING_TAIL;
	uba_division_uart_buffer.laittude[uba_count.latitude] = STRING_TAIL;
	uba_division_uart_buffer.heading[uba_count.heading] = STRING_TAIL;
	uba_division_uart_buffer.speed[uba_count.speed] = STRING_TAIL;
	uba_division_uart_buffer.accel[uba_count.accel] = STRING_TAIL;
	return uba_division_uart_buffer;
}

char *obtain_car_status_from_buffer() {
	uchar *ocsfb_json_txt;
	SendServerStruct ocsfb_struct_latest_car_status_buffer;
	if (!dataBufferFromUart) {
		return NULL;
	}
	if (dataBufferFromUart->size > 1) {
		pthread_mutex_lock(&mutex);
		ocsfb_struct_latest_car_status_buffer = uart_buffer_analysis(dataBufferFromUart->tail->data, dataBufferFromUart->tail->receivetime);
		ocsfb_json_txt = json_car_status(ocsfb_struct_latest_car_status_buffer);
		free(DelHeadNode(dataBufferFromUart));
		pthread_mutex_unlock(&mutex);
		return ocsfb_json_txt;
	}
	else
	{
		return NULL;
	}
}

static long get_schedule_time(void) {
	long gst_schedule_time = scheduleTime;
	return gst_schedule_time;
}





