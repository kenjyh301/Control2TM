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

pthread_mutex_t server_connect_lock;
pthread_mutex_t cat48_mutex;

tcp_server* targetServer= new tcp_server(PORT);
tcp_server* dataServer= new tcp_server(DATA_PORT);

int connfd = -1;

mqd_t target_mq;
struct mq_attr target_mq_attr;


void* mTargetServerConnect(void* args) {
	struct tcp_server* targetServer=(struct tcp_server*)args;

	char tcp_port[10];
	sprintf(tcp_port,"%d",targetServer->port);
	pthread_mutex_init(&server_connect_lock, NULL);
	int socketId;
	socketId = inet_listen(tcp_port, 0, NULL);
	printf("listening...\n");
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
	iff_init();
	mTraceData currentData;
	while (1) {
		int ret = scrQueue.pop(currentData);
		if (ret == -1) {
			continue;
			usleep(10);
		}

		iff_processing(currentData);
		usleep(10);
	}
	return NULL;
}

void* mSendTarget(void*){

	struct timespec tm;
	/* initialize the queue attributes */
	target_mq_attr.mq_maxmsg = 100;
	target_mq_attr.mq_msgsize = CAT48_LEN;  //size of cat48 data package

//	if(mq_unlink("/target_mq")==-1)exit(0);
	target_mq= mq_open("/target_mq1",O_RDWR|O_CREAT|O_NONBLOCK,0777,&target_mq_attr);
	if(target_mq==-1){
		printf("open target_mq error\n");
		exit(0);
	}




//	while (targetServer->connfd == -1)
//		usleep(100);
	unsigned char** cat48 = new unsigned char*;
	unsigned char* cat48_full= new unsigned char[CAT48_LEN];
	cat48_full[0]='S';cat48_full[1]='i';cat48_full[2]='m';
	int* msg_len = new int;

	unsigned char type = 1;

	while (1) {
		while (target_ready == 0)
			usleep(100);

		pthread_mutex_lock(&server_connect_lock);
		//convert range phase t o m and 360/2^16 degree
		const int scale_range = 1000;
		const float scale_phase = 65536.0 / 6000.0;
		uint16_t cat48_range = center_of_range * scale_range;
		uint16_t cat48_phase = center_of_phase * scale_phase;

//		cat48_range=5000;
//		cat48_phase=7282;

//		printf("We are here:%hx %hx\n\n\n", cat48_range, cat48_phase);


		coding_cat048_plot_report(cat48_phase, cat48_range, type, cat48,
				msg_len);

		memcpy(cat48_full+3,*cat48,18);
		clock_gettime(CLOCK_REALTIME,&tm);
		tm.tv_nsec=50;
//		int status= mq_timedsend(target_mq,(char*)*cat48,(unsigned int)*msg_len,0,&tm);
		int status = mq_send(target_mq,(char*)cat48_full,CAT48_LEN,0);
		if(status<0){
			printf("target_mq send failure %d\n",status);
		}
		else{
			printf("target_mq send success\n");
		}


		write(targetServer->connfd, *cat48, *msg_len);
		target_ready = 0;
		pthread_mutex_unlock(&server_connect_lock);
	}
	mq_close(target_mq);
	return NULL;
}


