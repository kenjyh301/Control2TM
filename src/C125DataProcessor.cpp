/*
 * C125DataProcessor.cpp
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */

#include "C125DataProcessor.h"

C125Communicator C125DataProcessor::mC125Communicator;
DhlCommunicator C125DataProcessor::mDhlComunicator;

C125DataProcessor::C125DataProcessor() {
	// TODO Auto-generated constructor stub

}

C125DataProcessor::~C125DataProcessor() {
	// TODO Auto-generated destructor stub
}


void *C125DataProcessor::communicatorThreadRun(void *argc){
		mDhlComunicator.run();
		SystemComponentDetector::run();
		C125Message receivedMessage;
		std::cout << "size of C125Message: " << sizeof(C125Message) <<std::endl;
		while(1 ){
			if(mDhlComunicator.getReceivedMessage(receivedMessage) == ERR_SUCCESS){
				switch( receivedMessage.msgOpcode ){
				case REQUEST_ASSIGNED_TARGET:
				{
					int returnCode = mC125Communicator.assignTarget(receivedMessage.msgTrackInfo.bear,receivedMessage.msgTrackInfo.range);
					std::cout<<"Received Assigned target bear: "<< std::dec <<receivedMessage.msgTrackInfo.bear<<"  rage: "<<receivedMessage.msgTrackInfo.range<<std::endl;
					if(returnCode == ERR_SUCCESS ){
						receivedMessage.msgOpcode = RESPOND_ASSIGNED_DONE;
					} else {
						receivedMessage.msgOpcode = RESPOND_ASSIGNED_FAILE;
						mC125Communicator.setLabelDefault();
					}
					receivedMessage.msgC125Status.status = returnCode;
					receivedMessage.length = sizeof(receivedMessage);
					std::cout<<"Received REQUEST_ASSIGNED_TARGET from dhl. Respond "<< std::dec<<receivedMessage.msgOpcode<<"  status: "<<std::dec<<receivedMessage.msgC125Status.status<<std::endl;
					mDhlComunicator.transmitMessage(receivedMessage);
					break;
				}
				case REQUEST_DEASSIGNED_TARGET:
				{
					int returnCode = mC125Communicator.deassignTarget();
					if(ERR_SUCCESS == returnCode ){
						receivedMessage.msgOpcode = RESPOND_DEASSIGNED_DONE;
						mC125Communicator.setLabelDefault();
					} else {
						receivedMessage.msgOpcode = RESPOND_DEASSIGNED_FAILED;
					}
					receivedMessage.msgC125Status.status = returnCode;
					receivedMessage.length = sizeof(receivedMessage);
					mDhlComunicator.transmitMessage(receivedMessage);
					std::cout<<"Received REQUEST_DEASSIGNED_TARGET from dhl. Respond "<< std::dec <<receivedMessage.msgOpcode<<"  status: "<<std::hex<<receivedMessage.msgC125Status.status<<std::endl;
					break;
				}
				case REQUEST_SYSTEM_STATUS:
				{
					int returnCode = mC125Communicator.readSystemStatus();
					receivedMessage.msgOpcode = RESPOND_REQUEST_STATUS;
					receivedMessage.msgC125Status.status = returnCode;
					receivedMessage.length = sizeof(receivedMessage);
					std::cout<<"Received REQUEST_SYSTEM_STATUS from dhl. Respond "<<receivedMessage.msgOpcode<<"  status: "<<std::dec<<receivedMessage.msgC125Status.status<<std::endl;
					mDhlComunicator.transmitMessage(receivedMessage);
					break;
				}
				case REQUEST_TRACK_UPDATE:
				{
					int returnCode = mC125Communicator.requestUpdateTrack(receivedMessage.msgTrackInfo.bear,receivedMessage.msgTrackInfo.range);
					mC125Communicator.setATAInfo(receivedMessage.msgTrackInfo.bear, receivedMessage.msgTrackInfo.range, receivedMessage.msgTrackInfo.high, receivedMessage.msgTrackInfo.velocity);
					std::cout<<"Received Request re-update target bear: "<<receivedMessage.msgTrackInfo.bear<<"  rage: "<<receivedMessage.msgTrackInfo.range<<std::endl;
					break;
				}
				case REQUEST_TRACK_SIMULATION:
				{
					std::cout<<"Received request to make simulation with size " << sizeof(receivedMessage) << std::endl;
					int returnCode = mC125Communicator.setTrackSimulationToIFile(receivedMessage.msgTrackSimulation);
					receivedMessage.msgOpcode = RESPOND_TRACK_SIMULATION;
					receivedMessage.msgC125Status.status = returnCode;
					receivedMessage.length = sizeof(receivedMessage);
					std::cout<<"Received REQUEST_TRACK_SIMULATION from dhl. Respond "<<receivedMessage.msgOpcode<<"  status: "<<std::dec<<receivedMessage.msgC125Status.status<<std::endl;
					mDhlComunicator.transmitMessage(receivedMessage);
					break;
				}
				case REQUEST_UK42_LESSION:
				{
					std::cout<<"Received request to get I sim data with lesson " << receivedMessage.msgTrackSimulation.numberLession << std::endl;
					int returnCode = mC125Communicator.readTrackSimulationFromIFile(receivedMessage.msgTrackSimulation);
					receivedMessage.msgOpcode = RESPOND_UK42_LESSION;
					receivedMessage.msgC125Status.status = returnCode;
					receivedMessage.length = sizeof(receivedMessage);
					std::cout<<"Received RESPOND_UK42_LESSION from dhl. Respond "<<receivedMessage.msgOpcode<<"  status: "<<std::dec<<receivedMessage.msgC125Status.status<<std::endl;
					mDhlComunicator.transmitMessage(receivedMessage);
					break;
				}

				default:
					///Do request system status
					std::cout<<"Received message with invalid opcode from dhl:  "<<receivedMessage.msgOpcode<<std::endl;
					break;
				}
			} else {
				//			int returnCode = mC125Communicator.readSystemStatus();
				//			std::cout<<"Received REQUEST_SYSTEM_STATUS from dhl. Respond "<<REQUEST_SYSTEM_STATUS<<"  status: "<<returnCode<<std::endl;
				usleep(10000);
			}
		}
}

void C125DataProcessor::run()
{
	pthread_create(&communicatorThread, NULL, communicatorThreadRun, NULL);
}
