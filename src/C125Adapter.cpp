//============================================================================
// Name        : C125Adapter.cpp
// Author      : hungnv103
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "C125Adapter.h"

void creat_fake_data(){
	int x=1000/EX_AZD;
	int y=120;
	for(int i=0;i<80;i++){
		ExTarget::getInstance()->addHistory(x,y,IFF_COLOR,10);
		ExTarget::getInstance()->setMap(x, y, IFF_COLOR,EX_VIDEO_SIZE);
		x+=50/EX_AZD;
		y-=2;
		if(x>1500)x=1500;
		if(y<50)y=50;
	}
}

int main() {

	delete_queue();
	SystemConfigurations::getInstance()->init(CONFIGURE_FILE);
	ExTarget::getInstance()->init();

	C125DataProcessor processor;
	MonitorProcessor c125Monitor;

	setMutex(processor, c125Monitor);

	processor.run();
//	c125Monitor.run();

	//edit by Minh
	pthread_create(&tcp_targer_thread,NULL,mTargetServerConnect,(void*)targetServer);  // create server to send plot
	pthread_create(&data_thread,NULL,mdataRead,NULL);			// read data from uk51 ps file
	pthread_create(&detect_thread,NULL,mDetectTarget,NULL);		// detect target algorithm using IFF processing
	pthread_create(&send_thread,NULL,mSendTarget,NULL);			// send target through tcp IP and send target to target_mq

//	creat_fake_data();
	ExTarget::getInstance()->ExEnable(true);// enable on c125Monitor when run all
	ExTarget::getInstance()->setMode(EXHISTORY);


//	pthread_create(&draw_thread,NULL,mDrawRadar,NULL);

/////////////////////////////
	while(1){
		sleep(100);
	}
	return 0;

}
