/*
 * DhlCommunicator.cpp
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */


// change by Minh on send detected target

#include "DhlCommunicator.h"

mqd_t DhlCommunicator::mqfd1;
mqd_t DhlCommunicator::target_mq_rece;

DhlCommunicator::DhlCommunicator() {
	// TODO Auto-generated constructor stub
	pthread_mutex_init(&mWriteMessageLocker,NULL);
	pthread_mutex_init(&mReadMessageLocker,NULL);

	mSocketId = -1;
	mConnectionId = -1;

	mPort = SystemConfigurations::getInstance()->getPort();
}

DhlCommunicator::~DhlCommunicator() {
	// TODO Auto-generated destructor stub
	close(mSocketId);
	close(mConnectionId);
}

void DhlCommunicator::join(){
	pthread_join(mListeningConnectionThread,NULL);
	pthread_join(mDataTranssasionThread,NULL);
	pthread_join(mDataReceivingThread,NULL);
}

void DhlCommunicator::run(){
	pthread_create(&mListeningConnectionThread,NULL,DhlCommunicator::listening,this);
	pthread_create(&mDataTranssasionThread,NULL,DhlCommunicator::dataTransmiting,this);
	pthread_create(&mDataReceivingThread,NULL,DhlCommunicator::dataReceiving,this);
}

void *DhlCommunicator::listening(void *args)
{
	DhlCommunicator *communicator = (DhlCommunicator*)args;

	communicator->mSocketId = -1;
	communicator->mConnectionId= -1;

//	communicator->mHostIp = SystemConfigurations::getInstance()->getHostIp();
	communicator->mPort = SystemConfigurations::getInstance()->getPort();


	int socketId;
	socketId = inet_listen(communicator->mPort.c_str(),0,NULL);
	std::cout<<"Waiting Connection in "<<communicator->mPort<<" Socket Id: "<<socketId<<std::endl;
	if( socketId == -1 ){
		std::cout<<"Cannot listen on the port "<<communicator->mPort<<std::endl;
		return NULL;
	}
	pthread_mutex_lock(&communicator->mConnectionLocker);
	if( communicator->mSocketId >= 0 ){
		close(communicator->mSocketId);
	}
	communicator->mSocketId = socketId;
	pthread_mutex_unlock(&communicator->mConnectionLocker);
	char s[INET6_ADDRSTRLEN];
	struct sockaddr_storage their_address; // client information
	socklen_t sin_size = sizeof(their_address);

	while(1){
		int connectionFd = -1;
		connectionFd = accept(communicator->mSocketId, (struct sockaddr*)&their_address, &sin_size); // block here!
		if( connectionFd == -1 ){
			std::cout<<"Cannot listen on the port: "<<communicator->mPort<<" socket Id "<<communicator->mSocketId<<std::endl;
			sleep(1);
			continue;
		}
		inet_ntop(their_address.ss_family, get_in_addr((struct sockaddr *)&their_address), s, sizeof (s));

		std::cout<<"Got connection from IP: "<< s<<" on port:"<< communicator->mPort<<std::endl;

		///Close current Connection
		pthread_mutex_lock(&communicator->mConnectionLocker);
		if( communicator->mConnectionId >= 0 ){
			close(communicator->mConnectionId);
		}
		usleep(200);
		communicator->mConnectionId = connectionFd;
		pthread_mutex_unlock(&communicator->mConnectionLocker);
	}
	return NULL;
}
void *DhlCommunicator::dataReceiving(void *args){
	DhlCommunicator *communicator = (DhlCommunicator*)args;
	while(1){
		if(communicator->mConnectionId >0 && communicator->mSocketId > 0 && communicator->mReceivedMessageQueue.isFull() == false ){
			///Connection established
			C125Message receivedMessage;
			unsigned int receivedSize = 0;
			receivedSize = read(communicator->mConnectionId,&receivedMessage,sizeof(C125Message));
			if( receivedSize < 0){
				///Connection is down
				std::cout<<"Received Failed, close connection  message size: "<<receivedSize<<std::endl;
				pthread_mutex_lock(&communicator->mConnectionLocker);
				close(communicator->mConnectionId);
				communicator->mConnectionId = -1;
				pthread_mutex_unlock(&communicator->mConnectionLocker);
				continue;
			} else if(receivedSize== sizeof(C125Message)){
				pthread_mutex_lock(&communicator->mReadMessageLocker);
				string temp_s;
				temp_s.assign(receivedMessage.msgHeader, 4);
				if(temp_s == C125_CONTROL_HEADER) {
					communicator->mReceivedMessageQueue.push(receivedMessage);
					std::cout<<"Received one message from client with byte(s)" << sizeof(receivedMessage) << std::endl;
				} else {
					std::cout<<"Received one message from client with invalid header "<< receivedMessage.msgHeader <<std::endl;
				}
				pthread_mutex_unlock(&communicator->mReadMessageLocker);
			}// test mode
			else if(receivedSize==1){
				if (receivedMessage.msgHeader[0] == 'a') {
					ExTarget::getInstance()->setMode(EXSTART);
					ExTarget::getInstance()->resetMap();
					std::cout<<"enter start mode"<<endl;
				}
				if (receivedMessage.msgHeader[0] == 'b') {
					ExTarget::getInstance()->setMode(EXSTOP);
					ExTarget::getInstance()->resetMap();
				}
				if (receivedMessage.msgHeader[0] == 'c') {
					ExTarget::getInstance()->setMode(EXHISTORY);
					for(int i=0;i<ExTarget::getInstance()->getHistory().size;i++){
					ExTarget::getInstance()->setMap(ExTarget::getInstance()->getHistory().pos[i].x,
													ExTarget::getInstance()->getHistory().pos[i].y,
													ExTarget::getInstance()->getHistory().pos[i].value,
													EX_PLOT_SIZE);
//						exHistory his= ExTarget::getInstance()->getHistory();
//						ExTarget::getInstance()->setMap(3000,75,254,10);
					}
					std::cout<<"enter history mode"<<endl;
				}
			}
			else {
				//				std::cout<<"Received Failed,  message size: "<<receivedSize<<std::endl;
				usleep(100);
			}
		} else {
			usleep(100);
		}
	}
	return NULL;
}

