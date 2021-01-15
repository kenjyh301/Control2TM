/*
 * onScreenTarget.cpp
 *
 *  Created on: Oct 26, 2020
 *      Author: Minh
 */
#include "onScreenTarget.h"
#include <string>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <mqueue.h>

pthread_t tcp_targer_thread;
pthread_t tcp_data_thread;
pthread_t data_thread;
pthread_t detect_thread;
pthread_t send_thread;
pthread_t draw_thread;

pthread_mutex_t server_connect_lock;
pthread_mutex_t cat48_mutex;
pthread_mutex_t circle_end_lock;

tcp_server* targetServer = new tcp_server(PORT);
tcp_server* dataServer = new tcp_server(DATA_PORT);

int connfd = -1;

mqd_t target_mq;
struct mq_attr target_mq_attr;

bool circleEnd = false;
int C125_SCALE=100;

void* mTargetServerConnect(void* args) {
	struct tcp_server* targetServer = (struct tcp_server*) args;

	char tcp_port[10];
	sprintf(tcp_port, "%d", targetServer->port);
	pthread_mutex_init(&server_connect_lock, NULL);
	int socketId;
	socketId = inet_listen(tcp_port, 0, NULL);
	printf("Target server listening...\n");
	if (socketId < 0) {
		printf("cannot create socket\n");
		return NULL;
	}
	pthread_mutex_lock(&server_connect_lock);
	if (targetServer->socketfd > 0) {
		close(targetServer->socketfd);
	}
	pthread_mutex_unlock(&server_connect_lock);
	targetServer->socketfd = socketId;

	char s[INET6_ADDRSTRLEN];
	struct sockaddr_storage client_address;
	socklen_t sin_size = sizeof(client_address);

	while (1) {

		connfd
				= accept(socketId, (struct sockaddr*) &client_address,
						&sin_size);

		//		scrQueue.freeQueue();

		if (connfd < 0) {
			printf("cannot connected\n");
			return NULL;
		}
		inet_ntop(client_address.ss_family, get_in_addr(
				(struct sockaddr *) &client_address), s, sizeof(s));
		printf("connected from %s, port %d\n", s, socketId);

		pthread_mutex_lock(&server_connect_lock);
		if (targetServer->connfd > 0) {
			close(targetServer->connfd);
		}
		usleep(200);
		pthread_mutex_unlock(&server_connect_lock);

		targetServer->connfd = connfd;
	}
	return NULL;
}

void* mDetectTarget(void*) {
	static int pre_az = 0;
	iff_init();
	mTraceData currentData;
	while (1) {
		int ret = scrQueue.pop(currentData);
		if (ret == -1) {
			continue;
			usleep(10);
		}
		//detect when radar run completed circle send cat34 north mark
		pthread_mutex_lock(&circle_end_lock);
		if((currentData.az<pre_az)&&(currentData.az<100)&&(pre_az>5900)){
			circleEnd=true;
		}
		pthread_mutex_unlock(&circle_end_lock);
		//		else circleEnd=false;
		pre_az= currentData.az;


		iff_processing(currentData);
		usleep(10);
	}
	return NULL;
}

void* mSendTarget(void*) {

	static int c_q=0;

	struct timespec tm;
	/* initialize the queue attributes */
	target_mq_attr.mq_maxmsg = 100;
	target_mq_attr.mq_msgsize = CAT48_LEN; //size of cat48 data package

	target_mq = mq_open("/target_mq1", O_RDWR | O_CREAT | O_NONBLOCK, 0777,
			&target_mq_attr);
	if (target_mq == -1) {
		printf("open target_mq error\n");
		exit(0);
	}

	//	while (targetServer->connfd == -1)
	//		usleep(100);
	unsigned char** cat48 = new unsigned char*;
	unsigned char** cat34 = new unsigned char*;
	unsigned char* cat48_full = new unsigned char[CAT48_LEN];
	cat48_full[0] = 'S';
	cat48_full[1] = 'i';
	cat48_full[2] = 'm';
	int* msg_len = new int;

	unsigned char type = 1;

	while (1) {
		//check posible data ready
		while (target_ready == 0&&circleEnd==false) {
			usleep(100);
		}

		// Check north maker
		pthread_mutex_lock(&circle_end_lock);
		if (circleEnd == true) {

			//				printf("Circle End handle\n");
			//cat34 reuse cat48 buffer
			coding_cat034_north_marker(cat34, msg_len);
			//				for(int i=0;i<*msg_len;i++){
			//					printf("%hhx\t",*cat34[i]);
			//				}
			if (*msg_len > CAT48_LEN) {
				printf("Incompatible length: too short");
				exit(0);
			}
			//				printf("%hhx\n",(*cat34)[0]);
			memset(cat48_full + 3, 0, CAT48_LEN - 3);
			memcpy(cat48_full + 3, *cat34, *msg_len);
			for(int i=0;i<10;i++){
						printf("%hhx\t",(*cat34)[i]);
					}
			printf("\n");
			//				printf("%hhx\n",(cat48_full)[3]);
			int status = mq_send(target_mq, (char*) cat48_full, CAT48_LEN,
					0);
			if (status < 0) {
				printf("target_mq send failure %d\n", status);
			} else {


				printf("target_mq send success cat34 \n");
//				printf("\n\n%d\n\n",++c_q);
			}
			circleEnd = false;//handle done
			continue;// north marker higher priority to plot object
		}
		pthread_mutex_unlock(&circle_end_lock);
		usleep(10);

		//lock TCP server to send video data
		pthread_mutex_lock(&server_connect_lock);
		//convert range phase t o m and 360/2^16 degree
//		const int scale_range = 1000;
		const float scale_phase = 65536.0 / 6000.0;
		int line_size= SystemConfigurations::getInstance()->get_line_size();
		uint32_t cat48_range = center_of_range*C125_SCALE*1000/line_size;
		uint32_t cat48_phase = center_of_phase * scale_phase;

		//		cat48_range=5000;
		//		cat48_phase=7282;

		//		printf("We are here:%hx %hx\n\n\n", cat48_range, cat48_phase);


		coding_cat048_plot_report(cat48_phase, cat48_range, type, cat48,
				msg_len);

		memcpy(cat48_full + 3, *cat48, 18);
		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_nsec = 50;

		//		int status= mq_timedsend(target_mq,(char*)*cat48,(unsigned int)*msg_len,0,&tm);

//		cat48_full[0]=c_q;
//		c_q++;
		int status = mq_send(target_mq, (char*) cat48_full, CAT48_LEN, 0);
		if (status < 0) {
			printf("target_mq send failure %d\n", status);
		} else {
//			printf("\n\n%d\n\n",++c_q);
			pthread_mutex_lock(&cat48_mutex);
			uint16_t range = cat48_full[15] *256+cat48_full[16];
			uint16_t bear = cat48_full[17] *256+cat48_full[18];
			float r = (float) range * 1852 / 256;
			float b = (float) bear * 360 / 65536;
			for(int i=0;i<4;i++)printf("%hhx\t",cat48_full[15+i]);
			printf("Before  %d  %f   %f\n",cat48_full[0], r, b);
			pthread_mutex_unlock(&cat48_mutex);
			usleep(100000);
			printf("target_mq send success cat48\n");
		}
		write(targetServer->connfd, *cat48, *msg_len);
		target_ready = 0;
		pthread_mutex_unlock(&server_connect_lock);
	}
	mq_close(target_mq);
	return NULL;
}

