/*
 * C125DataProcessor.h
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */

#ifndef C125DATAPROCESSOR_H_
#define C125DATAPROCESSOR_H_

#include "SystemComponentDetector.h"
#include "C125Communicator.h"
#include "DhlCommunicator.h"
#include "cmmenums.h"
#include <unistd.h>
#include <iostream>
#include <string.h>

class C125DataProcessor {
public:
	C125DataProcessor();
	virtual ~C125DataProcessor();
	void run();
	static void* communicatorThreadRun (void *argc);

	static C125Communicator& getC125DataProcessor() {return mC125Communicator;}

private:
	static DhlCommunicator mDhlComunicator;
	static C125Communicator mC125Communicator;
	pthread_t communicatorThread;
};

#endif /* C125DATAPROCESSOR_H_ */
