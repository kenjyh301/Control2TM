/*
 * C125Communicator.cpp
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 *  Update to simulation track by HungLQ11 in 2018
 */

#include "C125Communicator.h"
#include "SystemComponentDetector.h"
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string.h>

C125Communicator::C125Communicator() {
	// TODO Auto-generated constructor stub
	mUk51FileReader.setMeOffset(FileReaderHelper::UK51_OFFSET);
	mUk31FileReader.setMeOffset(FileReaderHelper::UK31_OFFSET);
	mUk61FileReader.setMeOffset(FileReaderHelper::UK61_OFFSET);
	mUk43FileReader.setMeOffset(FileReaderHelper::UK43_OFFSET);

	//_ Set default labels
	//	mUk31DefaultStatus = std::string("Ã�ï¿½Ã�Â Ã�Å“ Ã�Â¾Ã‘â€žÃ�Â¸Ã‘â€ Ã�ÂµÃ‘â‚¬Ã�Â° Ã�Â½Ã�Â°Ã�Â²Ã�ÂµÃ�Â´Ã�ÂµÃ�Â½Ã�Â¸Ã‘ï¿½", 22); //_ Need set Window/Preferences/General/Workspace/Text file encoding/Other/ cp8666
	//	mUk51DefaultStatus = std::string("Ã�ï¿½Ã�Â Ã�Å“ Ã�Â½Ã�Â°Ã‘â€¡Ã�Â°Ã�Â»Ã‘Å’Ã�Â½Ã�Â¸Ã�ÂºÃ�Â° Ã‘â‚¬Ã�Â°Ã‘ï¿½Ã‘â€¡Ã�ÂµÃ‘â€šÃ�Â°", 23);
	mUk31DefaultStatus = std::string("SQDK", 22); //_ Need set Window/Preferences/General/Workspace/Text file encoding/Other/ cp8666
	mUk51DefaultStatus = std::string("TDT", 23);
	mTrackDataDefaultWithoutTrackNumber = std::string("  1     0    0.0    0.0    0.0   0.0   0.0   0.0  0.0  0  0  0  0  0  - 0");
	mStartLession = "I-";

	//_ Test
	//	TrackSimulation trackSimRead;
	//	TrackSimulation trackSim;
	//
	//	trackSim.numberLession = 2;
	//	trackSim.trackDataSize = 2;
	//
	//	TrackSimulationData data;
	//	data.dangcodong = 1;
	//	data.tocdo0ms = 300;
	//	data.culi0kmx10 = 300;
	//	data.thamsoduongbay0kmx10 = 30;
	//	data.phuongvi0dox10 = 300;
	//	data.hesoquataix10 = 30;
	//	data.docao0kmx10 = 300;
	//	data.docaokkmx10 = 0;
	//	data.goccodongdo = 30;
	//	data.thoidiemcodongs = 30;
	//	data.thoidiemphatnhieus = 20;
	//	data.thoidiemphats = 0;
	//	data.thoidiemkethucs = 300;
	//	data.dangmuctieu = 2;
	//	data.dangnhieu = 3;
	//	for(int i = 0; i < C125_MAX_SIMULATION_TRACK_IN_LESSON; i++) {
	//		trackSim.trackData[i] = data;
	//		trackSim.trackData[i].tocdo0ms += 5*i;
	//	}
	//	std::cout << "Test SET return code " << setTrackSimulationToIFile(trackSim) << endl;

}

C125Communicator::~C125Communicator() {
	// TODO Auto-generated destructor stub
}

void C125Communicator::setUk51MutexLock(pthread_mutex_t* mutex) {
	uk51MutexLock = mutex;
	return;
}
void C125Communicator::setUk31MutexLock(pthread_mutex_t* mutex) {
	uk31MutexLock = mutex;
	return;
}
void C125Communicator::setUk61MutexLock(pthread_mutex_t* mutex) {
	uk61MutexLock = mutex;
	return;
}
void C125Communicator::setUk43MutexLock(pthread_mutex_t* mutex) {
	uk43MutexLock = mutex;
//	cout << "[C125Communicator] mutex uk43 address :" << uk43MutexLock << endl;
	return;
}

