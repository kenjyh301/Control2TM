/*
 * cmmenums.h
 *
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 */

#ifndef CMMENUMS_H_
#define CMMENUMS_H_

/*----------------------C125 status-----------------------------------*/
enum C125MessageOpcode{
	///Request from dhl --> c125Adapter
	REQUEST_ASSIGNED_TARGET=100,
	REQUEST_DEASSIGNED_TARGET=101,
	REQUEST_SYSTEM_STATUS=102,
	REQUEST_TRACK_UPDATE=103,
	REQUEST_TRACK_SIMULATION=104,
	REQUEST_UK42_LESSION=105,

	///Reply
	RESPOND_ASSIGNED_DONE=150,
	RESPOND_ASSIGNED_FAILE=152,///Check data for more details
	RESPOND_DEASSIGNED_DONE=153,
	RESPOND_DEASSIGNED_FAILED=154,///Check data for more details
	RESPOND_REQUEST_STATUS=155,///CHeck data for more details
	RESPOND_TRACK_SIMULATION=156,
	RESPOND_UK42_LESSION=157
};

#endif /* CMMENUMS_H_ */
