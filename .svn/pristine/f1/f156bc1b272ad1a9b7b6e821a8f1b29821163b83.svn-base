/*
 * MessageQueues.h
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */

#ifndef MESSAGEQUEUES_H_
#define MESSAGEQUEUES_H_

#include "cmmdefs.h"

#define MAX_MESSAGE_QUEUES_SIZE			100

class MessageQueues {
public:
	MessageQueues();
	virtual ~MessageQueues();

	int pop(C125Message &message);
	int push(const C125Message &message);
	bool isEmpty();
	bool isFull();
private:
	C125Message mMessages[MAX_MESSAGE_QUEUES_SIZE];
	int mReadIndex;
	int mWriteIndex;
	int mMessageCount;
};

#endif /* MESSAGEQUEUES_H_ */
