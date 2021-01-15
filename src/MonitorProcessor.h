/*
 * Monitor Processor.h
 *
 *  Created on: Jul 16, 2020
 *      Author: CongThuan
 */

#ifndef MONITORPROCESSOR_H_
#define MONITORPROCESSOR_H_

#include "C125Communicator.h"
#include <mqueue.h>
#include "DhlCommunicator.h"
#include "inet_sockets.h"
#include "SystemConfigurations.h"
#include "FileReaderHelper.h"
#include "C125DataProcessor.h"
#include <queue>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <sys/socket.h>
#include "SystemComponentDetector.h"
#include <errno.h>
#include "dataType.h"

class MonitorProcessor {
public:
	MonitorProcessor();
	virtual ~MonitorProcessor();

	static void getPath();
	void setUk51MutexLock(pthread_mutex_t* mutex);
	void setUk31MutexLock(pthread_mutex_t* mutex);
	void setUk61MutexLock(pthread_mutex_t* mutex);
	void setUk43MutexLock(pthread_mutex_t* mutex);

	void setUk51FileReader(FileReaderHelper*);
	void setUk31FileReader(FileReaderHelper*);
	void setUk61FileReader(FileReaderHelper*);
	void setUk43FileReader(FileReaderHelper*);

	static FileReaderHelper mUk51FileReader;
	static FileReaderHelper mUk31FileReader;
	static FileReaderHelper mUk61FileReader;
	static FileReaderHelper mUk43FileReader;

	void run();
	static mqd_t mqfd;
private:
	static std::string GatewayPort;

	pthread_t mServerListening;
	pthread_t mSendDataThread;
	pthread_t mReadDataThread;
	pthread_t mDataReceivingThread;

	static pthread_mutex_t* uk51MutexLock;
	static pthread_mutex_t* uk31MutexLock;
	static pthread_mutex_t* uk61MutexLock;
	static pthread_mutex_t* uk43MutexLock;

	static pthread_mutex_t mConnectionLock;

	static int socketId;
	static int connectId;

	static void *readData(void *args);
//	static void *listening(void *args);
	static void *dataTransmiting(void *args);
	static void *dataReceiving(void *args);

};

#endif /* MONITORPROCESSOR_H_ */