int C125Communicator::assignTarget(float bear, float range){
	int returnCode = ERR_SUCCESS;

	char valueRange[4];
	char valueBear[4];
	char trigerConditionValue;
	char otherCondion[4];
	*((float*)otherCondion) = -1000;
	*((float*)valueBear) = DEG_TO_RAD(bear);
	*((float*)(valueRange)) = KM_TO_METER(range);
	///Init writer
	returnCode =initFileWriter();
	if( returnCode != ERR_SUCCESS ){
		return returnCode; ///Errors occur
	}

	// TODO
	///Adjust the system scale
	returnCode = adjustScaleBaseOnRange(range);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}

	pthread_mutex_lock(uk51MutexLock);
	///Read trigger condition first
	returnCode = mUk51FileReader.readFromFile(&trigerConditionValue,1,C125_TRIGGER_ASSIGMENT_BYTE_OFFSET);
	pthread_mutex_unlock(uk51MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode; ///Errors occur
	}
	//	std::cout << "------------ here ---------------------" << std::endl;
	if( (trigerConditionValue & C125_ASSIGMENT_FLAG) ){
		///The Target have already deassign
		returnCode = ERR_TARGET_HAS_ALREADY_ASSIGN;
		return returnCode;
	}



	char uk31_findingMode = 0;
	pthread_mutex_lock(uk31MutexLock);
	returnCode = mUk31FileReader.readFromFile(&uk31_findingMode,1,C125_FINDING_BYTE_OFFSET);
	pthread_mutex_unlock(uk31MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode; ///Errors occur
	}
	printf("uk31_finding_mode: %02x ",(int)uk31_findingMode);

	if( uk31_findingMode & C125_FINDING_STATUS_CU_FLAG){
		returnCode = ERR_TARGET_HAS_ACTED;
		return returnCode;
	} else if((uk31_findingMode & C125_FINDING_STATUS_H_FLAG)==0){
		returnCode = ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN;
		return returnCode;
	}
	///Create trigger
	trigerConditionValue |= C125_ASSIGMENT_FLAG;

	///Write back to the memory

	pthread_mutex_lock(uk51MutexLock);
	returnCode = mUk51FileReader.writeToFile(&trigerConditionValue,1,C125_TRIGGER_ASSIGMENT_BYTE_OFFSET);
	pthread_mutex_unlock(uk51MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}
	//	usleep(C125_MEMORY_UPDATE_PERIOD*2);///Wait for one update circle and re-update new range and bear

	sleep(1);
	pthread_mutex_lock(uk51MutexLock);
	returnCode = mUk51FileReader.writeToFile(valueRange,4,C125_RANGE_BYTE_OFFSET);///Write bear and range concurrent
	returnCode = mUk51FileReader.writeToFile(valueBear,4,C125_BEAR_BYTE_OFFSET);///Write bear and range concurrent
	returnCode =mUk51FileReader.writeToFile(otherCondion,4,C125_RANGE_BYTE_OFFSET+4);
	pthread_mutex_unlock(uk51MutexLock);
	if(returnCode != ERR_SUCCESS ){
		return returnCode;
	}
	usleep(C125_MEMORY_UPDATE_PERIOD*2);
	///Read back memory to assume that the uk31 has been update
	char assigmentStatusFlag;
	pthread_mutex_lock(uk51MutexLock);
	returnCode = mUk51FileReader.readFromFile(&assigmentStatusFlag,1,C125_TRIGGER_ASSIGMENT_BYTE_OFFSET);
	pthread_mutex_unlock(uk51MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}
	if( (assigmentStatusFlag & C125_ASSIGMENT_FLAG) == 0 ){
		///Assign Failed
		returnCode = ERR_WRITE_FAILED;
	}
	return returnCode;
}
int C125Communicator::requestUpdateTrack(float bear, float range){
	int returnCode = ERR_SUCCESS;
	char valueRange[4];
	char valueBear[4];
	char trigerConditionValue;

	*((float*)valueBear) = DEG_TO_RAD(bear);
	*((float*)(valueRange)) = KM_TO_METER(range);
	///Init writer
	returnCode =initFileWriter();
	if( returnCode != ERR_SUCCESS ){
		return returnCode; ///Errors occur
	}

	// TODO
	///Adjust the system scale
	returnCode = adjustScaleBaseOnRange(range);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}

	///Read trigger condition first
	pthread_mutex_lock(uk51MutexLock);
	returnCode = mUk51FileReader.readFromFile(&trigerConditionValue,1,C125_TRIGGER_ASSIGMENT_BYTE_OFFSET);
	pthread_mutex_unlock(uk51MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode; ///Errors occur
	}

	if( (trigerConditionValue & C125_ASSIGMENT_FLAG) == 0 ){
		///The Target have already deassign
		returnCode = ERR_TARGET_HAS_ALREADY_DEASSIGN;
		return returnCode;
	}

	///Create trigger
	trigerConditionValue |= C125_ASSIGMENT_FLAG;

	///Write back to the memory
	returnCode = mUk51FileReader.writeToFile(&trigerConditionValue,1,C125_TRIGGER_ASSIGMENT_BYTE_OFFSET);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}

	sleep(1);

	returnCode = mUk51FileReader.writeToFile(valueRange,4,C125_RANGE_BYTE_OFFSET);///Write bear and range concurrent
	returnCode = mUk51FileReader.writeToFile(valueBear,4,C125_BEAR_BYTE_OFFSET);///Write bear and range concurrent

	return returnCode;
}

