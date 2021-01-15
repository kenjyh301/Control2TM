/*
 * asterix_cat240.h
 *
 *  Created on: Mar 13, 2017
 *      Author: hungnv103
 */

#ifndef SERVER_ASTERIX_CAT240_H_
#define SERVER_ASTERIX_CAT240_H_

#include "types.h"
/***********************CAT240 Field Definition******************************/
#define ASTERIX_HEADER_LENGTH 						3
#define ASTERIX_FRN_FX								0				/// Order 8
/***********************CAT240 Field Definition******************************/

#define CAT240_VALUE								240

#define CAT240_FRN_DATA_SOURCE_IDENTIFIER			7				/// Order 1
#define CAT240_FRN_MESSAGE_TYPE						6				/// Order 2
#define CAT240_FRN_VIDEO_RECORD_HEADER				5				/// Order 3
#define CAT240_FRN_VIDEO_SUMMARY					4				/// Order 4
#define CAT240_FRN_VIDEO_HEADER_NANO				3				/// Order 5
#define CAT240_FRN_VIDEO_HEADER_FEMTO				2				/// Order 6
#define CAT240_FRN_VIDEO_CELL_RESOLUTION			1				/// Order 7


#define CAT240_FRN_VIDEO_BLOCKS_COUNTER				7
#define CAT240_FRN_VIDEO_BLOCK						6
#define CAT240_FRN_VIDEO_LOW_BLOCK					6
#define CAT240_FRN_VIDEO_MEDIUM_BLOCK				5
#define CAT240_FRN_VIDEO_HIGH_BLOCK					4
#define CAT240_FRN_TIME_OF_DAY						3
#define CAT240_FRN_RE								2
#define CAT240_FRN_SP								1


#define CAT240_LENGTH_SOURCE_IDENTIFIER				2
#define CAT240_LENGTH_MESSAGE_TYPE					1
#define CAT240_LENGTH_VIDEO_RECORD_HEADER			4
#define CAT240_LENGTH_VIDEO_HEADER					12
#define CAT240_LENGTH_VIDEO_CELL_RESOLUTION			2
#define CAT240_LENGTH_VIDEO_BLOCK_COUNTER			2
#define CAT240_V12_LENGTH_VIDEO_BLOCK_COUNTER		5
#define CAT240_LENGTH_VIDEO_BLOCK					4			/// 4 multiple number of cell
#define CAT240_LENGTH_RESERVED_EXPANSION			1			/// 4 multiple number of cell
#define CAT240_LENGTH_FIELD_SPECIFICATION			2

#define CAT240_LENGTH_VIDEO_BLOK_IN_BITS			(CAT240_LENGTH_VIDEO_BLOCK*8)

#define CAT240_VIDEO_RESOLUTION_1_BITS				1			// signal amplitude is decoding in the 1 bit
#define CAT240_VIDEO_RESOLUTION_2_BITS				2			// signal amplitude is decoding in the 2 bit
#define CAT240_VIDEO_RESOLUTION_4_BITS				3			// signal amplitude is decoding in the 4 bit
#define CAT240_VIDEO_RESOLUTION_8_BITS				4			// signal amplitude is decoding in the 8 bit
#define CAT240_VIDEO_RESOLUTION_16_BITS				5			// signal amplitude is decoding in the 16 bit
#define CAT240_VIDEO_RESOLUTION_32_BITS				6			// signal amplitude is decoding in the 32 bit

#define CAT240_VIDEO_RESOLUTION						CAT240_VIDEO_RESOLUTION_8_BITS

#define CAT240_VIDEO_BLOCK_LOW_DATA_FACTOR			4
#define CAT240_VIDEO_BLOCK_LOW_DATA_MAX				(CAT240_VIDEO_BLOCK_LOW_DATA_FACTOR*256)
#define CAT240_VIDEO_BLOCK_MEDIUM_DATA_FACTOR		64
#define CAT240_VIDEO_BLOCK_MEDIUM_DATA_MAX			(CAT240_VIDEO_BLOCK_MEDIUM_DATA_FACTOR*256)
#define CAT240_VIDEO_BLOCK_HIGH_DATA_FACTOR			256
#define CAT240_VIDEO_BLOCK_MEDIUM_DATA_MAX			(CAT240_VIDEO_BLOCK_MEDIUM_DATA_FACTOR*256)

#define DATA_SOURCE_IDENTIFIER_VALUE			0xAA55

#define CAT240_MESSAGE_TYPE_VIDEO_SUMARRY_MESSAGE	1
#define CAT240_MESSAGE_TYPE_VIDEO_MESSAGE			2


/***********************CAT034 Field Definition******************************/
#define CAT034_VALUE								34

/***********************CAT034 Field Reference Numbers */
#define CAT034_FRN_DATA_SOURCE_IDENTIFIER			7
#define CAT034_FRN_DATA_MESSAGE_TYPE				6
#define CAT034_FRN_TIME_OF_DAY						5
#define CAT034_FRN_SECTOR_NUMBER					4
#define CAT034_FRN_ANTENNA_ROTATION					3
#define CAT034_FRN_SYSTEM_CONFIGURATION				2
#define CAT034_FRN_SYSTEM_PROCESSING_MODE			1

