/*
	Monitor Processor
	Created on: Jul 14, 2020
	Author: thuannc1

 */

#include "MonitorProcessor.h"

string MonitorProcessor::GatewayPort;
FileReaderHelper MonitorProcessor::mUk51FileReader;
FileReaderHelper MonitorProcessor::mUk31FileReader;
FileReaderHelper MonitorProcessor::mUk61FileReader;
FileReaderHelper MonitorProcessor::mUk43FileReader;
pthread_mutex_t* MonitorProcessor::uk51MutexLock;
pthread_mutex_t* MonitorProcessor::uk31MutexLock;
pthread_mutex_t* MonitorProcessor::uk61MutexLock;
pthread_mutex_t* MonitorProcessor::uk43MutexLock;
pthread_mutex_t MonitorProcessor::mConnectionLock;
mqd_t MonitorProcessor::mqfd;
int MonitorProcessor::socketId;
int MonitorProcessor::connectId;

MonitorProcessor::MonitorProcessor(){
	//TODO khoi tao
}
MonitorProcessor::~MonitorProcessor(){
}
void MonitorProcessor::setUk51MutexLock(pthread_mutex_t* mutex) {
	uk51MutexLock = mutex;
	return;
}
void MonitorProcessor::setUk31MutexLock(pthread_mutex_t* mutex) {
	uk31MutexLock = mutex;
	return;
}
void MonitorProcessor::setUk61MutexLock(pthread_mutex_t* mutex) {
	uk61MutexLock = mutex;
	return;
}
void MonitorProcessor::setUk43MutexLock(pthread_mutex_t* mutex) {
	uk43MutexLock = mutex;
	return;
}
void MonitorProcessor::setUk51FileReader(FileReaderHelper* fileReader) {
	mUk51FileReader = *fileReader;
}
void MonitorProcessor::setUk31FileReader(FileReaderHelper* fileReader) {
	mUk31FileReader = *fileReader;
}
void MonitorProcessor::setUk61FileReader(FileReaderHelper* fileReader) {
	mUk61FileReader = *fileReader;
}
void MonitorProcessor::setUk43FileReader(FileReaderHelper* fileReader) {
	mUk43FileReader = *fileReader;
}
void MonitorProcessor::getPath( ){
	mUk43FileReader.mFilName = SystemComponentDetector::getUk43Path();
	mUk61FileReader.mFilName = SystemComponentDetector::getUk61Path();
	mUk31FileReader.mFilName = SystemComponentDetector::getUk31Path();
	mUk51FileReader.mFilName = SystemComponentDetector::getUk51Path();
}
void *MonitorProcessor::readData(void *args) {
	//TODO read config file
	char uk43DataArray[UK43DATALENGTH];
	char uk61DataArray[UK61DATALENGTH];
	char uk51DataArray[UK51DATALENGTH];
	char uk31DataArray[UK31DATALENGTH];
	uk43DataStruct uk43Data;
	uk61DataStruct uk61Data;
	uk31DataStruct uk31Data;
	ukDataSend	   ukData;
	struct mq_attr attr;
	struct timespec tm;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(ukDataSend);

	mqfd = mq_open("/queue1", O_RDWR|O_CREAT, 0777, &attr);
	if(mqfd == -1) {
		perror("Child mq_open failure");
		exit(0);
	}

	sleep(10);
	std::cout<<"Read data ready"<<endl;

	while(1) {

		//		usleep(100000);


		sleep(1);
		getPath();

		///Read trigger condition first
		pthread_mutex_lock(uk43MutexLock);
		int returnCode;
		returnCode= mUk43FileReader.readFromFile(uk43DataArray, UK43DATALENGTH, 0);
		pthread_mutex_unlock(uk43MutexLock);
		if( returnCode != ERR_SUCCESS ){
			cout << "Read file uk43 error" << endl;
			usleep(500);
			continue;
		}

		uk43Data.r = *(int16_t*)(uk43DataArray + 266);
		uk43Data.v = *(int16_t*)(uk43DataArray + 282);
		uk43Data.p = *(int16_t*)(uk43DataArray + 284);
		uk43Data.h = *(int16_t*)(uk43DataArray + 286);

		uk43Data.h1_k1 = *(int16_t*)(uk43DataArray + 236);
		uk43Data.h2_k1 = *(int16_t*)(uk43DataArray + 238);
		uk43Data.h1_k2 = *(int16_t*)(uk43DataArray + 240);
		uk43Data.h2_k2 = *(int16_t*)(uk43DataArray + 242);
		uk43Data.d_k1 = *(int16_t*)(uk43DataArray + 298);
		uk43Data.d_k2 = *(int16_t*)(uk43DataArray + 310);

		uk43Data.app = *(int8_t*)(uk43DataArray + 249);
		uk43Data.kdu_mtt_1_pnk_oek_1 = *(int8_t*)(uk43DataArray + 250);
		uk43Data.kdu_mtt_2 = *(int8_t*)(uk43DataArray + 252);
		uk43Data.pnk_oek_2 = *(int8_t*)(uk43DataArray + 251);

		///Read trigger condition first
		pthread_mutex_lock(uk61MutexLock);
		mUk61FileReader.setMeOffset(FileReaderHelper::UK61_OFFSET);
		returnCode = mUk61FileReader.readFromFile(uk61DataArray, UK61DATALENGTH, 0);
		pthread_mutex_unlock(uk61MutexLock);
		if( returnCode != ERR_SUCCESS ){
			cout << "Read file uk61 error" << endl;
			usleep(500);
			continue;
		}
		uk61Data.b = *(double*)(uk61DataArray + 276);
		uk61Data.e = *(double*)(uk61DataArray + 284);
		uk61Data.b1 = *(int16_t*)(uk61DataArray + 328);
		uk61Data.b2 = *(int16_t*)(uk61DataArray + 330);
		uk61Data.b3 = *(int16_t*)(uk61DataArray + 332);
		uk61Data.b4 = *(int16_t*)(uk61DataArray + 334);
		uk61Data.launch_mode = *(int8_t*)(uk61DataArray + 365);
		uk61Data.EMNR = *(int8_t*)(uk61DataArray + 370);

		///Read trigger condition first
		pthread_mutex_lock(uk31MutexLock);
		mUk31FileReader.setMeOffset(FileReaderHelper::UK31_OFFSET);
		returnCode = mUk31FileReader.readFromFile(uk31DataArray, UK31DATALENGTH, 0);
		pthread_mutex_unlock(uk31MutexLock);
		if( returnCode != ERR_SUCCESS ){
			cout << "Read file uk31 error" << endl;
			usleep(500);
			continue;
		}
		uk31Data.ratio = *(int8_t*)(uk31DataArray + 63);
		uk31Data.mode  = *(int8_t*)(uk31DataArray + 595);
		uk31Data.cau_mb = *(int8_t*)(uk31DataArray + 596);
		uk31Data.mb1_mb2 = *(int8_t*)(uk31DataArray + 603);
		uk31Data.oek = *(int8_t*)(uk31DataArray + 604);
		uk31Data.yhb = *(int8_t*)(uk31DataArray + 606);
		uk31Data.tt = *(int8_t*)(uk31DataArray + 767);
		uk31Data.numberRocketEncode = *(int8_t*)(uk31DataArray + 595);
		int8_t temp1;
		int8_t temp2;
		int8_t temp3;
		temp1 = *(int8_t*) (uk31DataArray + 600);
		temp2 = *(int8_t*) (uk31DataArray + 601);
		temp3 = *(int8_t*) (uk31DataArray + 602);
		//		if (temp3 & 0x60 == 0x00){
		//			uk31Data.BamSat = ChuaXacDinh;
		//		} else if (((temp3 & 0x42) == 0x42)|| ((temp3 & 0x20) && (temp2 & 0x01) && ((temp1 & 0x22) == 0x22))) {
		//			uk31Data.BamSat = BamSatTuDong; // tu dong
		//		} else {
		//			uk31Data.BamSat = BamSatBangTay; // bang tay
		//		}

		/*if (temp3 & 0x60 == 0x00){
			uk31Data.BamSat = ChuaXacDinh;
		} else */if (((temp3 & 0x42) == 0x42)|| ((temp3 & 0x20) && (temp2 & 0x01) && ((temp1 & 0x22) == 0x22))) {
			uk31Data.BamSat = true; // tu dong
		} else {
			uk31Data.BamSat = false; // bang tay
		}
		strcpy(ukData.header, "}Mon");
		ukData.length = sizeof(ukData);


		//=======================for run ========================================//
				do {
			ukData.r = 1.5*uk43Data.r;
			ukData.v = uk43Data.v;
			ukData.p = uk43Data.p;
			ukData.h = uk43Data.h;
			ukData.h1_k1 = uk43Data.h1_k1;
			ukData.h1_k2 = uk43Data.h1_k2;
			ukData.h2_k1 = uk43Data.h2_k1;
			ukData.h2_k2 = uk43Data.h2_k2;
			ukData.d_k1  = 1.5*uk43Data.d_k1;
			ukData.d_k2  = 1.5*uk43Data.d_k2;

			if ((uk43Data.app & 0x02) == 0x02) { ukData.app = true; } else { ukData.app = false; }
			if ((uk43Data.kdu_mtt_1_pnk_oek_1 & 0x10) == 0x10) { ukData.ppd_k1 = KDU; }
			else if ((uk43Data.kdu_mtt_1_pnk_oek_1 & 0x30) == 0x20) {ukData.ppd_k1 = MTT;}
			else { ukData.ppd_k1 = ChuaXacDinhPP; }

			if ((uk43Data.kdu_mtt_2 & 0x01) == 0x01) { ukData.ppd_k2 = KDU; }
			else if ((uk43Data.kdu_mtt_2 & 0x03) == 0x02) {ukData.ppd_k2 = MTT;}
			else { ukData.ppd_k2 = ChuaXacDinhPP; }

			if ((uk43Data.kdu_mtt_1_pnk_oek_1 & 0x04) == 0x04) { ukData.kqs_k1 = PNK; }
			else if ((uk43Data.kdu_mtt_1_pnk_oek_1 & 0x0C) == 0x08) {ukData.kqs_k1 = OEK;}
			else { ukData.kqs_k1 = ChuaXacDinhKenhQuanSat; }

			if ((uk43Data.pnk_oek_2 & 0x40) == 0x40) {ukData.kqs_k2 = PNK;}
			else if ((uk43Data.pnk_oek_2 & 0xC0) == 0x80) {ukData.kqs_k2 = OEK;}
			else {ukData.kqs_k2 = ChuaXacDinhKenhQuanSat;}

			ukData.b = uk61Data.b * 180/3.14159;
			ukData.e = uk61Data.e * 180/3.14159;

			//cout << "e " << uk61Data.e << endl;

			if ((uk31Data.numberRocketEncode & 0x01) == 0x01 ) { ukData.numberRocketEncode = Mode2; } else { ukData.numberRocketEncode = Mode1; }

			if (ukData.numberRocketEncode == Mode1) {
				int8_t *temp;
				temp = (int8_t*)(&uk61Data.b1);
				if ((*temp & 0x01) == 0x01) { ukData.b1_1.Rocket_avai = true; } else { ukData.b1_1.Rocket_avai = false;}
				if ((*temp & 0x02) == 0x02) { ukData.b1_2.Rocket_avai = true; } else { ukData.b1_2.Rocket_avai = false;}
				if ((*temp & 0x04) == 0x04) { ukData.b1_3.Rocket_avai = true; } else { ukData.b1_3.Rocket_avai = false;}
				if ((*temp & 0x08) == 0x08) { ukData.b1_4.Rocket_avai = true; } else { ukData.b1_4.Rocket_avai = false;}

				temp = (int8_t*)(&uk61Data.b1 + 1);
				if ((*temp & 0x01) == 0x01) { ukData.b1_1.TrangThai = true; } else { ukData.b1_1.TrangThai = false;}
				if ((*temp & 0x02) == 0x02) { ukData.b1_2.TrangThai = true; } else { ukData.b1_2.TrangThai = false;}
				if ((*temp & 0x04) == 0x04) { ukData.b1_3.TrangThai = true; } else { ukData.b1_3.TrangThai = false;}
				if ((*temp & 0x08) == 0x08) { ukData.b1_4.TrangThai = true; } else { ukData.b1_4.TrangThai = false;}

				temp = (int8_t*)(&uk61Data.b2);
				if ((*temp & 0x01) == 0x01) { ukData.b2_1.Rocket_avai = true; } else { ukData.b2_1.Rocket_avai = false;}
				if ((*temp & 0x02) == 0x02) { ukData.b2_2.Rocket_avai = true; } else { ukData.b2_2.Rocket_avai = false;}
				if ((*temp & 0x04) == 0x04) { ukData.b2_3.Rocket_avai = true; } else { ukData.b2_3.Rocket_avai = false;}
				if ((*temp & 0x08) == 0x08) { ukData.b2_4.Rocket_avai = true; } else { ukData.b2_4.Rocket_avai = false;}////////////

				temp = (int8_t*)(&uk61Data.b2 + 1);
				if ((*temp & 0x01) == 0x01) { ukData.b2_1.TrangThai = true; } else { ukData.b2_1.TrangThai = false;}
				if ((*temp & 0x02) == 0x02) { ukData.b2_2.TrangThai = true; } else { ukData.b2_2.TrangThai = false;}
				if ((*temp & 0x04) == 0x04) { ukData.b2_3.TrangThai = true; } else { ukData.b2_3.TrangThai = false;}
				if ((*temp & 0x08) == 0x08) { ukData.b2_4.TrangThai = true; } else { ukData.b2_4.TrangThai = false;}

				temp = (int8_t*)(&uk61Data.b3);
				if ((*temp & 0x01) == 0x01) { ukData.b3_1.Rocket_avai = true; } else { ukData.b3_1.Rocket_avai = false;}
				if ((*temp & 0x02) == 0x02) { ukData.b3_2.Rocket_avai = true; } else { ukData.b3_2.Rocket_avai = false;}
				if ((*temp & 0x04) == 0x04) { ukData.b3_3.Rocket_avai = true; } else { ukData.b3_3.Rocket_avai = false;}
				if ((*temp & 0x08) == 0x08) { ukData.b3_4.Rocket_avai = true; } else { ukData.b3_4.Rocket_avai = false;}

				temp = (int8_t*)(&uk61Data.b3 + 1);
				if ((*temp & 0x01) == 0x01) { ukData.b3_1.TrangThai = true; } else { ukData.b3_1.TrangThai = false;}
				if ((*temp & 0x02) == 0x02) { ukData.b3_2.TrangThai = true; } else { ukData.b3_2.TrangThai = false;}
				if ((*temp & 0x04) == 0x04) { ukData.b3_3.TrangThai = true; } else { ukData.b3_3.TrangThai = false;}
				if ((*temp & 0x08) == 0x08) { ukData.b3_4.TrangThai = true; } else { ukData.b3_4.TrangThai = false;}

				temp = (int8_t*)(&uk61Data.b4);
				if ((*temp & 0x01) == 0x01) { ukData.b4_1.Rocket_avai = true; } else { ukData.b4_1.Rocket_avai = false;}
				if ((*temp & 0x02) == 0x02) { ukData.b4_2.Rocket_avai = true; } else { ukData.b4_2.Rocket_avai = false;}
				if ((*temp & 0x04) == 0x04) { ukData.b4_3.Rocket_avai = true; } else { ukData.b4_3.Rocket_avai = false;}
				if ((*temp & 0x08) == 0x08) { ukData.b4_4.Rocket_avai = true; } else { ukData.b4_4.Rocket_avai = false;}

				temp = (int8_t*)(&uk61Data.b4 + 1);
				if ((*temp & 0x01) == 0x01) { ukData.b4_1.TrangThai = true; } else { ukData.b4_1.TrangThai = false;}
				if ((*temp & 0x02) == 0x02) { ukData.b4_2.TrangThai = true; } else { ukData.b4_2.TrangThai = false;}
				if ((*temp & 0x04) == 0x04) { ukData.b4_3.TrangThai = true; } else { ukData.b4_3.TrangThai = false;}
				if ((*temp & 0x08) == 0x08) { ukData.b4_4.TrangThai = true; } else { ukData.b4_4.TrangThai = false;}
			} else if (ukData.numberRocketEncode == Mode2) {
				int8_t *temp;
				temp = (int8_t*)(&uk61Data.b1);
				if ((*temp & 0x11) == 0x11) { ukData.b1_1.Rocket_avai = true; } else { ukData.b1_1.Rocket_avai = false;}
				if ((*temp & 0x22) == 0x22) { ukData.b1_2.Rocket_avai = true; } else { ukData.b1_2.Rocket_avai = false;}
				if ((*temp & 0x44) == 0x44) { ukData.b1_3.Rocket_avai = true; } else { ukData.b1_3.Rocket_avai = false;}
				if ((*temp & 0x88) == 0x88) { ukData.b1_4.Rocket_avai = true; } else { ukData.b1_4.Rocket_avai = false;}

				temp = (int8_t*)(&uk61Data.b1 + 1);
				if ((*temp & 0x11) == 0x11) { ukData.b1_1.TrangThai = true; } else { ukData.b1_1.TrangThai = false;}
				if ((*temp & 0x22) == 0x22) { ukData.b1_2.TrangThai = true; } else { ukData.b1_2.TrangThai = false;}
				if ((*temp & 0x44) == 0x44) { ukData.b1_3.TrangThai = true; } else { ukData.b1_3.TrangThai = false;}
				if ((*temp & 0x88) == 0x88) { ukData.b1_4.TrangThai = true; } else { ukData.b1_4.TrangThai = false;}

				temp = (int8_t*)(&uk61Data.b2);
				if ((*temp & 0x11) == 0x11) { ukData.b2_1.Rocket_avai = true; } else { ukData.b2_1.Rocket_avai = false;}
				if ((*temp & 0x22) == 0x22) { ukData.b2_2.Rocket_avai = true; } else { ukData.b2_2.Rocket_avai = false;}
				if ((*temp & 0x44) == 0x44) { ukData.b2_3.Rocket_avai = true; } else { ukData.b2_3.Rocket_avai = false;}
				if ((*temp & 0x88) == 0x88) { ukData.b2_4.Rocket_avai = true; } else { ukData.b2_4.Rocket_avai = NotAvaiable;}////////

				temp = (int8_t*)(&uk61Data.b2 + 1);
				if ((*temp & 0x11) == 0x11) { ukData.b2_1.TrangThai = true; } else { ukData.b2_1.TrangThai = false;}
				if ((*temp & 0x22) == 0x22) { ukData.b2_2.TrangThai = true; } else { ukData.b2_2.TrangThai = false;}
				if ((*temp & 0x44) == 0x44) { ukData.b2_3.TrangThai = true; } else { ukData.b2_3.TrangThai = false;}
				if ((*temp & 0x88) == 0x88) { ukData.b2_4.TrangThai = true; } else { ukData.b2_4.TrangThai = false;}

				temp = (int8_t*)(&uk61Data.b3);
				if ((*temp & 0x11) == 0x11) { ukData.b3_1.Rocket_avai = true; } else { ukData.b3_1.Rocket_avai = false;}
				if ((*temp & 0x22) == 0x22) { ukData.b3_2.Rocket_avai = true; } else { ukData.b3_2.Rocket_avai = false;}
				if ((*temp & 0x44) == 0x44) { ukData.b3_3.Rocket_avai = true; } else { ukData.b3_3.Rocket_avai = false;}
				if ((*temp & 0x88) == 0x88) { ukData.b3_4.Rocket_avai = true; } else { ukData.b3_4.Rocket_avai = false;}

				temp = (int8_t*)(&uk61Data.b3 + 1);
				if ((*temp & 0x11) == 0x11) { ukData.b3_1.TrangThai = true; } else { ukData.b3_1.TrangThai = false;}
				if ((*temp & 0x22) == 0x22) { ukData.b3_2.TrangThai = true; } else { ukData.b3_2.TrangThai = false;}
				if ((*temp & 0x44) == 0x44) { ukData.b3_3.TrangThai = true; } else { ukData.b3_3.TrangThai = false;}
				if ((*temp & 0x88) == 0x88) { ukData.b3_4.TrangThai = true; } else { ukData.b3_4.TrangThai = false;}

				temp = (int8_t*)(&uk61Data.b4);
				if ((*temp & 0x11) == 0x11) { ukData.b4_1.Rocket_avai = true; } else { ukData.b4_1.Rocket_avai = false;}
				if ((*temp & 0x22) == 0x22) { ukData.b4_2.Rocket_avai = true; } else { ukData.b4_2.Rocket_avai = false;}
				if ((*temp & 0x44) == 0x44) { ukData.b4_3.Rocket_avai = true; } else { ukData.b4_3.Rocket_avai = false;}
				if ((*temp & 0x88) == 0x88) { ukData.b4_4.Rocket_avai = true; } else { ukData.b4_4.Rocket_avai = false;}

				temp = (int8_t*)(&uk61Data.b4 + 1);
				if ((*temp & 0x11) == 0x11) { ukData.b4_1.TrangThai = true; } else { ukData.b4_1.TrangThai = false;}
				if ((*temp & 0x22) == 0x22) { ukData.b4_2.TrangThai = true; } else { ukData.b4_2.TrangThai = false;}
				if ((*temp & 0x44) == 0x44) { ukData.b4_3.TrangThai = true; } else { ukData.b4_3.TrangThai = false;}
				if ((*temp & 0x88) == 0x88) { ukData.b4_4.TrangThai = true; } else { ukData.b4_4.TrangThai = false;}

			}
			if ((uk61Data.launch_mode & 0x01) == 0x01) { ukData.launch_mode = true; } else { ukData.launch_mode = false; }
			if ((uk61Data.EMNR & 0x08) == 0x08) { ukData.EMNR = true; } else { ukData.EMNR = false; }

			if ((uk31Data.ratio & 0x03) == 0x00) {ukData.ratio = M40; }
			else if ((uk31Data.ratio & 0x03) == 0x01) {ukData.ratio = M55; }
			else if ((uk31Data.ratio & 0x03) == 0x02) {ukData.ratio = M80; }
			else if ((uk31Data.ratio & 0x03) == 0x03) {ukData.ratio = M100; }

			if ((uk31Data.mode & 0x08) == 0x08) {
				int8_t test= uk31Data.mode;// test variable not important
				ukData.mode = CheDoLuyenTap;}
			else if ((uk31Data.mode & 0x01) == 0x01) { ukData.mode = CheDoChienDau;}
			else if ((uk31Data.mode & 0x02) == 0x02) { ukData.mode = CheDoKiemTraChucNang;}
			else { ukData.mode == ChuaXacDinhCheDo; }

			//			if (uk31Data.cau_mb & 0x08 == 0x08) { ukData.cau = Disable; } else { ukData.cau = Enable; }
			//			if (uk31Data.cau_mb & 0x40 == 0x40) { ukData.mb_enable = Enable; } else { ukData.mb_enable = Disable; }
			//			if (uk31Data.mb1_mb2 & 0x04 == 0x04) { ukData.mb1 = Enable; } else { ukData.mb1 = Disable; }
			//			if (uk31Data.mb1_mb2 & 0x08 == 0x08) { ukData.mb2 = Enable; } else { ukData.mb2 = Disable; }
			//			if (uk31Data.oek & 0x04 == 0x04) { ukData.oek = Enable; } else { ukData.oek = Disable; }
			//			if (uk31Data.yhb & 0x20 == 0x20) { ukData.yhb = Enable; } else { ukData.yhb = Disable; }
			//			if (uk31Data.tt & 0x01 == 0x01) { ukData.tt = Enable; } else { ukData.tt = Disable; }

			if ((uk31Data.cau_mb & 0x08) == 0x08) { ukData.cau = false; } else { ukData.cau = true; }
			if ((uk31Data.cau_mb & 0x40) == 0x40) { ukData.mb_enable = true; } else { ukData.mb_enable = false; }
			if ((uk31Data.mb1_mb2 & 0x04) == 0x04) { ukData.mb1 = true; } else { ukData.mb1 = false; }
			if ((uk31Data.mb1_mb2 & 0x08) == 0x08) { ukData.mb2 = true; } else { ukData.mb2 = false; }
			if ((uk31Data.oek & 0x04) == 0x04) { ukData.oek = true; } else { ukData.oek = false; }
			if ((uk31Data.yhb & 0x20) == 0x20) { ukData.yhb = true; } else { ukData.yhb = false; }
			if ((uk31Data.tt & 0x01) == 0x01) { ukData.tt = true; } else { ukData.tt = false; }
			ukData.BamSat = uk31Data.BamSat;
//			cout  << "size" << sizeof(ukData) << endl;

		} while(0);

		//======================================for test====================================/

		//		do {
//		ukData.r = 100;
//		ukData.v = 200;
//		ukData.p = 300;
//		ukData.h = 400; // ddang sai
//		ukData.h1_k1 = 450;
//		ukData.h1_k2 = 455;
//		ukData.h2_k1 = 461;
//		ukData.h2_k2 = 462;
//		ukData.d_k1  = 463;
//		ukData.d_k2  = 464;
//
//		ukData.app = true;
//		ukData.ppd_k1 = MTT; // Sai nhan dc gia tri 0
//		ukData.ppd_k2 = MTT;
//
//		ukData.kqs_k1 = OEK;
//		ukData.kqs_k2 = OEK;
//
//		ukData.b = 4.5f;
//		ukData.e = 203.3f;
//
//		ukData.b1_1.Rocket_avai = true;
//		ukData.b1_2.Rocket_avai = false;
//		ukData.b1_3.Rocket_avai = false;
//		ukData.b1_4.Rocket_avai = true;
//
//		ukData.b1_1.TrangThai = true;
//		ukData.b1_2.TrangThai = true;
//		ukData.b1_3.TrangThai = true;
//		ukData.b1_4.TrangThai = true;
//
//		ukData.b2_1.Rocket_avai = false;
//		ukData.b2_2.Rocket_avai = false;
//		ukData.b2_3.Rocket_avai = true;
//		ukData.b2_4.Rocket_avai = true;
//
//		ukData.b2_1.TrangThai = false;
//		ukData.b2_2.TrangThai = true;
//		ukData.b2_3.TrangThai = true;
//		ukData.b2_4.TrangThai = false;
//
//		ukData.b3_1.Rocket_avai = false;
//		ukData.b3_2.Rocket_avai = false;
//		ukData.b3_3.Rocket_avai = false;
//		ukData.b3_4.Rocket_avai = false;
//
//		ukData.b3_1.TrangThai = true;
//		ukData.b3_2.TrangThai = true;
//		ukData.b3_3.TrangThai = true;
//		ukData.b3_4.TrangThai = true;
//
//		ukData.b4_1.Rocket_avai = false;
//		ukData.b4_2.Rocket_avai = false;
//		ukData.b4_3.Rocket_avai = false;
//		ukData.b4_4.Rocket_avai = true;
//
//
//		ukData.b4_1.TrangThai = true;
//		ukData.b4_2.TrangThai = true;
//		ukData.b4_3.TrangThai = true;
//		ukData.b4_4.TrangThai = true;
//
//		ukData.launch_mode = PhongPhatMot;
//		ukData.EMNR = true;
//
//		ukData.ratio = M100;
//
//		ukData.mode = CheDoLuyenTap;
//
//		ukData.cau = false;
//		ukData.mb_enable = true;
//		ukData.mb1 = false;
//		ukData.mb2 = false;
//		ukData.oek = true;
//		ukData.yhb = false;
//		ukData.tt = false;
//		ukData.numberRocketEncode = Mode2;
		//				} while (0);

		cout << "Size of message send: " << (ukData.length) << endl;

		ExTarget::getInstance()->ExEnable(ukData.mode==CheDoChienDau);
		excerciseMode = (ukData.mode==CheDoLuyenTap); // if exerciseMode==true read data from uk51 ppi51 as file to process plot

		if (mqfd != -1) {
			clock_gettime(CLOCK_REALTIME, &tm);
			tm.tv_sec += 1;
			int status = mq_timedsend(mqfd, (char*)&ukData, sizeof(ukData), 0, &tm);
			if (status == -1) {
				printf("mq_send failure\n");
			}
			else {
				//			printf("mq_send successful\n");
			}
		}
		cout << strerror(errno) << endl;

		mUk43FileReader.setMeOffset(FileReaderHelper::UK43_OFFSET);
		mUk51FileReader.setMeOffset(FileReaderHelper::UK51_OFFSET);
		mUk31FileReader.setMeOffset(FileReaderHelper::UK31_OFFSET);
		mUk61FileReader.setMeOffset(FileReaderHelper::UK61_OFFSET);
	}
	mq_close(mqfd);
	return NULL;
}
void MonitorProcessor::run(){
	//TODO load config;
	GatewayPort = SystemConfigurations::getInstance()->GatewayPort;
	pthread_create(&mReadDataThread, NULL, readData, NULL);
}
