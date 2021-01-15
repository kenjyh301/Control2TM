/*
 * dataProcess.h
 *
 *  Created on: Oct 16, 2020
 *      Author: Minh
 */

#ifndef PPIDATA_H_
#define PPIDATA_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <sys/dcmd_chr.h>


#include "screenDataQueue/sceenDataQueue.h"
#include "onScreenTarget.h"
#include "ExTarget.h"

#define NUM_BYTES 1370
#define DATA_BASE_0		0x0804f08c
#define DATA_BASE_1 	0x080586b4
#define PORT 5353
#define SA struct sockaddr
#define azd 10
#define SAMPLE_TIME 16000
#define SLEEP_TIME 1000

extern screenDataQueue scrQueue;
extern int ppi51_pid;
extern int ppi51_chid;
extern bool excerciseMode;

void* mdataRead_from_file(void*);
void* mdataRead(void*);
void print_current_data();

void* mDrawRadar(void*);


#endif /* PPIDATA_H_ */
