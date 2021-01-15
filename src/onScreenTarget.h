/*
 * onScreenTarget.h
 *
 *  Created on: Oct 21, 2020
 *      Author: Minh
 */

#ifndef ONSCREENTARGET_H_
#define ONSCREENTARGET_H_

#include <iostream>
#include <stdio.h>

#include "inet_sockets.h"
#include "DhlCommunicator.h"
#include "ppiDataProcess.h"
#include "iff/iff_clone.h"
//#include "header/header.h"
#include "cat48/asterix_cat.h"

using namespace std;

#define PORT 5353
#define PORT_STRING "5353"
#define DATA_PORT 5454
#define CAT48_LEN 21  // 18 bytes cat48 plus 3 bytes header "Sim"

struct tcp_server {
	int socketfd;
	int connfd;
	int port;
	tcp_server(int p) {
		socketfd = -1;
		connfd = -1;
		port = p;
	}
};

extern pthread_t tcp_targer_thread;
extern pthread_t tcp_data_thread;
extern pthread_t data_thread;
extern pthread_t detect_thread;
extern pthread_t send_thread;
extern pthread_t draw_thread;
extern tcp_server* targetServer;
extern tcp_server* dataServer;
extern mqd_t target_mq;

extern unsigned int center_of_range;
extern unsigned int center_of_phase;



void* mTargetServerConnect(void*);
void* mDetectTarget(void*);
void* mSendTarget(void*);

#endif /* ONSCREENTARGET_H_ */