int C125Communicator::deassignTarget(){
	int returnCode = ERR_SUCCESS;
	char trigerConditionValue;

	///Read trigger condition first
	returnCode =initFileWriter();
	if( returnCode != ERR_SUCCESS ){
		return returnCode; ///Errors occur
	}
	pthread_mutex_lock(uk51MutexLock);
	returnCode = mUk51FileReader.readFromFile(&trigerConditionValue,1,C125_TRIGGER_ASSIGMENT_BYTE_OFFSET);
	pthread_mutex_unlock(uk51MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode; ///Errors occur
	}

	if( (trigerConditionValue & C125_ASSIGMENT_FLAG) ==0 ){
		///The Target have already deassign
		returnCode = ERR_TARGET_HAS_ALREADY_DEASSIGN;
		return returnCode;
	}

	char uk31_findingMode = 0;
	pthread_mutex_lock(uk31MutexLock);
	returnCode = mUk31FileReader.readFromFile(&uk31_findingMode,1,C125_FINDING_BYTE_OFFSET);
	pthread_mutex_unlock(uk31MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode; ///Errors occur
	}

	if( uk31_findingMode & C125_FINDING_STATUS_CU_FLAG){
		returnCode = ERR_TARGET_HAS_ACTED;
		return returnCode;
	}

	///Clear trigger
	trigerConditionValue &= (~C125_ASSIGMENT_FLAG);

	///Write back to the memory
	pthread_mutex_lock(uk51MutexLock);
	returnCode = mUk51FileReader.writeToFile(&trigerConditionValue,1,C125_TRIGGER_ASSIGMENT_BYTE_OFFSET);
	pthread_mutex_unlock(uk51MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}
	usleep(C125_MEMORY_UPDATE_PERIOD*2);///Wait for one update circle and re-update new range and bear
	pthread_mutex_lock(uk51MutexLock);
	returnCode = mUk51FileReader.readFromFile(&trigerConditionValue,1,C125_TRIGGER_ASSIGMENT_BYTE_OFFSET);
	pthread_mutex_unlock(uk51MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}
	if( (trigerConditionValue & C125_ASSIGMENT_FLAG) ){
		///Assign Failed
		returnCode = ERR_WRITE_FAILED;
	}
	return returnCode;
}

int C125Communicator::readSystemStatus(){
	int returnCode = ERR_SUCCESS;
	returnCode =initFileWriter();
	if( returnCode != ERR_SUCCESS ){
		return returnCode; ///Errors occur
	}
	///Read
	char uk31_byte599;
	char uk31_byte63;
	char uk51_byte48;
	pthread_mutex_lock(uk31MutexLock);
	returnCode = mUk31FileReader.readFromFile(&uk31_byte63,1,C125_SCALE_BYTE_OFFSET);
	pthread_mutex_unlock(uk31MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}
	pthread_mutex_lock(uk31MutexLock);
	returnCode = mUk31FileReader.readFromFile(&uk31_byte599,1,C125_FINDING_BYTE_OFFSET);
	pthread_mutex_unlock(uk31MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}
	pthread_mutex_lock(uk51MutexLock);
	returnCode = mUk51FileReader.readFromFile(&uk51_byte48,1,C125_TRIGGER_ASSIGMENT_BYTE_OFFSET);
	pthread_mutex_unlock(uk51MutexLock);
	if( returnCode != ERR_SUCCESS ){
		return returnCode;
	}

	std::cout<<"uk31_byte599: "<<std::hex<<(int)uk31_byte599<<" uk31_byte63: "<<(int)uk31_byte63<<" uk51_byte48:"<<(int)uk51_byte48<<std::endl;

	///Values
	switch(uk31_byte599 &0xFE){ ///Ignore first bit
	case C125_FINDING_STATUS_H_FLAG:
	{
		if(uk51_byte48 & C125_ASSIGMENT_FLAG ){
			///Already assigned
			std::cout<<"System status ERR_TARGET_HAS_ALREADY_ASSIGN"<<std::endl;
			returnCode = ERR_TARGET_HAS_ALREADY_ASSIGN;
		} else {
			std::cout<<"System status ERR_SYSTEM_READY_RECEIVED_ASSIGN"<<std::endl;
			returnCode = ERR_SYSTEM_READY_RECEIVED_ASSIGN;
		}
		break;
	}
	case C125_FINDING_STATUS_CU_FLAG:
	{
		//if((uk51_byte48 & C125_ASSIGMENT_FLAG)==0 ){
		///Already assigned
		std::cout<<"System status ERR_TARGET_HAS_ACTED"<<std::endl;
		returnCode = ERR_TARGET_HAS_ACTED;
		//} else {
		//std::cout<<"System status ERR_TARGET_HAS_ALREADY_DEASSIGN"<<std::endl;
		//returnCode = ERR_TARGET_HAS_ALREADY_DEASSIGN;
		//}
		break;
	}
	case C125_FINDING_STATUS_KO_FLAG:
	{
		std::cout<<"System status ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN"<<std::endl;
		returnCode = ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN;
		break;
	}
	case C125_FINDING_STATUS_MSP_FLAG:
	{
		std::cout<<"System status ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN"<<std::endl;
		returnCode = ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN;
		break;
	}
	case C125_FINDING_STATUS_VSP_FLAG:
	{
		std::cout<<"System status ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN"<<std::endl;
		returnCode = ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN;
		break;
	}
	case C125_FINDING_STATUS_DISABLE_FLAG:
	{
		std::cout<<"System status ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN"<<std::endl;
		returnCode = ERR_TARGET_HAS_CATCHED;
		break;
	}
	default:
		returnCode = ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN;
		break;
	}

	///Read launched and fired on the uk51
	mUk51FileReader.setMeOffset(FileReaderHelper::UK43_OFFSET);
	char launch1 = 0;
	char launch2 = 0;
	char fireOnChannel1 = 0;
	char fireOnChannel2 = 0;
	pthread_mutex_lock(uk51MutexLock);
	mUk51FileReader.readFromFile(&launch1,1,C125_LAUCH_ON_CHANNEL_1_BYTE_OFFSET);
	mUk51FileReader.readFromFile(&launch2,1,C125_LAUCH_ON_CHANNEL_2_BYTE_OFFSET);
	mUk51FileReader.readFromFile(&fireOnChannel1,1,C125_FIRE_ON_CHANNEL_1_BYTE_OFFSET);
	pthread_mutex_unlock(uk51MutexLock);
	//	mUk51FileReader.readFromFile(&fireOnChannel2,1,C125_FIRE_ON_CHANNEL_2_BYTE_OFFSET);///don't need
	fireOnChannel2 = fireOnChannel1;

	std::cout<<"uk43: lauch 1: " <<std::hex<<(int)launch1<<" lauch 2: " <<(int)launch2<<std::endl;
	std::cout<<"uk43: fire 1: " <<std::hex<<(int)fireOnChannel1<<" fire 2: "<<(int)fireOnChannel2<<std::endl;

	if((launch1&C125_LAUCH_ON_CHANNEL_1_BIT_MASK)){
		returnCode |= ERR_TARGET_HAS_LAUNCHED_MISSILE_1;
	}
	if((launch2&C125_LAUCH_ON_CHANNEL_2_BIT_MASK)){
		returnCode |= ERR_TARGET_HAS_LAUNCHED_MISSILE_2;
	}

	if((fireOnChannel1&C125_FIRE_ON_CHANNEL_1_BIT_MASK)){
		returnCode |= ERR_TARGET_HAS_MISSILE_FIRED_1;
	}
	if((fireOnChannel2&C125_FIRE_ON_CHANNEL_2_BIT_MASK)){
		returnCode |= ERR_TARGET_HAS_MISSILE_FIRED_2;
		setLabelDefault();
	}

	mUk51FileReader.setMeOffset(FileReaderHelper::UK51_OFFSET);
	return returnCode;
}

