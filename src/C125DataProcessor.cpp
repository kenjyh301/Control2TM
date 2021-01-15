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
//		SystemComponentDetector::run();
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

					receivedMessage.msgTrackSimulation.trackDataSize=8;
					mDhlComunicator.transmitMessage(receivedMessage);
					break;
				}

				case REQUEST_EXTARGET_START:
				{
					ExTarget::getInstance()->setMode(EXSTART);
					ExTarget::getInstance()->resetMap();
					ExTarget::getInstance()->resetHistory();
					receivedMessage.msgOpcode = RESPOND_EXTARGET_START_DONE;
					receivedMessage.msgC125Status.status = ERR_SUCCESS;
					receivedMessage.length = sizeof(receivedMessage);
					std::cout<<"ExTarget Mode Start"<<endl;
					mDhlComunicator.transmitMessage(receivedMessage);
					break;
				}
				case REQUEST_EXTARGET_STOP:
				{
					ExTarget::getInstance()->setMode(EXSTOP);
					ExTarget::getInstance()->resetMap();
					receivedMessage.msgOpcode = RESPOND_EXTARGET_STOP_DONE;
					receivedMessage.msgC125Status.status = ERR_SUCCESS;
					receivedMessage.length = sizeof(receivedMessage);
					std::cout<<"ExTarget Mode Stop"<<endl;
					mDhlComunicator.transmitMessage(receivedMessage);
					break;
				}
				case REQUEST_EXTARGET_NORMAL:
				{
					if (ExTarget::getInstance()->getMode() != EXSTART) {
						ExTarget::getInstance()->setMode(EXSTART);
						ExTarget::getInstance()->resetMap();
					}
					int x=0,y=0;
					x=(int)(receivedMessage.msgTrackInfo.bear/360.0*(MAP_AZI_MAX));
					y=(int)receivedMessage.msgTrackInfo.range;
					int ID= (int) receivedMessage.msgUUID;
//					cout<<x<<" "<<y<<endl;
					ExTarget::getInstance()->addHistory(x,y,NORMAL_COLOR,ID);
					ExTarget::getInstance()->setMap(x,y,NORMAL_COLOR,EX_VIDEO_SIZE);

					receivedMessage.msgOpcode = RESPOND_EXTARGET_NORMAL;
					receivedMessage.msgC125Status.status = ERR_SUCCESS;
					receivedMessage.length = sizeof(receivedMessage);
					std::cout<<"ExTarget Receive Normal"<<endl;
					mDhlComunicator.transmitMessage(receivedMessage);
					break;
				}
				case REQUEST_EXTARGET_IFF:
				{
					if(ExTarget::getInstance()->getMode()!=EXSTART){
						ExTarget::getInstance()->setMode(EXSTART);
						ExTarget::getInstance()->resetMap();
					}
					int x = 0, y = 0;
					x = receivedMessage.msgTrackInfo.bear / 360.0 * (MAP_AZI_MAX);
					y = receivedMessage.msgTrackInfo.range;
					int ID= (int) receivedMessage.msgUUID;
					ExTarget::getInstance()->addHistory(x,y,IFF_COLOR,ID);
					ExTarget::getInstance()->setMap(x,y,IFF_COLOR,EX_VIDEO_SIZE);

					receivedMessage.msgOpcode = RESPOND_EXTARGET_IFF;
					receivedMessage.msgC125Status.status = ERR_SUCCESS;
					receivedMessage.length = sizeof(receivedMessage);
					std::cout<<"ExTarget Receive IFF"<<endl;
					mDhlComunicator.transmitMessage(receivedMessage);
					break;
				}
				case REQUEST_EXTARGET_HISTORY:
				{
					ExTarget::getInstance()->setMode(EXHISTORY);
					int ID= receivedMessage.msgUUID;
					ExTarget::getInstance()->resetMap();
					for(int i=0;i<ExTarget::getInstance()->getHistory().size;i++){
						if(ExTarget::getInstance()->getHistory().pos[i].ID==ID||ID==0){
							ExTarget::getInstance()->setMap(ExTarget::getInstance()->getHistory().pos[i].x,
														ExTarget::getInstance()->getHistory().pos[i].y,
														ExTarget::getInstance()->getHistory().pos[i].value,
														EX_PLOT_SIZE);
						}
//						exHistory his= ExTarget::getInstance()->getHistory();
//						ExTarget::getInstance()->setMap(3000,75,254,10);
					}
					cout<<"Set map ok"<<endl;
					receivedMessage.msgOpcode = RESPOND_EXTARGET_HISTORY_DONE;
					receivedMessage.msgC125Status.status = ERR_SUCCESS;
					receivedMessage.length = sizeof(receivedMessage);
					std::cout<<"ExTarget Show History"<<endl;
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
