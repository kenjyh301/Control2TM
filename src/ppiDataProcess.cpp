/*
 * ppi51DataProcess.cpp
 *
 *  Created on: Oct 26, 2020
 *      Author: Minh
 */

#include "ppiDataProcess.h"

/* Note:
 * dataSpec: az - azimuth from 0 to 6000 equivalent to 0 - 360 degree
 * 			 azdd - distance between 2 az
 *
 */


struct dataSpec{
	uint16_t az;
	uint16_t azdd;
	uint16_t line_size;
	uint16_t num_of_line;
	uint16_t az_pre;
	dataSpec(){
		az=0;
		azdd=0;
		line_size=0;
		num_of_line=0;
		az_pre=0;
	}
};

int ppi51_pid = 0;
int ppi51_chid = 0;
int coid=-1;
int nodeid=-1;

const int header_size=20;
bool excerciseMode= false;


clock_t clk = 0, clk_pre = 0;

pthread_mutex_t data_mutex;
screenDataQueue scrQueue;

void get_uk51_nodeid() {
	nodeid = netmgr_strtond("uk51.", NULL);
	printf("uk51 nodeid: %d\n", nodeid);
}

void get_ppi51_pid() {
	char tmp[512];
	FILE* fp = popen("pidin -n uk51. -p ppi51 -F '%a %J %B' | grep RECEIVE",
			"r");
	fscanf(fp, "%d %s %d", &ppi51_pid, tmp, &ppi51_chid);
	printf("ppi51 pid %d, chid %d\n", ppi51_pid, ppi51_chid);
	pclose(fp);
}

void connect_ppi51(){
	coid = ConnectAttach(nodeid, ppi51_pid, ppi51_chid, _NTO_SIDE_CHANNEL, 0);
	printf("coid: %d\n", coid);
}

void get_ppi51_source(char* s) {
	get_uk51_nodeid();
	get_ppi51_pid();
	connect_ppi51();

	if (nodeid <= 0) {
		printf("get uk51 nodeID error\n");
		return;
	}
	if (ppi51_pid <= 0) {
		printf("get ppi51 pid error\n");
		return;
	}
	if (coid <= 0) {
		printf("Connect to uk51 error\n");
		return;
	}


	sprintf(s, "//net//uk51.//proc//%d//as", ppi51_pid);
	printf("data source file:%s\n");
}

