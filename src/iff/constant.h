/*
 * constant.h
 *
 *  Created on: Feb 21, 2020
 *      Author: thanhvn4
 */

#ifndef CORE_CONSTANT_H_
#define CORE_CONSTANT_H_
#include <stdbool.h>


#define HEADER_SIZE								1
#define PHASE_SIZE								1
#define CONST_MAX_ADC_RANGE 					(3060 - HEADER_SIZE - PHASE_SIZE) // 1 byte header + 1 byte phase
#define CONST_MAX_IFF_RANGE						100//12
#define CONST_MAX_NPA_RANGE						100//12
#define RSBN_PHASE_SIZE							1
#define CONST_MAX_RSBN_RANGE					100
#define PCI_ONE_PULSE_SIZE						(HEADER_SIZE + PHASE_SIZE + CONST_MAX_ADC_RANGE + CONST_MAX_IFF_RANGE + CONST_MAX_NPA_RANGE + RSBN_PHASE_SIZE + CONST_MAX_RSBN_RANGE)
#define CONST_ASTERIX_HEADER_LENGTH				8

#define CONST_HEADER_LENGTH						5
#define CONST_MAX_VIDEO_RANGE 					750
#define CONST_MAX_AZIMUTH_BITS					14

#define BASE_VALUE_MAX1184						512
#define CONST_LIGHT_VELOCITY					300000000.0f	// (m) 3 x 10 ^ 8

// CIP4 Constant
#define CONST_SAMPLING_FREQ						1250000
#define RANGE_STEP_SIZE							CONST_LIGHT_VELOCITY / (2 * CONST_SAMPLING_FREQ)
#define CONST_PHASE_RESOLUTION					12

#define CORE_CONFIG_FILE						"/home/vre/config/core_params.config"

typedef struct RawData_tag {
	 unsigned short phase;
	 unsigned short amplitude[CONST_MAX_ADC_RANGE];
	 unsigned short iff_pulse[CONST_MAX_IFF_RANGE];
	 unsigned short npa_pulse[CONST_MAX_NPA_RANGE];
	 unsigned short rsbn_phase;
	 unsigned short	rsbn_range[CONST_MAX_RSBN_RANGE];

} RawDataT;





#endif /* CORE_CONSTANT_H_ */