/* Description: For setting the ata Info. (char *) on status bar (top on screen) in the uk33 and uk53
 * Ata Info.'s Structure: Default Screen Name, ATA: Unit name|Sender name|B:|D:|H:
 * When: call this function when (including REQUEST_TRACK_UPDATE)
 * - Assign successfully: Update repeatedly until acted (pressing CU)
 * - Deassign successfully/Head officer cancel(l)ed/Missile fired
 * */
int C125Communicator::setATAInfo(float bear, float range, unsigned int height, unsigned int velocity) {
	int returnCode = ERR_SUCCESS, returnCodeUk31 = ERR_SUCCESS, returnCodeUk51 = ERR_SUCCESS;
	returnCode = initFileWriter();
	if( returnCode != ERR_SUCCESS ){
		std::cout << "Fail to initialize file writer" << std:: endl;
		return returnCode; ///Errors occur
	}

	char ataInfoUk31[70], ataInfoUk51[70], tempATAInfo[60], tmp[20];

	//_ Bear
	strcpy(tempATAInfo, std::string(" || ATA: B:").c_str());
	snprintf(tmp, 5, "%d", (int) bear);
	strcat(tempATAInfo, tmp);
	strcat(tempATAInfo, std::string("ÃƒÂ¸").c_str());
	snprintf(tmp, 5, "%d", (int)((bear - (int)(bear))*60));
	strcat(tempATAInfo, tmp);
	strcat(tempATAInfo, std::string("'").c_str());

	//_Range
	strcat(tempATAInfo, std::string(", Ã¢â‚¬Å¾:").c_str());
//	snprintf(tmp, 6, "%f", range);
	memset(tmp,0,20);
	sprintf(tmp,"%5.2f",range);
	strcat(tempATAInfo, tmp);

	//_ Heigh
	strcat(tempATAInfo, std::string(", H:").c_str());
	snprintf(tmp, 5, "%d", height);
	strcat(tempATAInfo, tmp);

	//_ Velocity
	strcat(tempATAInfo, std::string(", V:").c_str());
	snprintf(tmp, 5, "%d", velocity);
	strcat(tempATAInfo, tmp);
	std::cout << "tempATAInfo: " << tempATAInfo << std::endl;

	//_ set for uk31
	strcpy(ataInfoUk31, mUk31DefaultStatus.c_str());
	strcat(ataInfoUk31, tempATAInfo);
	std::cout << "Uk31Label: " << ataInfoUk31 << std::endl;
	returnCodeUk31 = mUk31FileReader.writeToFile(ataInfoUk31, sizeof(ataInfoUk31), C125_STATUS_UK31_BYTE_OFFSET);
	if( returnCodeUk31 != ERR_SUCCESS ){
		printf("Error in writing ATA Info. on screen Uk33");
		return returnCodeUk31;
	}

	strcpy(ataInfoUk51, mUk51DefaultStatus.c_str());
	strcat(ataInfoUk51, tempATAInfo);
	std::cout << "Uk51Label: " << ataInfoUk51 << std::endl;
	returnCodeUk51 = mUk51FileReader.writeToFile(ataInfoUk51, sizeof(ataInfoUk51), C125_STATUS_UK51_BYTE_OFFSET);
	if( returnCodeUk51 != ERR_SUCCESS ){
		printf("Error in writing ATA Info. on screen Uk53");
		return returnCodeUk51;
	}

	return ERR_SUCCESS;
}

