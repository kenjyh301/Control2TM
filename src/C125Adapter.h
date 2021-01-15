/*
 * C125Adapter.h
 *
 *  Created on: Aug 6, 2020
 *      Author: CongThuan
 */

#ifndef C125ADAPTER_H_
#define C125ADAPTER_H_

#include <iostream>
#include <unistd.h>
#include "C125DataProcessor.h"
#include "SystemConfigurations.h"
#include "MonitorProcessor.h"
#include "onScreenTarget.h"


using namespace std;
#define CONFIGURE_FILE			"/root/C125Adapter/config.ini"

pthread_mutex_t uk51MutexLock;
pthread_mutex_t uk31MutexLock;
pthread_mutex_t uk61MutexLock;
pthread_mutex_t uk43MutexLock;

void delete_queue (void) {
	remove("/dev/mqueue/queue1");
}

void setMutex(C125DataProcessor &Process, MonitorProcessor &c125Mon){
	C125DataProcessor::getC125DataProcessor().setUk31MutexLock(&uk31MutexLock);
	C125DataProcessor::getC125DataProcessor().setUk51MutexLock(&uk51MutexLock);
	C125DataProcessor::getC125DataProcessor().setUk61MutexLock(&uk61MutexLock);
	C125DataProcessor::getC125DataProcessor().setUk43MutexLock(&uk43MutexLock);

	c125Mon.setUk31MutexLock(&uk31MutexLock);
	c125Mon.setUk51MutexLock(&uk51MutexLock);
	c125Mon.setUk61MutexLock(&uk61MutexLock);
	c125Mon.setUk43MutexLock(&uk43MutexLock);
}

#endif /* C125ADAPTER_H_ */