void* mdataRead(void*) {
//	int NUM_BYTES= 20 + SystemConfigurations::getInstance()->get_line_size() * SystemConfigurations::getInstance()->get_num_of_line();

	uint8_t* data0 = new uint8_t[NUM_BYTES+2]; // 2 first bytes is AA55
									// next 20 bytes is header: az(4),azdd(4),line_size(4), num_of_line(4), 4bytes reserve
									// data 1350 bytes in case line_size=150 and num_of_line=9
	uint8_t* data1 = new uint8_t[NUM_BYTES+2];
	printf("Num bytes: %d\n",NUM_BYTES);
	char ppi51_as[50];
	get_ppi51_source(ppi51_as);
	FILE* f = fopen(ppi51_as, "r");
	FILE* f1;
	//	FILE* f = fopen("//net//uk51.//proc//581669//as", "r");
	if (f == NULL)
		printf("open ppi51 as error\n");
	else
		printf("open ppi51 as success\n");


	pthread_mutex_init(&data_mutex, NULL);

	dataSpec datSp0;
	dataSpec datSp1;

	data0[0]=0xAA;
	data1[0]=0xAA;
	data0[1]=0x55;
	data1[1]=0x55;

	if(SystemConfigurations::getInstance()->isVideo()){
		pthread_create(&tcp_data_thread,NULL,mTargetServerConnect,(void*)dataServer);

		f1= fopen("//minh_data//write_data","w");
	}

	sleep(10);  //wait for system initialize


	while (1) {
//		pthread_mutex_lock(&data_mutex);
		//do read here
		//		usleep(SAMPLE_TIME-(clock()-clk));
//		printf("%ld\n", clock() - clk);
//		clk = clock();

		if(!excerciseMode) {
			usleep(100);
// comment to develop feature. Uncomment for enabling check ecerciseMode
//			continue;
		}

		memset(data0 + 2, 0, NUM_BYTES);
		memset(data1 + 2, 0, NUM_BYTES);
		fseek(f, (long int) DATA_BASE_0, SEEK_SET);

		fread(data0+2, 20, 1, f);

		datSp0.az = (data0[0+2] | data0[1+2] << 8);
		datSp0.azdd = (data0[4+2] | data0[5+2] << 8);
		datSp0.line_size = (data0[8+2] | data0[9+2] << 8);
		datSp0.num_of_line = (data0[12+2] | data0[13+2] << 8);

		uint8_t* data= new uint8_t[datSp0.line_size];
		int step = (round)((float) datSp0.azdd / datSp0.num_of_line);
		int az=datSp0.az;

		if (datSp0.az != datSp0.az_pre) {
			datSp0.az_pre = datSp0.az;

			if (datSp0.line_size != SystemConfigurations::getInstance()->get_line_size()) {
				printf("Wrong line_size:");
				printf("%d",datSp0.line_size);
				std::exit(0);
			}

			if (datSp0.num_of_line != SystemConfigurations::getInstance()->get_num_of_line()) {
				printf("Wrong num of line %d %d",datSp0.num_of_line,SystemConfigurations::getInstance()->get_num_of_line());
				std::exit(0);
			}

			fread(data0+2+20,NUM_BYTES-20,1,f);// 2 bytes AA55 +20 bytes header
			if(SystemConfigurations::getInstance()->isVideo()){
				write(dataServer->connfd,data0,NUM_BYTES+2);
				fwrite(data0,NUM_BYTES+2,1,f1);
			}





			for (int i = 0; i < datSp0.num_of_line; i++) {
				memcpy(data,data0+2+20+i*datSp0.line_size,datSp0.line_size);
				mTraceData tData(az, step, data);
				scrQueue.push(tData);
//				printf("push suceess\n");
				az += step;
//				printf("SP0  %d\n",az);
			}
		}


		fseek(f, (long int) DATA_BASE_1, SEEK_SET);
		fread(data1 + 2, 20, 1, f);

		datSp1.az = (data1[0 + 2] | data1[1 + 2] << 8);
		datSp1.azdd = (data1[4 + 2] | data1[5 + 2] << 8);
		datSp1.line_size = (data1[8 + 2] | data1[9 + 2] << 8);
		datSp1.num_of_line = (data1[12 + 2] | data1[13 + 2] << 8);

		step = (round)((float) datSp1.azdd / datSp1.num_of_line);
		az = datSp1.az;

		if (datSp1.az != datSp1.az_pre) {
			datSp1.az_pre = datSp1.az;

			if (datSp1.line_size != SystemConfigurations::getInstance()->get_line_size()) {
				printf("Wrong line_size");
				std::exit(0);
			}

			if (datSp1.num_of_line != SystemConfigurations::getInstance()->get_num_of_line()) {
				printf("Wrong num of line");
				std::exit(0);
			}

			fread(data1+2+20, NUM_BYTES - 20, 1, f);

			if(SystemConfigurations::getInstance()->isVideo()){
				write(dataServer->connfd,data1,NUM_BYTES+2);
				fwrite(data1,NUM_BYTES+2,1,f1);
			}


			for (int i = 0; i < datSp1.num_of_line; i++) {
				memcpy(data, data1+2+20 + i * datSp1.line_size, datSp1.line_size);
				mTraceData tData(az, step, data);
				scrQueue.push(tData);
//				printf("push suceess\n");
				az += step;
//				printf("SP1  %d\n",az);
			}
		}

		delete[] data;

//		pthread_mutex_unlock(&data_mutex);
		//		sleep(SLEEP_TIME);
		usleep(SLEEP_TIME);
	}
}