int C125Communicator::setLabelDefault() {
	int returnCode = ERR_SUCCESS, returnCodeUk31 = ERR_SUCCESS, returnCodeUk51 = ERR_SUCCESS;
	returnCode = initFileWriter();
	if( returnCode != ERR_SUCCESS ){
		std::cout << "Fail to initialize file writer" << std:: endl;
		return returnCode; ///Errors occur
	}

	char tmp[23];
	//_ set for uk31
	strcpy(tmp, mUk31DefaultStatus.c_str());
	returnCodeUk31 = mUk31FileReader.writeToFile(tmp, sizeof(tmp), C125_STATUS_UK31_BYTE_OFFSET);
	if( returnCodeUk31 != ERR_SUCCESS ){
		printf("Error in writing ATA Info. on screen Uk33");
		return returnCodeUk31;
	}

	strcpy(tmp, mUk51DefaultStatus.c_str());
	returnCodeUk51 = mUk51FileReader.writeToFile(tmp, sizeof(tmp), C125_STATUS_UK51_BYTE_OFFSET);
	if( returnCodeUk51 != ERR_SUCCESS ){
		printf("Error in writing ATA Info. on screen Uk53");
		return returnCodeUk51;
	}

	return ERR_SUCCESS;
}

int C125Communicator::initFileWriter(){
	///Init path here
	std::string uk31Path = SystemComponentDetector::getUk31Path();
	std::string uk51Path = SystemComponentDetector::getUk51Path();
	std::string uk61Path = SystemComponentDetector::getUk61Path();
	std::string uk43Path = SystemComponentDetector::getUk43Path();

	if( uk31Path.empty() || uk51Path.empty()/*|| uk61Path.empty()*/){
		std::cout << "ERR_NOT_DETECT_SYSTEM_COMPONENTS" << endl;
		return ERR_NOT_DETECT_SYSTEM_COMPONENTS;
	}

	mUk31FileReader.setFilName( uk31Path);
	mUk51FileReader.setFilName( uk51Path);
	mUk61FileReader.setFilName( uk61Path);
	mUk43FileReader.setFilName( uk43Path);
	//	mUk31FileReader.setFilName( SystemComponentDetector::getUk31Path());
	return ERR_SUCCESS;
}

int C125Communicator::getScaleMode(int *scale){
	///Get scale value
	int returnCode = ERR_SUCCESS;
	returnCode = initFileWriter();
	if(returnCode != ERR_SUCCESS ){
		return returnCode;
	}

	char localScale = C125_SCALE_OPERATION_40;
	pthread_mutex_lock(uk31MutexLock);
	returnCode = mUk31FileReader.readFromFile(&localScale,1,C125_SCALE_BYTE_OFFSET);
	pthread_mutex_unlock(uk31MutexLock);
	if(returnCode != ERR_SUCCESS ){
		return returnCode;
	}

	*scale = (localScale&C125_SCALE_BIT_MASK);

	return returnCode;
}

int C125Communicator::setScaleMode(int scale){
	///Get scale value
	int returnCode = ERR_SUCCESS;
	returnCode = initFileWriter();
	if(returnCode != ERR_SUCCESS ){
		return returnCode;
	}

	char localScale = scale&0xFF;
	if(localScale == C125_SCALE_OPERATION_40){
		localScale = C125_CHANGE_SCALE_VALUE_TO_40;
	} else if(localScale == C125_SCALE_OPERATION_55){
		localScale = C125_CHANGE_SCALE_VALUE_TO_55;
	} else if(localScale == C125_SCALE_OPERATION_80){
		localScale = C125_CHANGE_SCALE_VALUE_TO_80;
	} else if(localScale == C125_SCALE_OPERATION_100){
		localScale = C125_CHANGE_SCALE_VALUE_TO_100;
	}
	returnCode = mUk31FileReader.writeToFile(&localScale,1,C125_CHANGE_SCALE_BYTE_OFFSET);
	if(returnCode != ERR_SUCCESS ){
		return returnCode;
	}
	return returnCode;
}

