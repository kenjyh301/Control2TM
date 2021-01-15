/*
 * iff_clone.h
 *
 *  Created on: Oct 21, 2020
 *      Author: Minh
 */

#ifndef IFF_CLONE_H_
#define IFF_CLONE_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "constant.h"
#include "../screenDataQueue/sceenDataQueue.h"
#include "../SystemConfigurations.h"
//#include "../header/header.h"

#define CONST_MAX_IFF_RANGE		 100




typedef struct {
	unsigned int phase_start;
	unsigned int phase_end;
	unsigned int range_start;
	unsigned int range_end;
	int size;
	unsigned char  isDetecing;
	unsigned char  counter;
}IffInfoT;

typedef struct {
	unsigned int center_of_phase;
	unsigned int center_of_range;
}IffTargetT;

extern int target_ready;

void iff_init();
void iff_processing(mTraceData RawData);


#endif /* IFF_CLONE_H_ */