void* mdataRead_from_file(void*) {
//	int NUM_BYTES= 20 + SystemConfigurations::getInstance()->get_line_size() * SystemConfigurations::getInstance()->get_num_of_line();


	uint8_t* data0 = new uint8_t[NUM_BYTES+2]; // 2 first bytes is AA55
										// next 20 bytes is header: az(4),azdd(4),line_size(4), num_of_line(4), 4bytes reserve
										// data 1350 bytes in case line_size=150 and num_of_line=9
	uint8_t* data1 = new uint8_t[NUM_BYTES+2];

	const char* file_name = "//minh_data//log";
	FILE* f = fopen(file_name, "r");
	if (f == NULL)
		printf("open file error\n");
	else
		printf("open file success\n");

	pthread_mutex_init(&data_mutex, NULL);

	uint16_t az = 0;
	uint16_t azdd = 0;
	uint16_t line_size = 0;
	uint16_t num_of_line = 0;
	uint16_t az_pre = -1;

	int count = 0;

	while (1) {
		fread(data0, 2, 1, f);
		if (data0[0] != 0xAA || data0[1] != 0x55)
			continue;
		pthread_mutex_lock(&data_mutex);
		//do read here
		//		usleep(SAMPLE_TIME-(clock()-clk));
		//		printf("%ld\n",clock()-clk);
		//		clk=clock();
		fseek(f, 0, SEEK_CUR);
		memset(data0, 0, NUM_BYTES);

		fread(data0, 20, 1, f);

		az = (data0[0] | data0[1] << 8);
		azdd = (data0[4] | data0[5] << 8);
		line_size = (data0[8] | data0[9] << 8);
		num_of_line = (data0[12] | data0[13] << 8);
		int step = (round)((float) azdd / num_of_line);
		if (az == az_pre)
			continue;
		az_pre = az;

		if (line_size != 150) {
			printf("Wrong line_size:");
			printf("%d\n", line_size);
			std::exit(0);
		}

		if (num_of_line != 9) {
			printf("Wrong num of line");
			std::exit(0);
		}

		for (int i = 0; i < num_of_line; i++) {
			fread(data0, ELEMENT_SIZE, 1, f);
			if (data0[0])
				printf("aa\n");
			//			mTraceData tData(az, step, data);
			//			scrQueue.push(tData);
			az += step;
		}

		pthread_mutex_unlock(&data_mutex);
		usleep(SLEEP_TIME);

	}
}


void* mDrawRadar(void*){
	char a[50];
	get_ppi51_source(a);
	int line_size = 150;
	int i100 = 100;
	int num_of_line=1;
	int msg_len = num_of_line * line_size + 20;
	char* msg= new char[msg_len];
	int az=0;
	int mazd=10;

	while(1){
		if(!ExTarget::getInstance()->checkState()){
			usleep(100);
			continue;
		}
		if(ExTarget::getInstance()->getMode() == EXSTOP){
			usleep(100);
			continue;
		}
		memcpy(msg, &az, 4);
		memcpy(msg + 4, &mazd, 4);
		memcpy(msg + 8, &line_size, 4);
		memcpy(msg + 12, &num_of_line, 4);
		memcpy(msg + 16, &i100, 4);
		pthread_mutex_lock(&ExTarget::getInstance()->exMapMutexLock);
		memcpy(msg+20,ExTarget::getInstance()->getMap((int)(az/5999.0*MAP_AZI_MAX)),line_size);
		if (ExTarget::getInstance()->getMode() != EXSTOP){
			int r = MsgSend(coid, msg, msg_len, 0, 0);
			if (r == -1) {
				printf("send fail\n");
				exit(EXIT_FAILURE);
			}
		}
		if(ExTarget::getInstance()->getMode()!=EXHISTORY){
			memset(ExTarget::getInstance()->getMap((int)(az/5999.0*MAP_AZI_MAX)),0,150);
//			cout<<"."<<endl;
		}
		pthread_mutex_lock(&ExTarget::getInstance()->exMapMutexLock);
		az+=mazd;
		if(az>5999)az=0;
		usleep(10000);
	}
	delete[] msg;

}
