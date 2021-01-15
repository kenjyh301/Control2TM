/*
 * ExTarget.cpp
 *
 *  Created on: Dec 15, 2020
 *      Author: Minh
 */

#include "ExTarget.h"
#include <iostream>

ExTarget::ExTarget(){

}

ExTarget* ExTarget::getInstance(){
	static ExTarget* exTarget= new ExTarget();
	return exTarget;
}

void ExTarget::init(){
	for(int i=0;i<MAP_AZI_MAX;i++){
		memset(map[i],0,150);
	}
	history= initExHistory();
	mode= EXSTOP;
	ExTargetEnable= false;
}

void ExTarget::setMap(int x,int y,int value,int size){
	if(y<30)size*=2;
	if(y>=150||y<0||x>MAP_AZI_MAX||x<0)return;

	int max_x= (x+size)<(MAP_AZI_MAX)?x+size:x+size-(MAP_AZI_MAX);
	int min_x= (x-size)>=0?x-size:x-size+(MAP_AZI_MAX);

	int max_y=y;
	int min_y=y;
	if(size==EX_PLOT_SIZE){
		max_y=y+1<150?y+1:149;
		min_y=y-1>0?y-1:0;
	}


//	std::cout<<max_y<<" "<<min_y<<endl;
//	int min_y=y;
	pthread_mutex_lock(&exMapMutexLock);
	if (min_x < max_x) {
		for (int i = min_x; i <= max_x; i++) {
			for (int j = min_y; j <= max_y; j++) {
				map[i][j] = value;
			}
			//		map[i][y]=value;
		}
	} else {
		for (int i = min_x; i <= MAP_AZI_MAX-1; i++) {
			for (int j = min_y; j <= max_y; j++) {
				map[i][j] = value;
			}
			//		map[i][y]=value;
		}
		for (int i = 0; i <= max_x; i++) {
			for (int j = min_y; j <= max_y; j++) {
				map[i][j] = value;
			}
			//		map[i][y]=value;
		}
	}

	pthread_mutex_unlock(&exMapMutexLock);
}

void ExTarget::resetMap(){
	for(int i=0;i<MAP_AZI_MAX;i++){
		memset(map[i],0,150);
	}
}

unsigned char* ExTarget::getMap(int x){
	return map[x];
}

struct exHistory ExTarget::initExHistory(){
	struct exHistory ret;
	ret.pos= new exTargetPos[MAX_HISTORY_SIZE];
	ret.size = 0;
	return ret;
}

void ExTarget::resetHistory(){
	history.size=0;
}

void ExTarget::addHistory(int x,int y,int value,int ID=0){
	history.pos[history.size].x=x;
	history.pos[history.size].y=y;
	history.pos[history.size].value=value;
	history.pos[history.size].ID= ID;
	history.size++;
	int tmp=history.size;
	if(history.size>99)history.size=0;
}

struct exHistory ExTarget::getHistory(){
	return history;
}

ExTargetMode ExTarget::getMode(){
	return mode;
}

void ExTarget::setMode(ExTargetMode mode){
	this->mode=mode;
}

void ExTarget::ExEnable(bool value){
	ExTargetEnable= value;
}
bool ExTarget::checkState(){
	return ExTargetEnable;
}