int C125Communicator::adjustScaleBaseOnRange(long range){
	int returnCode = ERR_SUCCESS;
	int currentScale;

	returnCode = getScaleMode(&currentScale);
	if(returnCode != ERR_SUCCESS ){
		return returnCode;
	}

	///For debug only
	if(currentScale == C125_SCALE_OPERATION_40){
		std::cout<<"System scale is: 40"<<std::endl;
	} else if(currentScale == C125_SCALE_OPERATION_55){
		std::cout<<"System scale is: 50"<<std::endl;
	} else if(currentScale == C125_SCALE_OPERATION_80){
		std::cout<<"System scale is: 80"<<std::endl;
	} else if(currentScale == C125_SCALE_OPERATION_100){
		std::cout<<"System scale is: 100"<<std::endl;
	}

	int expectedScale = C125_SCALE_OPERATION_100;
	if(range > C125_MEMROY_MAX_RANGE_IN_100_MODE ){
		return ERR_TARGET_OUT_OF_RANGE;
	} else if( (range <= C125_MEMROY_MAX_RANGE_IN_100_MODE) && (range> C125_MEMROY_MAX_RANGE_IN_80_MODE)){
		std::cout<<"Expected System scale is in : 100"<<std::endl;
		expectedScale = C125_SCALE_OPERATION_80;
	} else if( (range <= C125_MEMROY_MAX_RANGE_IN_80_MODE) && (range> C125_MEMROY_MAX_RANGE_IN_55_MODE)){
		std::cout<<"Expected System scale is in : 80"<<std::endl;
		expectedScale = C125_SCALE_OPERATION_80;
	} else if( (range <= C125_MEMROY_MAX_RANGE_IN_55_MODE) && (range> C125_MEMROY_MAX_RANGE_IN_40_MODE)){
		std::cout<<"Expected System scale is in : 55"<<std::endl;
		expectedScale = C125_SCALE_OPERATION_55;
	} else{
		std::cout<<"Expected System scale is in : 40"<<std::endl;
		expectedScale = C125_SCALE_OPERATION_40;
	}
	///Compare current and expect scale
	if( expectedScale > currentScale ){
		///Change the c125 to the expected scale
		returnCode = setScaleMode(expectedScale);
	}

	return returnCode;
}

std::string C125Communicator::getNoiseMode(int type){
	std::string rs = "-";
	switch (type) {
	case (int) SimulationTrack::NET_KHONGNHIEU:
{
		rs = "-";
		break;
}
	case (int) SimulationTrack::PRS_DANCHONGRD:
{
		rs = "PRS";
		break;
}
	case (int) SimulationTrack::ASHP_NHIEUTAPTICHCUC:
{
		rs = "ASHP";
		break;
}
	case (int) SimulationTrack::NIP_NHIEUNHAPNHAY:
{
		rs = "NIP";
		break;
}
	case (int) SimulationTrack::UVOD_D_NHIEUXUNGDANTHEOCULI:
{
		rs = "UVOD_D";
		break;
}
	case (int) SimulationTrack::UVOD_F_NHIEUXUNGDANTHEOGOC:
{
		rs = "UVOD_F";
		break;
}
	case (int) SimulationTrack::UVOD_DF_NHIEUXUNGDANTHEOCLIGOC:
{
		rs = "UVOD_DF";
		break;
}
	case (int) SimulationTrack::PP_NHIEUTIEUCUC:
{
		rs = "PP";
		break;
}
	case (int) SimulationTrack::MERC_NHIEUTICHCUCKHONGDONGBO:
{
		rs = "MERC";
		break;
}
	default:
	{
		break;
	}
	}
	//	std::cout<<"Dang hoat dong " << type << " is " << rs << std::endl;
	return rs;
}

int C125Communicator::getNoiseMode(std::string typeStr){
	int rs = (int) SimulationTrack::NET_KHONGNHIEU;
	if (!strncmp(typeStr.c_str(), string("-").c_str(), strlen(typeStr.c_str()))) {
		;
	} else if (!strncmp(typeStr.c_str(), string("PRS").c_str(), strlen(typeStr.c_str()))) {
		rs = (int) SimulationTrack::PRS_DANCHONGRD;
	} else if (!strncmp(typeStr.c_str(), string("ASHP").c_str(), strlen(typeStr.c_str()))) {
		rs = (int) SimulationTrack::ASHP_NHIEUTAPTICHCUC;
	} else if (!strncmp(typeStr.c_str(), string("NIP").c_str(), strlen(typeStr.c_str()))) {
		rs = (int) SimulationTrack::NIP_NHIEUNHAPNHAY;
	} else if (!strncmp(typeStr.c_str(), string("UVOD_D").c_str(), strlen(typeStr.c_str()))) {
		rs = (int) SimulationTrack::UVOD_D_NHIEUXUNGDANTHEOCULI;
	} else if (!strncmp(typeStr.c_str(), string("UVOD_F").c_str(), strlen(typeStr.c_str()))) {
		rs = (int) SimulationTrack::UVOD_F_NHIEUXUNGDANTHEOGOC;
	} else if (!strncmp(typeStr.c_str(), string("UVOD_DF").c_str(), strlen(typeStr.c_str()))) {
		rs = (int) SimulationTrack::UVOD_DF_NHIEUXUNGDANTHEOCLIGOC;
	} else if (!strncmp(typeStr.c_str(), string("PP").c_str(), strlen(typeStr.c_str()))) {
		rs = (int) SimulationTrack::PP_NHIEUTIEUCUC;
	} else if (!strncmp(typeStr.c_str(), string("MERC").c_str(), strlen(typeStr.c_str()))) {
		rs = (int) SimulationTrack::MERC_NHIEUTICHCUCKHONGDONGBO;
	} else {
		;
	}
	//	std::cout<<"Dang hoat dong " << type << " is " << rs << std::endl;
	return rs;
}