#define CAT034_FRN_MESSAGE_COUNT_VALUE				7
#define CAT034_FRN_GENERIC_POLAR_WINDOWN			6
#define CAT034_FRN_DATA_FILTER						5
#define CAT034_FRN_3D_POSITION						4
#define CAT034_FRN_COLLIMATION_ERROR				3

/***********************CAT034 Data Length of Field******************************/
#define CAT034_LENGTH_DATA_SOURCE_IDENTIFIER		2
#define CAT034_LENGTH_MESSAGE_TYPE					1
#define CAT034_LENGTH_TIME_OF_DAY					3
#define CAT034_LENGTH_SECTOR_NUMBER					1
#define CAT034_LENGTH_ATENNA_ROTATION_PERIOD		2
#define CAT034_LENGTH_SYSTEM_CONFIGURATION			1	//(1+)
#define CAT034_LENGTH_SYSTEM_PROCESSING				1	//(1+)

#define CAT034_LENGTH_MESSAGE_COUNT_VALUE			2	//(1 + 2*N)
#define CAT034_LENGTH_GENERIC_POLAR_WINDOW			8
#define CAT034_LENGTH_DATA_FILTER					1
#define CAT034_LENGTH_3D_POSITION_DATA_SOURCE		8
#define CAT034_LENGTH_COLLIMATION_ERROR				2

///Message type
#define CAT034_MESSAGE_TYPE_NORTH_MARK				1
#define CAT034_MESSAGE_TYPE_SECTOR_CROSSING			2
#define CAT034_MESSAGE_TYPE_GEOFILETER				3

/***********************CAT048 Field Definition******************************/
#define CAT048_VALUE								48

#define CAT048_FRN_DATA_SOURCE_IDENTIFIER			7
#define CAT048_FRN_TIME_OF_DAY						6
#define CAT048_FRN_TARGET_REPORT_DESCRIPTOR			5
#define CAT048_FRN_POSITION_IN_POLAR				4
#define CAT048_FRN_MODE_3A_CODE						3
#define CAT048_FRN_FLIGHT_LEVEL						2
#define CAT048_FRN_RADAR_PLOT_CHARACTOR				1

#define CAT048_FRN_HEIGHT_MEASURE_3D				3
#define CAT048_FRN_TRACK_NUMBER						4

/***********************CAT048 Data Length of Field******************************/
#define CAT048_LENGTH_DATA_SOURCE_IDENTIFIER			2
#define CAT048_LENGTH_TIME_OF_DAY						3
#define CAT048_LENGTH_TARGET_REPORT_DESCRIPTOR			2	//(1+)
#define CAT048_LENGTH_POSITION_IN_POLAR					4
#define CAT048_LENGTH_MODE_3A_CODE						2
#define CAT048_LENGTH_FLIGHT_LEVEL						2
#define CAT048_LENGTH_RADAR_PLOT_CHARACTOR				2	//(1+1+...)

#define CAT048_LENGTH_HEIGHT_MEASURE_3D					2
#define CAT048_LENGTH_TRACK_NUMBER						2

#define Init_Header(buff, data_length, cat)			do{ buff[0] = cat; buff[1] = ((data_length)>>8)&0xFF ; buff[2] = ((data_length)&0xFF) ;} while(0)

#define Set_Fspec(fspec,frn)						(fspec |= (1<<frn))


/*
 * @Brief: This function will coding the video data into cat 240 video message.
 * @Input:
 * 			video_data: the signal amplitude in 8 bit resolution
 * 			sampling_rate: the adc sampling rate sample/s, current vre version is 1.250.000 ///1,25M
 * 			ranage_in_point: usually, we using 3060 for 360 km
 *
 * 	@Output:
 * 			cat240_message: the message that is complied with asterix
 * 			message_length: the length of the asterix message in byte
 */

void coding_cat240_video_message(unsigned char *video_data, int phase, long sampling_rate, int range_in_point, unsigned char sp ,unsigned char **cat240_message, int *message_length);
void coding_cat240_video_summary_message( unsigned char *summary, int summary_lenth, unsigned char **cat240_message, int *message_length);

void coding_cat240_v12_video_message(unsigned char *video_data, int phase, long sampling_rate, int range_in_point, unsigned char sp, unsigned char **cat240_message, int *message_length);
void coding_cat240_v12_video_summary_message( unsigned char *summary, int summary_lenth, unsigned char **cat240_message, int *message_length);


void coding_cat034_north_marker(unsigned char **cat034, int *msg_length);
void coding_cat034_sector_crossing(unsigned int azimuth, unsigned char **cat034, int *msg_length);

/*
 * Range: in meter
 * Phase: 0-2^16 <=> [0-360]
 */
void coding_cat048_plot_report( int plot_phase, int plot_range,unsigned char type_of_plot, unsigned char **cat048, int *msg_length);
void coding_cat048_plot_3d_report( int plot_phase, int plot_range, int plot_altitude, unsigned char **cat048, int *msg_length);


#endif /* SERVER_ASTERIX_CAT240_H_ */
