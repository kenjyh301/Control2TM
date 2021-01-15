/*
 * screenDataQueue.cpp
 *
 *  Created on: Oct 22, 2020
 *      Author: Minh
 */

#include "sceenDataQueue.h"

mTraceData::mTraceData(){
	az=0;
	azdd=0;
	memset(data,0,ELEMENT_SIZE);
}

mTraceData::mTraceData(int az,int azd,uint8_t* data){
	this->az=az;
	this->azdd= azd;
	memcpy(this->data,data,ELEMENT_SIZE);
}

screenDataQueue::screenDataQueue(){
	mReadPointer=0;
	mWritePointer=0;
}

int screenDataQueue::pop(mTraceData& output){
	if(isEmpty()){
//		printf("ScreenDataQueue is empty\n");
		return -1;
	}
	output = data[mReadPointer];
	mReadPointer++;
	if(mReadPointer==QUEUE_MAX)mReadPointer=0;
	return 1;
}

int screenDataQueue::push(mTraceData input){
	if(isFull()){
		printf("ScreenDataQueue is full\n");
		return -1;
	}
	data[mWritePointer]= input;
	mWritePointer++;
	if(mWritePointer==QUEUE_MAX)mWritePointer=0;
	return 1;
}

int screenDataQueue::isEmpty(){
	if(mReadPointer==mWritePointer) return 1;
	return 0;
}

int screenDataQueue::isFull(){
	if(mReadPointer-mWritePointer==1)return 1;
	if(mReadPointer==0&&mWritePointer==QUEUE_MAX-1) return 1;
	return 0;
}

void screenDataQueue::freeQueue(){
	mReadPointer=0;
	mWritePointer=0;
}