std::string C125Communicator::generateTrackSimulationData(bool isDefaultTrackData, int trackNum, TrackSimulationData dataTrack) {
	char tmpRS[255];
	sprintf(tmpRS, "%s %02i\n", mTrackDataDefaultWithoutTrackNumber.c_str(), (int) trackNum);
	if (isDefaultTrackData) {
		;
	} else {
		sprintf(tmpRS, "  %1i  %4i  %5.1f  %5.1f  %5.1f  %4.1f  %4.1f  %4.1f  %i  %i  %i  %i  %i  %i  %s %02i\n",
				dataTrack.dangcodong,
				dataTrack.tocdo0ms,
				dataTrack.culi0kmx10/10.0,
				dataTrack.thamsoduongbay0kmx10/10.0,
				dataTrack.phuongvi0dox10/10.0,
				dataTrack.hesoquataix10/10.0,
				dataTrack.docao0kmx10/10.0,
				dataTrack.docaokkmx10/10.0,
				dataTrack.goccodongdo,
				dataTrack.thoidiemcodongs,
				dataTrack.thoidiemphatnhieus,
				dataTrack.thoidiemphats,
				dataTrack.thoidiemkethucs,
				dataTrack.dangmuctieu,
				getNoiseMode(dataTrack.dangnhieu).c_str(),
				(int) trackNum);
	}
	string rs(tmpRS, sizeof(tmpRS));
	return rs;
}

int C125Communicator::setTrackSimulationToIFile(TrackSimulation trackSimulation){
	int returnCode = ERR_SUCCESS;
	FILE *fOriginal, *fNew;
	unsigned int iTrackCount = trackSimulation.trackDataSize;
	std::cout << "Request to make the simulation for track(s) " << iTrackCount << " at Less " << trackSimulation.numberLession << endl;

	string enterStr = "\n";
	string fullFileName = SystemComponentDetector::getUk42SimFilePath();
	if (fullFileName == string("")) {
		std::cout << "Can not get Uk42 sim file Path" << std::endl;
		return ERR_CANNOT_FIND_FILE;
	} else {
		std::cout << "Uk42 sim file Path: " << fullFileName << ".txt" << std::endl;
	}

	string fullFileNameBackup = fullFileName + string("_bk.txt");
	string fullFileNameBackupOrg = fullFileName + string("_org.txt");
	fullFileName.append(".txt");

	string cmdBackupOrg = "cp -X " + fullFileName + " " + fullFileNameBackupOrg; //_ No copy if exist file
	string cmdBackup = "mv -f " + fullFileName + " " + fullFileNameBackup;
	string cmdRestore = "mv -f " + fullFileNameBackup + " " + fullFileName;

	//_ Make the back up of current file at the first time
	fOriginal = fopen(fullFileName.c_str(), "r");
	if (fOriginal != NULL) {
		system(cmdBackupOrg.c_str());
		system(cmdBackup.c_str());
		fclose(fOriginal);
	} else {
		returnCode = ERR_READ_FAILED;
		return returnCode;
	}

	//_ Next, start copy new track data
	fOriginal = fopen(fullFileNameBackup.c_str(), "r");
	fNew = fopen(fullFileName.c_str(), "w");
	if ( (fOriginal != NULL) && (fNew != NULL)){
		char buf[255] = {0x0};
		string nameLess = mStartLession;
		ostringstream stream;
		stream << trackSimulation.numberLession;
		nameLess.append(stream.str());

		//_ Set 8 targets as data
		bool isSetAllTrackInLession = (iTrackCount < 1) || (iTrackCount > C125_MAX_SIMULATION_TRACK_IN_LESSON) || (iTrackCount == EFFECT_ALL_TARGET_IN_LESSION);

		while (fgets(buf, sizeof(buf), fOriginal) != NULL){
			if (!strncmp(buf, nameLess.c_str(), 3u)) {
				if (ferror(fNew)) {
					returnCode = ERR_WRITE_FAILED;
					system(cmdRestore.c_str());
					fclose(fOriginal);
					fclose(fNew);
					return returnCode;
				} else {
					;
				}

				fputs(buf, fNew);
				for (unsigned int i = 0; i < C125_MAX_SIMULATION_TRACK_IN_LESSON; i++) {
					string trackDataString;
					if (fgets(buf, sizeof(buf), fOriginal) != NULL) {
						if (isSetAllTrackInLession) {
							trackDataString = generateTrackSimulationData(false, i + 1, trackSimulation.trackData[i]);
							fputs(trackDataString.c_str(), fNew);
						} else {
							if (i == iTrackCount - 1 ) {
								trackDataString = generateTrackSimulationData(false, i + 1, trackSimulation.trackData[i]);
								fputs(trackDataString.c_str(), fNew);
							} else {
								fputs(buf, fNew);
							}
						}
					}

					if (ferror(fNew)) {
						returnCode = ERR_WRITE_FAILED;
						system(cmdRestore.c_str());
						fclose(fOriginal);
						fclose(fNew);
						return returnCode;
					} else {
						;
					}
				}
			} else {
				fputs(buf, fNew);
				if (ferror(fNew)) {
					returnCode = ERR_WRITE_FAILED;
					system(cmdRestore.c_str());
					fclose(fOriginal);
					fclose(fNew);
					return returnCode;
				} else {
					;
				}
			}
		}
		fclose(fOriginal);
		fclose(fNew);
	} else {
		returnCode = ERR_READ_FAILED;
		return returnCode;
	}
	return returnCode;
}

