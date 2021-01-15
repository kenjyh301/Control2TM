/*
 * C125Communicator.h
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */

#ifndef C125COMMUNICATOR_H_
#define C125COMMUNICATOR_H_

#include "stdio.h"
#include "cmmdefs.h"
#include <pthread.h>
#include "FileReaderHelper.h"
using namespace std;

class C125Communicator {
public:
	C125Communicator();
	virtual ~C125Communicator();

	int assignTarget(float bear, float range);
	int requestUpdateTrack(float bear, float range);
	int deassignTarget();

	int readSystemStatus();
	int setATAInfo(float bear, float range, unsigned int height, unsigned int velocity);
	int setLabelDefault();
	int setTrackSimulationToIFile(TrackSimulation trackSimulation);
	int readTrackSimulationFromIFile(TrackSimulation &trackSimulation);

	void setUk51MutexLock(pthread_mutex_t* mutex);
	void setUk31MutexLock(pthread_mutex_t* mutex);
	void setUk61MutexLock(pthread_mutex_t* mutex);
	void setUk43MutexLock(pthread_mutex_t* mutex);

//	FileReaderHelper* getUk51FileReader(void);
//	FileReaderHelper* getUk31FileReader(void);
//	FileReaderHelper* getUk61FileReader(void);
//	FileReaderHelper* getUk43FileReader(void);

	int initFileWriter();
private:

	int getScaleMode(int *scale);
	int setScaleMode(int scale);
	std::string getNoiseMode(int type);
	int getNoiseMode(std::string typeStr);
	std::string generateTrackSimulationData(bool isDefaultTrackData, int trackNum, TrackSimulationData dataTrack);//_ gen string value if <= trackNumSize, else default
	int adjustScaleBaseOnRange(long range);
private:

	pthread_mutex_t* uk51MutexLock;
	pthread_mutex_t* uk31MutexLock;
	pthread_mutex_t* uk61MutexLock;
	pthread_mutex_t* uk43MutexLock;

	FileReaderHelper mUk51FileReader;
	FileReaderHelper mUk31FileReader;
	FileReaderHelper mUk61FileReader;
	FileReaderHelper mUk43FileReader;

	std::string mUk51DefaultStatus;
	std::string mUk31DefaultStatus;

	std::string mTrackDataDefaultWithoutTrackNumber;
	string mStartLession;
};

#endif /* C125COMMUNICATOR_H_ */
