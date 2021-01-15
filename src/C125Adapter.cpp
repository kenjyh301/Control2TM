//============================================================================
// Name        : C125Adapter.cpp
// Author      : hungnv103
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "C125Adapter.h"

int main() {

	delete_queue();
	SystemConfigurations::getInstance()->init(CONFIGURE_FILE);
	C125DataProcessor processor;
	MonitorProcessor c125Monitor;

	setMutex(processor, c125Monitor);

	processor.run();
	c125Monitor.run();


	//edit by Minh
	pthread_create(&tcp_targer_thread,NULL,mTargetServerConnect,(void*)targetServer);
	pthread_create(&data_thread,NULL,mdataRead,NULL);
	pthread_create(&detect_thread,NULL,mDetectTarget,NULL);
	pthread_create(&send_thread,NULL,mSendTarget,NULL);

	while(1){
		sleep(100);
	}
	return 0;
}
