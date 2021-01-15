/*
 * MessageQueues.cpp
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */

#include "MessageQueues.h"

MessageQueues::MessageQueues() {
	// TODO Auto-generated constructor stub
	mReadIndex = 0;
	mWriteIndex = 0;
	mMessageCount = 0;
}

MessageQueues::~MessageQueues() {
	// TODO Auto-generated destructor stub
}

int MessageQueues::pop(C125Message &message)
{
	int returnCode = ERR_SUCCESS;
	if(true == isEmpty()){
		returnCode = ERR_QUEUE_EMPTY;
		return returnCode;
	}
	message = mMessages[mReadIndex];
	mReadIndex = mReadIndex + 1;
	mMessageCount = mMessageCount - 1;
	if( mReadIndex >= MAX_MESSAGE_QUEUES_SIZE ){
		mReadIndex = 0;
	}

	return returnCode;
}
int MessageQueues::push(const C125Message &message)
{
	int returnCode = ERR_SUCCESS;
	if(true == isFull()){
		returnCode = ERR_QUEUE_FULL;
		return returnCode;
	}
	mMessages[mWriteIndex] = message;
	mWriteIndex = mWriteIndex + 1;
	mMessageCount = mMessageCount + 1;
	if( mWriteIndex >= MAX_MESSAGE_QUEUES_SIZE ){
		mWriteIndex = 0;
	}
	return returnCode;
}
bool MessageQueues::isEmpty()
{
	return (this->mMessageCount <=0);
}

bool MessageQueues::isFull()
{
	return (this->mMessageCount>= MAX_MESSAGE_QUEUES_SIZE);
}
