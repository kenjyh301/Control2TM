/*
 * cmmdefs.h
 *
 *  Updated on: Sep 23, 2018
 *      Author: hunglq11
 *  Created on: Sep 23, 2017
 *      Author: hungnv103
 *
 */

#ifndef CMMDEFS_H_
#define CMMDEFS_H_

#include <stdint.h>

/*------------------------Return code---------------------------*/
#define 		ERR_SUCCESS									0
#define 		ERR_OPEN_FILE_FAILED						1
#define 		ERR_CANNOT_FIND_FILE						2
#define 		ERR_FILE_PATH_NOT_SETTED					3
#define 		ERR_WRITE_FAILED							4
#define 		ERR_READ_FAILED								5
#define 		ERR_QUEUE_EMPTY								6
#define			ERR_QUEUE_FULL								7
#define 		ERR_NOT_DETECT_SYSTEM_COMPONENTS			8
///System operation code

///Continue return code
#define			ERR_TARGET_HAS_ALREADY_ASSIGN				0x100
#define			ERR_TARGET_OUT_OF_RANGE						0x200

#define			ERR_TARGET_HAS_ALREADY_DEASSIGN				0x400
#define			ERR_TARGET_HAS_ACTED						0x800
#define			ERR_TARGET_HAS_CATCHED						0x1000 // YHB found target and sent info. to yk61
#define			ERR_TARGET_HAS_LAUNCHED_MISSILE_1			0x2000 // Launched missile at channel 1
#define			ERR_TARGET_HAS_LAUNCHED_MISSILE_2			0x4000 // Launched missile at channel 2
#define			ERR_TARGET_HAS_MISSILE_FIRED_1				0x8000 // Missile fired
#define			ERR_TARGET_HAS_MISSILE_FIRED_2				0x10000 // Missile fired

//C125 System Status
#define			ERR_SYSTEM_READY_RECEIVED_ASSIGN			0x20000
#define			ERR_SYSTEM_NOT_READY_RECEIVED_ASSIGN		0x40000 ///Not in H mode

#define			ERR_SYSTME_IN_H_MODE						150
#define			ERR_SYSTME_IN_CU_MODE						151
#define			ERR_SYSTME_IN_KO_MODE						152
#define			ERR_SYSTME_IN_MSP_MODE						153
#define			ERR_SYSTME_IN_VSP_MODE						154
#define			ERR_SYSTME_IN_DISABLE_MODE					155

/*-----------------------C125 status flag value-------------------------*/
#define 		C125_FINDING_STATUS_H_FLAG					0x02
#define 		C125_FINDING_STATUS_CU_FLAG					0x04
#define 		C125_FINDING_STATUS_KO_FLAG					0x08
#define 		C125_FINDING_STATUS_MSP_FLAG				0x10
#define 		C125_FINDING_STATUS_VSP_FLAG				0x20
#define 		C125_FINDING_STATUS_DISABLE_FLAG			0x00	//40

#define 		C125_ASSIGMENT_FLAG							0x40

/*-----------------------C125 Memory Offset-------------------------------*/
//uk51
#define 		C125_TRIGGER_ASSIGMENT_BYTE_OFFSET	48
#define 		C125_BEAR_BYTE_OFFSET				54
#define 		C125_RANGE_BYTE_OFFSET				58
#define			C125_STATUS_UK51_BYTE_OFFSET		2816	//_ = 4880-2064

//uk31
#define			C125_FINDING_BYTE_OFFSET			599
#define			C125_SCALE_BYTE_OFFSET				63
#define			C125_CHANGE_SCALE_BYTE_OFFSET		596
#define			C125_STATUS_UK31_BYTE_OFFSET		4880

///uk43
#define 		C125_LAUCH_ON_CHANNEL_1_BYTE_OFFSET	250
#define 		C125_LAUCH_ON_CHANNEL_2_BYTE_OFFSET	252

#define			C125_FIRE_ON_CHANNEL_1_BYTE_OFFSET	201
#define			C125_FIRE_ON_CHANNEL_2_BYTE_OFFSET	201

/*-----------------------C125 Message ------------------------------------*/
#define 		C125_HEADER_VALUE					125
#define 		C125_CONTROL_HEADER					"}Ctl" // 125Ctl

