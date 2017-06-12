/*   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include "commeinclude.h"

#define RTCIRQFREQUENCY	100//设置硬件RTC中断频率
/*---------------------------------------------*/

static int initial_thread();//线程初始化
static void* thread_timer(void* arg);//RTC线程
/*-----------------------------------------------------*/

static bool TimerUart10msFlag = false;//串口10ms发送标志位
static bool TimerMqtt10msFlag = false;//MQTT10ms发送标志位
pthread_mutex_t mutex;
/*------------------------------------------------------*/

int main(int argc, char* argv[]) {
	pthread_mutex_init(&mutex, NULL);
	initial_thread();
	while (1) {
	}
	return 0;
}
/*-----------------------------------------------------*/

int initial_thread() {
	pthread_t it_id_timer;
	pthread_t it_id_uart;
	pthread_t it_id_mqtt;

	pthread_create(&it_id_timer, NULL, thread_timer, NULL);
	pthread_create(&it_id_uart, NULL, thread_uart, NULL);
	pthread_create(&it_id_mqtt, NULL, thread_mqtt, NULL);
	return 0;
}

void* thread_timer(void* arg) {
	unsigned long data = 0;
	struct timeval tvs, tve;

	int tt_fd = open("/dev/rtc0", O_RDONLY);
	if (tt_fd < 0) {
		perror("open");
		exit(errno);
	}
	if (ioctl(tt_fd, RTC_IRQP_SET, RTCIRQFREQUENCY) < 0) {
		perror("ioctl(RTC_IRQP_SET)");
		close(tt_fd);
		exit(errno);
	}
	if (ioctl(tt_fd, RTC_PIE_ON, 0) < 0) {
		perror("ioctl(RTC_PIE_ON)");
		close(tt_fd);
		exit(errno);
	}
	for (;;) {
		gettimeofday(&tvs, 0);
		if (read(tt_fd, &data, sizeof(unsigned long)) < 0) {
			perror("read");
			close(tt_fd);
			exit(errno);
		}
		TimerUart10msFlag = true;
		TimerMqtt10msFlag  = true;
		gettimeofday(&tve, 0);
	}
	ioctl(tt_fd, RTC_PIE_OFF, 0);
	close(tt_fd);
	sleep(5);
	return ((void*)0);
}
/*检查发送标志位是否为真*/

bool isCheckSendTimer(char comm){
	if( UART_COM==comm){
		if(false ==TimerUart10msFlag){
			return false;
		}
		else{
			TimerUart10msFlag =false;
			return true;
		}
	}
	else if(MQTT_COM == comm){
		if(false == TimerMqtt10msFlag){
			return false;
		}
		else{
			TimerMqtt10msFlag = false;
			return true;
		}
	}
	return false;
}



