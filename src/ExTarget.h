/*
 * ExTarget.h
 *
 *  Created on: Dec 15, 2020
 *      Author: Minh
 */


/* Note
 * Only run when ExTargetEnable= true. ExTargetEnable was set when read data from weapon mode= CheDoChienDau
 */


#ifndef EXTARGET_H_
#define EXTARGET_H_

#include <string>
#include <pthread.h>
#include "SystemConfigurations.h"
using namespace std;

#define EX_PLOT_SIZE (int)(15.0/EX_AZD)
#define EX_VIDEO_SIZE (int)(35.0/EX_AZD)
#define NORMAL_COLOR SystemConfigurations::getInstance()->get_normal_color()
#define IFF_COLOR SystemConfigurations::getInstance()->get_iff_color()
#define MAP_AZI_MAX 2999    //maximum value is 5999
#define EX_AZD (int)(6000.0/MAP_AZI_MAX)
#define FULL_CIRCLE_TIME 10000 // milisecond
#define MAX_HISTORY_SIZE 100

struct exTargetPos{
	int x;
	int y;
	int value;
	int ID;
};

struct exHistory{
	exTargetPos* pos;
	int size;
};

enum ExTargetMode{
	EXSTART,
	EXSTOP,
	EXHISTORY
};

class ExTarget{
public:
	ExTarget();
	static ExTarget* getInstance();
	void init();
	void setMap(int x,int y,int value,int size);
	void resetMap();
	unsigned char* getMap(int x);
	struct exHistory initExHistory();
	void resetHistory();
	void addHistory(int x,int y,int value,int ID);
	struct exHistory getHistory();
	ExTargetMode getMode();
	void setMode(ExTargetMode mode);
	pthread_mutex_t exMapMutexLock;
	void ExEnable(bool value);
	bool checkState();
private:
	unsigned char map[MAP_AZI_MAX][150];
	struct exHistory history;
	ExTargetMode mode;
	bool ExTargetEnable;
};

#endif /* EXTARGET_H_ */