/*-----------------------C125 System Property-----------------------------*/
#define 		C125_MEMORY_UPDATE_PERIOD			35000		///in us
#define			C125_MEMROY_MAX_RANGE_IN_100_MODE	98
#define			C125_MEMROY_MAX_RANGE_IN_80_MODE	81
#define			C125_MEMROY_MAX_RANGE_IN_55_MODE	54
#define			C125_MEMROY_MAX_RANGE_IN_40_MODE	40.5

/*-----------------------C125 Scale Value--------------------------------*/
#define			C125_SCALE_BIT_MASK					0x03
#define 		C125_SCALE_OPERATION_40				0
#define 		C125_SCALE_OPERATION_55				1
#define 		C125_SCALE_OPERATION_80				2
#define 		C125_SCALE_OPERATION_100			3

#define			C125_CHANGE_SCALE_VALUE_TO_40		0x00
#define			C125_CHANGE_SCALE_VALUE_TO_55		0x04
#define			C125_CHANGE_SCALE_VALUE_TO_80		0x02
#define			C125_CHANGE_SCALE_VALUE_TO_100		0x06

///Launched and fired
#define 		C125_LAUCH_ON_CHANNEL_1_BIT_MASK	0x40
#define 		C125_LAUCH_ON_CHANNEL_2_BIT_MASK	0x04

#define			C125_FIRE_ON_CHANNEL_1_BIT_MASK		0x10
#define			C125_FIRE_ON_CHANNEL_2_BIT_MASK		0x20

#define			C125_MAX_SIMULATION_TRACK_IN_LESSON	8
#define			EFFECT_ALL_TARGET_IN_LESSION		229

#define CLEAR_BIT(x,n)		(x &=(~(1<<n)) )
#define SET_BIT(x,n)		(x |=(1<<n) )

#define KM_TO_METER(x)		(x*1000)
#define METER_TO_KM(x)		(x/1000)
#define DEG_TO_RAD(x)		(x*3.14159/180)
#define RAD_TO_DEG(x)		(x*180/3.14159)

namespace SimulationTrack{
enum DangCoDong{
	NET_KHONGCODONG = 1,
	PoH_THEOH = 2,
	PoP_THEOP = 3,
	UPoP_SAILECHTHEOP = 4,
	PoPH_THEOPH = 5,
	UPoP_SAILECHTHEOPH = 6,
	MEN_THEOCONRAN = 7
};

enum DangMucTieu{
	TN_TIEMKICH = 0,
	SB_NBCL = 1,
	KR_TLCOCANH = 2
};

enum DangNhieu{
	NET_KHONGNHIEU = 0,
	PRS_DANCHONGRD = 1,
	ASHP_NHIEUTAPTICHCUC = 2,
	NIP_NHIEUNHAPNHAY = 3,
	UVOD_D_NHIEUXUNGDANTHEOCULI = 4,
	UVOD_F_NHIEUXUNGDANTHEOGOC = 5,
	UVOD_DF_NHIEUXUNGDANTHEOCLIGOC = 6,
	PP_NHIEUTIEUCUC = 7,
	MERC_NHIEUTICHCUCKHONGDONGBO = 8
};
}

typedef struct{
	unsigned int status;
}__attribute__((packed,aligned(1))) C125Status;

typedef struct{
	float bear;///degree
	float range;//km
	unsigned int velocity; //m/s
	unsigned int high;///m
}__attribute__((packed,aligned(1))) TrackInfo;

typedef struct{
    int dangcodong;
    int tocdo0ms;
    int culi0kmx10;
    int thamsoduongbay0kmx10;
    int phuongvi0dox10;
    int hesoquataix10;
    int docao0kmx10;
    int docaokkmx10;
    int goccodongdo;
    int thoidiemcodongs;
    int thoidiemphatnhieus;
    int thoidiemphats;
    int thoidiemkethucs;
    int dangmuctieu;
    int dangnhieu;
}__attribute__((packed,aligned(1))) TrackSimulationData;

typedef struct{
	int numberLession; //_ 1 - 8 in the I file
	int trackDataSize; //_ after trackDataSize (1-8): no need to update TrackSimulationData
	TrackSimulationData trackData[C125_MAX_SIMULATION_TRACK_IN_LESSON];
}__attribute__((packed,aligned(1))) TrackSimulation;

typedef struct{
	char msgHeader[4];
	uint16_t length;
	int msgUUID;
	unsigned short msgOpcode;
	C125Status msgC125Status;
	TrackInfo msgTrackInfo;
//    char ataInfo[90];
	TrackSimulation msgTrackSimulation;
}__attribute__((packed,aligned(1))) C125Message;

#endif /* CMMDEFS_H_ */
