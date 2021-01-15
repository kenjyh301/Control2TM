/*
 * DhlCommunicator.h
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */

#ifndef DHLCOMMUNICATOR_H_
#define DHLCOMMUNICATOR_H_

#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <pthread.h>
#include <string>
#include "MessageQueues.h"
#include "SystemConfigurations.h"
#include "inet_sockets.h"
#include "dataType.h"
#include "onScreenTarget.h"


using namespace std;


class DhlCommunicator {
public:
	virtual ~DhlCommunicator();
	DhlCommunicator();

	void run();
	void join();

	int getReceivedMessage(C125Message &msg);
	int transmitMessage(const C125Message &msg);

	static void *listening(void *args);
	static void *dataTransmiting(void *args);
	static void *dataReceiving(void *args);
	static mqd_t mqfd1;
	static mqd_t target_mq_rece;

	void getConnectionID(){
		printf("%d",mConnectionId);
	}
	void setConnectionID(){
		mConnectionId=-1;
	}


private:
	MessageQueues mReceivedMessageQueue;
	MessageQueues mRespondMessageQueue;
	pthread_mutex_t mWriteMessageLocker;
	pthread_mutex_t mReadMessageLocker;
	pthread_t mListeningConnectionThread;
	pthread_t mDataTranssasionThread;
	pthread_t mDataReceivingThread;
	int mSocketId;
	int mConnectionId;
	pthread_mutex_t mConnectionLocker;

	std::string mPort;
};

#endif /* DHLCOMMUNICATOR_H_ */