int C125Communicator::readTrackSimulationFromIFile(TrackSimulation &trackSimulation){
	int iLession = trackSimulation.numberLession;
	std::cout << "Request to read the simulation for lession " << iLession << endl;
	int returnCode = ERR_SUCCESS;
	string fullFileName = SystemComponentDetector::getUk42SimFilePath();
	if (fullFileName == string("")) {
		std::cout << "Can not get Uk42 sim file Path" << std::endl;
		return ERR_CANNOT_FIND_FILE;
	} else {
		std::cout << "Uk42 sim file Path: " << fullFileName << ".txt" << std::endl;
	}

	fullFileName.append(".txt");
	FILE *fp;
	fp = fopen(fullFileName.c_str(), "r");
	if (fp != NULL) {
		char buf[255] = {0x0};
		string nameLess = mStartLession;
		ostringstream stream;
		stream << iLession;
		nameLess.append(stream.str());
		while (fgets(buf, sizeof(buf), fp) != NULL){
			//			std::cout << "now READ buf " << buf << endl;
			if (!strncmp(buf, nameLess.c_str(), 3u)) {
				for (unsigned int i = 0; i < C125_MAX_SIMULATION_TRACK_IN_LESSON; i++) {
					fgets(buf, sizeof(buf), fp);
					const char* bufTok;
					unsigned int iCountCol = 0;
					for ( bufTok = strtok(buf, " "); bufTok; bufTok = strtok(0, " ") )
					{
						switch(iCountCol) {
						case 0:
						{
							trackSimulation.trackData[i].dangcodong = strtod(bufTok, NULL);
							break;
						}
						case 1:
						{
							trackSimulation.trackData[i].tocdo0ms = strtod(bufTok, NULL);
							break;
						}
						case 2:
						{
							trackSimulation.trackData[i].culi0kmx10 = 10*strtod(bufTok, NULL);
							break;
						}
						case 3:
						{
							trackSimulation.trackData[i].thamsoduongbay0kmx10 = 10*strtod(bufTok, NULL);
							break;
						}
						case 4:
						{
							trackSimulation.trackData[i].phuongvi0dox10 = 10*strtod(bufTok, NULL);
							break;
						}
						case 5:
						{
							trackSimulation.trackData[i].hesoquataix10 = 10*strtod(bufTok, NULL);
							break;
						}
						case 6:
						{
							trackSimulation.trackData[i].docao0kmx10 = 10*strtod(bufTok, NULL);
							break;
						}
						case 7:
						{
							trackSimulation.trackData[i].docaokkmx10 = 10*strtod(bufTok, NULL);
							break;
						}
						case 8:
						{
							trackSimulation.trackData[i].goccodongdo = strtod(bufTok, NULL);
							break;
						}
						case 9:
						{
							trackSimulation.trackData[i].thoidiemcodongs = strtod(bufTok, NULL);
							break;
						}
						case 10:
						{
							trackSimulation.trackData[i].thoidiemphatnhieus = strtod(bufTok, NULL);
							break;
						}
						case 11:
						{
							trackSimulation.trackData[i].thoidiemphats = strtod(bufTok, NULL);
							break;
						}
						case 12:
						{
							trackSimulation.trackData[i].thoidiemkethucs = strtod(bufTok, NULL);
							break;
						}
						case 13:
						{
							trackSimulation.trackData[i].dangmuctieu = strtod(bufTok, NULL);
							break;
						}
						case 14:
						{
							int iDangnhieu = getNoiseMode(bufTok);
							trackSimulation.trackData[i].dangnhieu = iDangnhieu;
							break;
						}
						case 15:
						{
							break;
						}
						default:
							break;
						}
						iCountCol++;
					}
				}
				break;
			} else {
				continue;
			}
		}
		fclose(fp);
	} else {
		returnCode = ERR_READ_FAILED;
	}
	return returnCode;
}