void *DhlCommunicator::dataTransmiting(void *args)
{
	static int c_q;
	struct mq_attr attr;
	attr.mq_maxmsg = 10;
	struct timespec tm;
	attr.mq_msgsize = sizeof(ukDataSend);
	DhlCommunicator *communicator = (DhlCommunicator*)args;
	mqfd1 = mq_open("/queue1", O_RDWR|O_CREAT|O_NONBLOCK, 0777, &attr);
	if(mqfd1 == -1) {
		printf("Parent mq_open failure");
		exit(0);
	}
	///////////////////////////////////////////////////////////////////////////Edit by Minh
	attr.mq_maxmsg=100;
	attr.mq_msgsize=CAT48_LEN;
/////////////////////////////////////////
	target_mq_rece= mq_open("/target_mq1",O_RDWR|O_CREAT|O_NONBLOCK, 0777, &attr);
	if(target_mq_rece==-1){
		printf("open target_mq_rece error");
		exit(0);
	}

	ukDataSend ukData;
	char* data_cat48= new char[CAT48_LEN];
	while(1){
		if(communicator->mConnectionId >0 && communicator->mSocketId > 0 && (communicator->mRespondMessageQueue.isEmpty()==false)){
			///Connection established
			C125Message messageToSend;
			pthread_mutex_lock(&communicator->mWriteMessageLocker);
			int returnCode = communicator->mRespondMessageQueue.pop(messageToSend);
			pthread_mutex_unlock(&communicator->mWriteMessageLocker);
			if(ERR_SUCCESS == returnCode )
			{
				int sendLength = 0;
				sendLength = send(communicator->mConnectionId,&messageToSend,sizeof(C125Message),MSG_NOSIGNAL);
				if(sendLength <= 0 ){
					///Send message Failed, close connection
					std::cout<<"Transmit Failed, close connection"<<std::endl;
					pthread_mutex_lock(&communicator->mConnectionLocker);
					close(communicator->mConnectionId);
					communicator->mConnectionId = -1;
					pthread_mutex_unlock(&communicator->mConnectionLocker);
					continue;
				}
			}
		}
//edit by Minh
//		else if(communicator->mConnectionId >0 && communicator->mSocketId > 0 && (communicator->mRespondMessageQueue.isEmpty()==true))
		else {
			clock_gettime(CLOCK_REALTIME,&tm);
			tm.tv_nsec+=50;

			int status= mq_timedreceive(target_mq_rece,data_cat48,CAT48_LEN,0,&tm);
			if(status!=-1){
//				if(data_cat48[3]==34)printf(" Circle End\n");
//				printf("\n\nReceive %d    %d\n\n",++c_q,status);
				int cat48_len;
				if(communicator->mConnectionId>0&& communicator->mSocketId>0){
					cout<<"send target"<<endl;
					cat48_len=send(communicator->mConnectionId,data_cat48,CAT48_LEN,MSG_NOSIGNAL);
					if (cat48_len <= 0) {
						printf("send target failure\n\n\n");
						pthread_mutex_lock(&communicator->mConnectionLocker);
						close(communicator->mConnectionId);
						communicator->mConnectionId = -1;
						pthread_mutex_unlock(&communicator->mConnectionLocker);
						continue;
					}
					else{
						pthread_mutex_lock(&cat48_mutex);
						uint16_t range = data_cat48[15] *256+data_cat48[16];
						uint16_t bear = data_cat48[17] *256+data_cat48[18];
						float r = (float) range * 1852 / 256;
						float b = (float) bear * 360 / 65536;
						for(int i=0;i<4;i++)printf("%hhx\t",data_cat48[15+i]);
						printf("After  %d  %f   %f\n",data_cat48[0], r, b);
						printf("send target success\n\n\n");
						pthread_mutex_unlock(&cat48_mutex);
						usleep(100000);
					}
				}

				continue;
			}
		}
//////////////end
//		if(communicator->mConnectionId >0 && communicator->mSocketId > 0)
		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_nsec += 50;
		int status = mq_timedreceive(mqfd1, (char*) &ukData,
				sizeof(ukData) + 1, 0, &tm);
		if (status == -1) {
			//				cout << "mq_receive failure" << mqfd1 <<endl; // Chuyen luong doc queue sang thread khac
			//				usleep(50000);
		} else {
			//				printf("mq_receive successful\n");
			int sendLength = 0;
			if (communicator->mConnectionId > 0 && communicator->mSocketId > 0) {
				cout << "data send" << endl;
				sendLength = send(communicator->mConnectionId, &ukData,
						sizeof(ukDataSend), MSG_NOSIGNAL);
				if (sendLength <= 0) {
					pthread_mutex_lock(&communicator->mConnectionLocker);
					close(communicator->mConnectionId);
					communicator->mConnectionId = -1;
					pthread_mutex_unlock(&communicator->mConnectionLocker);
					continue;
				}
			}
		}

	}
	return NULL;
}

int DhlCommunicator::getReceivedMessage(C125Message &msg)
{
	pthread_mutex_lock(&mReadMessageLocker);
	int returnCode = mReceivedMessageQueue.pop(msg);
	pthread_mutex_unlock(&mReadMessageLocker);

	return returnCode;
}
int DhlCommunicator::transmitMessage(const C125Message &msg){
	pthread_mutex_lock(&mWriteMessageLocker);
	int returnCode = mRespondMessageQueue.push(msg);
	pthread_mutex_unlock(&mWriteMessageLocker);

	return returnCode;
}
