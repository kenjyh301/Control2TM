/*
 * sceenDataQueue.h
 *
 *  Created on: Oct 22, 2020
 *      Author: Minh
 */

#ifndef SCEENDATAQUEUE_H_
#define SCEENDATAQUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>

using namespace std;

#define ELEMENT_SIZE 150
#define QUEUE_MAX 10000

class mTraceData{
public:
	int az;
	int azdd;
	uint8_t data[ELEMENT_SIZE];
public:
	mTraceData();
	mTraceData(int az,int azd,uint8_t* data);
};

class screenDataQueue{
public:
	screenDataQueue();
	int pop(mTraceData& output);
	int push(mTraceData input);
	int isEmpty();
	int isFull();
	void freeQueue();

public:
	int mReadPointer;
	int mWritePointer;
	mTraceData data[QUEUE_MAX];
};

#endif /* SCEENDATAQUEUE_H_ */
