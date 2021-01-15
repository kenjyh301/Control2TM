/*
 * asterix_cat240.c
 *
 *  Created on: Mar 13, 2017
 *      Author: hungnv103
 */



#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <stdint.h>
#include <math.h>
#include "asterix_cat.h"

void get_elapsed_since_midnight(long long int *elapseSinceMidnight)
{
	/*
	 * elpaseSinceMidnight: save the time from the midnight in microsecond unit
	 */
	struct timeval _timeSinceEpoch;
	const long ONE_DAY_IN_SECOND	= 86400;	/// 86400 = 24*60*60
	const long SECOND_TO_MICRO_SECOND_FACTOR = 1000000;
	gettimeofday(&_timeSinceEpoch,NULL); // Get time from 01/01/1970

	*elapseSinceMidnight = (_timeSinceEpoch.tv_sec%ONE_DAY_IN_SECOND); ///get elapsed since midnight in second
	*elapseSinceMidnight = (*elapseSinceMidnight)*SECOND_TO_MICRO_SECOND_FACTOR + _timeSinceEpoch.tv_usec;
//	printf("done");
}

void coding_cat240_video_message(unsigned char *video_data, int phase ,long sampling_rate, int range_in_point, unsigned char sp, unsigned char **cat240_message, int *message_length)
{
	/// This static parameter is using to generate the message index

	///Calculate the asterix message length:

	unsigned short video_block_counter;
	unsigned char *msg; /// assigned the another to more convention.
	int current_msg_index = 0;

	*message_length = ASTERIX_HEADER_LENGTH  // fixed header length
			+ CAT240_LENGTH_SOURCE_IDENTIFIER
			+ CAT240_LENGTH_MESSAGE_TYPE
			+ CAT240_LENGTH_VIDEO_RECORD_HEADER
			+ CAT240_LENGTH_VIDEO_HEADER
			+ CAT240_LENGTH_VIDEO_CELL_RESOLUTION
			+ CAT240_LENGTH_VIDEO_BLOCK_COUNTER;
	if( sp == 0xFF )
	{
		*message_length += CAT240_LENGTH_RESERVED_EXPANSION;
	}
	/// Calculate the video_block_cell
	switch( CAT240_VIDEO_RESOLUTION)
	{
	case CAT240_VIDEO_RESOLUTION_1_BITS:
		video_block_counter = ceil( range_in_point*1.0/32); /// default is 8 bits
		break;
	case CAT240_VIDEO_RESOLUTION_32_BITS:
		video_block_counter = range_in_point;
		break;
	case CAT240_VIDEO_RESOLUTION_4_BITS:
		video_block_counter = ceil(range_in_point*1.0/8);
		break;
	case CAT240_VIDEO_RESOLUTION_8_BITS:
		video_block_counter = ceil(range_in_point*1.0/4);
		break;
	default:
		video_block_counter = ceil( range_in_point*1.0/8); /// default is 8 bits
	}

	*message_length += video_block_counter*CAT240_LENGTH_VIDEO_BLOCK;

	msg = (unsigned char*)malloc(*message_length);

	/// Init the header
	Init_Header(msg,*message_length, CAT240_VALUE);
	current_msg_index += ASTERIX_HEADER_LENGTH;

	{

		/// Init the field specification
		uint8_t fspec1 = 0, fspec2 = 0;

		Set_Fspec(fspec1,CAT240_FRN_DATA_SOURCE_IDENTIFIER);
		Set_Fspec(fspec1,CAT240_FRN_MESSAGE_TYPE);
		Set_Fspec(fspec1,CAT240_FRN_VIDEO_RECORD_HEADER);
		Set_Fspec(fspec1,CAT240_FRN_VIDEO_HEADER_NANO);
		Set_Fspec(fspec1,CAT240_FRN_VIDEO_CELL_RESOLUTION);
		Set_Fspec(fspec1,ASTERIX_FRN_FX);

		Set_Fspec(fspec2,CAT240_FRN_VIDEO_BLOCKS_COUNTER);
		Set_Fspec(fspec2,CAT240_FRN_VIDEO_BLOCK);

		if( sp != 0xFF )
		{
			Set_Fspec(fspec2, CAT240_FRN_RE);
		}

		msg[current_msg_index] = fspec1;
		current_msg_index += 1;

		msg[current_msg_index] = fspec2;
		current_msg_index += 1;
	}

	{
		// Assign Data source identifier
		uint8_t data_source_identifier[CAT240_LENGTH_SOURCE_IDENTIFIER];
		data_source_identifier[0] = (DATA_SOURCE_IDENTIFIER_VALUE>>8)&0xFF;
		data_source_identifier[1] = DATA_SOURCE_IDENTIFIER_VALUE&0xFF;
		ms_mem(cpy)(msg+current_msg_index,data_source_identifier,CAT240_LENGTH_SOURCE_IDENTIFIER);
		current_msg_index += CAT240_LENGTH_SOURCE_IDENTIFIER;
	}

	{
		/// Assign Message Type
		msg[current_msg_index] = CAT240_MESSAGE_TYPE_VIDEO_MESSAGE;
		current_msg_index+= CAT240_LENGTH_MESSAGE_TYPE;
	}

	{
		/// Assign Video record header
		static long message_index=0;

		uint8_t video_record_header[CAT240_LENGTH_VIDEO_RECORD_HEADER];
		video_record_header[3] = message_index&0xFF;
		video_record_header[2] = (message_index>>8)&0xFF;
		video_record_header[1] = (message_index>>16)&0xFF;
		video_record_header[0] = (message_index>>24)&0xFF;

		message_index += 1; // Increase message

		ms_mem(cpy)(msg+current_msg_index, video_record_header, CAT240_LENGTH_VIDEO_RECORD_HEADER);
		current_msg_index += CAT240_LENGTH_VIDEO_RECORD_HEADER;
	}

	{
		///Assigned Video Header Nano

		///Becase the curren VRE version only using 14 bits for decode phase, so we must append 2 bits at the
		/// end of the
		int start_az = (phase&0x3FFF)<<2; /// 14 bit phase
		int end_az = ((phase&0x3FF)<<2)|0x0003; ///
		int start_range = 0;
		long cell_duration = (1000000000.0f/sampling_rate);			///10^-9 LSB - in nano second
		uint8_t video_header_nano[CAT240_LENGTH_VIDEO_HEADER];

		video_header_nano[1] = (start_az&0xFF);
		video_header_nano[0] = (start_az>>8)&0xFF;

		video_header_nano[3] = (end_az)&0xFF;
		video_header_nano[2] = (end_az>>8)&0xFF;

		video_header_nano[7] = start_range&0xFF;
		video_header_nano[6] = (start_range>>8)&0xFF;
		video_header_nano[5] = (start_range>>16)&0xFF;
		video_header_nano[4] = (start_range>>24)&0xFF;

		video_header_nano[11] = (cell_duration)&0xFF;
		video_header_nano[10] = (cell_duration>>8)&0xFF;
		video_header_nano[9] = (cell_duration>>16)&0xFF;
		video_header_nano[8] = (cell_duration>>24)&0xFF;

		ms_mem(cpy)(msg+current_msg_index,video_header_nano, CAT240_LENGTH_VIDEO_HEADER);
		current_msg_index += CAT240_LENGTH_VIDEO_HEADER;
	}

	{
		/// Assigned the video cell resolution
		uint8_t video_cell_resolution[CAT240_LENGTH_VIDEO_CELL_RESOLUTION];
		video_cell_resolution[0] = 0;/// Spare
		video_cell_resolution[1] = CAT240_VIDEO_RESOLUTION;
		ms_mem(cpy)(msg + current_msg_index, video_cell_resolution, CAT240_LENGTH_VIDEO_CELL_RESOLUTION);
		current_msg_index += CAT240_LENGTH_VIDEO_CELL_RESOLUTION;
	}

	{
		/// Assigned video block count
		uint8_t video_block_counter_data[CAT240_LENGTH_VIDEO_BLOCK_COUNTER];
		video_block_counter_data[1] = video_block_counter&0xFF;
		video_block_counter_data[0] = (video_block_counter>>8)&0xFF;
		ms_mem(cpy)(msg+ current_msg_index, video_block_counter_data, CAT240_LENGTH_VIDEO_BLOCK_COUNTER);
		current_msg_index += CAT240_LENGTH_VIDEO_BLOCK_COUNTER;
	}

	switch( CAT240_VIDEO_RESOLUTION)
	{
	case CAT240_VIDEO_RESOLUTION_1_BITS:
		break;
	case CAT240_VIDEO_RESOLUTION_32_BITS:
		break;
	case CAT240_VIDEO_RESOLUTION_4_BITS:
		break;
	case CAT240_VIDEO_RESOLUTION_8_BITS:
	{
		int cpy_length = (range_in_point/CAT240_LENGTH_VIDEO_BLOCK)*CAT240_LENGTH_VIDEO_BLOCK;
		ms_mem(cpy)(msg+current_msg_index, video_data, cpy_length);
		current_msg_index += cpy_length;
		if( cpy_length < range_in_point)
		{
			ms_mem(cpy)( msg + current_msg_index, video_data + (cpy_length), range_in_point-cpy_length);
			current_msg_index+= (range_in_point-cpy_length);
		}
	}
	break;
	default:
		break;
	}

	*cat240_message = msg;
}
void coding_cat240_video_summary_message( unsigned char *summary, int summary_lenth, unsigned char **cat240_message, int *message_length)
{
	///Calculate the asterix message length:

	uint8_t *msg = *cat240_message; /// assigned the another to more convention.
	int current_msg_index = 0;

	summary_lenth &= 0xFF;

	*message_length = ASTERIX_HEADER_LENGTH  // fixed header length
			+ CAT240_LENGTH_SOURCE_IDENTIFIER
			+ CAT240_LENGTH_MESSAGE_TYPE
			+ 1 + summary_lenth;

	/// Init the header
	Init_Header(msg,*message_length, CAT240_VALUE);
	current_msg_index += ASTERIX_HEADER_LENGTH;

	{

		/// Init the field specification
		uint8_t fspec1 = 0;

		Set_Fspec(fspec1,CAT240_FRN_DATA_SOURCE_IDENTIFIER);
		Set_Fspec(fspec1,CAT240_FRN_MESSAGE_TYPE);
		Set_Fspec(fspec1,CAT240_FRN_VIDEO_SUMMARY);


		msg[current_msg_index] = fspec1;
		current_msg_index += 1;
	}

	{
		// Assign Data source identifier
		uint8_t data_source_identifier[CAT240_LENGTH_SOURCE_IDENTIFIER];
		data_source_identifier[1] = DATA_SOURCE_IDENTIFIER_VALUE&0xFF;
		data_source_identifier[0] = (DATA_SOURCE_IDENTIFIER_VALUE>>8)&0xFF;
		ms_mem(cpy)(msg+current_msg_index,data_source_identifier,CAT240_LENGTH_SOURCE_IDENTIFIER);
		current_msg_index += CAT240_LENGTH_SOURCE_IDENTIFIER;
	}

	{
		/// Assign Message Type
		msg[current_msg_index] = CAT240_MESSAGE_TYPE_VIDEO_SUMARRY_MESSAGE;
		current_msg_index+= CAT240_LENGTH_MESSAGE_TYPE;
	}
	{
		///Assign messsage Type
		msg[current_msg_index] = summary_lenth;
		current_msg_index++;

		ms_mem(cpy)(msg+current_msg_index, summary, summary_lenth);
		//current_msg_index+= summary_lenth;
	}
}

void coding_cat240_v12_video_message(unsigned char *video_data, int phase, long sampling_rate, int range_in_point, unsigned char sp, unsigned char **cat240_message, int *message_length)
{
	/// This static parameter is using to generate the message index

	///Calculate the asterix message length:

	unsigned short video_octet_counter;
	uint8_t rep = 0;
	unsigned short video_block_factor;
	uint8_t video_block_field_spec = 0;
	uint8_t *msg; /// assigned the another to more convention.
	int current_msg_index = 0;

	*message_length = ASTERIX_HEADER_LENGTH  // fixed header length
			+ CAT240_LENGTH_SOURCE_IDENTIFIER
			+ CAT240_LENGTH_MESSAGE_TYPE
			+ CAT240_LENGTH_VIDEO_RECORD_HEADER
			+ CAT240_LENGTH_VIDEO_HEADER
			+ CAT240_LENGTH_VIDEO_CELL_RESOLUTION
			+ CAT240_V12_LENGTH_VIDEO_BLOCK_COUNTER
			+ CAT240_LENGTH_FIELD_SPECIFICATION;
	if( sp != 0xFF )
	{
		*message_length += CAT240_LENGTH_RESERVED_EXPANSION;
	}
	/// Calculate the video_block_cell
	switch( CAT240_VIDEO_RESOLUTION)
	{
	case CAT240_VIDEO_RESOLUTION_1_BITS:
		video_octet_counter = ceil( range_in_point*1.0/8);
		break;
	case CAT240_VIDEO_RESOLUTION_2_BITS:
		video_octet_counter = ceil(range_in_point*2.0/8);
		break;
	case CAT240_VIDEO_RESOLUTION_4_BITS:
		video_octet_counter = ceil(range_in_point*4.0/8);/// default is 8 bits
		break;
	case CAT240_VIDEO_RESOLUTION_8_BITS:
		video_octet_counter = range_in_point;//ceil(range_in_point*8.0/8);
		break;
	case CAT240_VIDEO_RESOLUTION_16_BITS:
		video_octet_counter = ceil(range_in_point*16.0/8);
		break;
	case CAT240_VIDEO_RESOLUTION_32_BITS:
		video_octet_counter = ceil(range_in_point*32.0/8);
		break;
	default:
		video_octet_counter = ceil( range_in_point*1.0/8); /// default is 8 bits
	}


	if( video_octet_counter <= CAT240_VIDEO_BLOCK_LOW_DATA_MAX)
	{
		video_block_field_spec = CAT240_FRN_VIDEO_LOW_BLOCK;
		rep = ceil((video_octet_counter-1)*1.0/CAT240_VIDEO_BLOCK_LOW_DATA_FACTOR);
		video_block_factor = CAT240_VIDEO_BLOCK_LOW_DATA_FACTOR;
	} else if(video_octet_counter <= CAT240_VIDEO_BLOCK_MEDIUM_DATA_MAX )
	{
		video_block_field_spec = CAT240_FRN_VIDEO_MEDIUM_BLOCK;
		rep = ceil((video_octet_counter-1)*1.0/CAT240_VIDEO_BLOCK_MEDIUM_DATA_FACTOR);
		video_block_factor = CAT240_VIDEO_BLOCK_MEDIUM_DATA_FACTOR;
	} else {
		video_block_field_spec = CAT240_FRN_VIDEO_HIGH_BLOCK;
		rep = ceil((video_octet_counter-1)*1.0/CAT240_VIDEO_BLOCK_HIGH_DATA_FACTOR);
		video_block_factor = CAT240_VIDEO_BLOCK_HIGH_DATA_FACTOR;
	}
	*message_length += (1 + (rep)*video_block_factor);
	msg = (unsigned char*)malloc(*message_length);

	/// Init the header
	Init_Header(msg,*message_length, CAT240_VALUE);
	current_msg_index += ASTERIX_HEADER_LENGTH;
	{

		/// Init the field specification
		uint8_t fspec1 = 0, fspec2 = 0;

		Set_Fspec(fspec1,CAT240_FRN_DATA_SOURCE_IDENTIFIER);
		Set_Fspec(fspec1,CAT240_FRN_MESSAGE_TYPE);
		Set_Fspec(fspec1,CAT240_FRN_VIDEO_RECORD_HEADER);
		Set_Fspec(fspec1,CAT240_FRN_VIDEO_HEADER_NANO);
		Set_Fspec(fspec1,CAT240_FRN_VIDEO_CELL_RESOLUTION);
		Set_Fspec(fspec1,ASTERIX_FRN_FX);

		Set_Fspec(fspec2,CAT240_FRN_VIDEO_BLOCKS_COUNTER);
		Set_Fspec(fspec2,video_block_field_spec);

		if( sp != 0xFF )
		{
			Set_Fspec(fspec2, CAT240_FRN_RE);
		}

		msg[current_msg_index] = fspec1;
		current_msg_index += 1;

		msg[current_msg_index] = fspec2;
		current_msg_index += 1;
	}

	{
		// Assign Data source identifier
		uint8_t data_source_identifier[CAT240_LENGTH_SOURCE_IDENTIFIER];
		data_source_identifier[0] = (DATA_SOURCE_IDENTIFIER_VALUE>>8)&0xFF;
		data_source_identifier[1] = DATA_SOURCE_IDENTIFIER_VALUE&0xFF;
		ms_mem(cpy)(msg+current_msg_index,data_source_identifier,CAT240_LENGTH_SOURCE_IDENTIFIER);
		current_msg_index += CAT240_LENGTH_SOURCE_IDENTIFIER;
	}

	{
		/// Assign Message Type
		msg[current_msg_index] = CAT240_MESSAGE_TYPE_VIDEO_MESSAGE;
		current_msg_index+= CAT240_LENGTH_MESSAGE_TYPE;
	}

	{
		/// Assign Video record header
		static long message_index=0;

		uint8_t video_record_header[CAT240_LENGTH_VIDEO_RECORD_HEADER];
		video_record_header[3] = message_index&0xFF;
		video_record_header[2] = (message_index>>8)&0xFF;
		video_record_header[1] = (message_index>>16)&0xFF;
		video_record_header[0] = (message_index>>24)&0xFF;

		message_index += 1; // Increase message

		ms_mem(cpy)(msg+current_msg_index, video_record_header, CAT240_LENGTH_VIDEO_RECORD_HEADER);
		current_msg_index += CAT240_LENGTH_VIDEO_RECORD_HEADER;
	}

	{
		///Assigned Video Header Nano

		///Becase the curren VRE version only using 14 bits for decode phase, so we must append 2 bits at the
		/// end of the
		int start_az = (phase&0x3FFF)<<2; /// 14 bit phase
		int end_az = start_az;//((phase&0x3FF)<<2)|0x0003; ///
		int start_range = 0;
		long cell_duration = (1000000000.0f/sampling_rate);			///10^-9 LSB - in nano second
		uint8_t video_header_nano[CAT240_LENGTH_VIDEO_HEADER];

		video_header_nano[1] = (start_az&0xFF);
		video_header_nano[0] = (start_az>>8)&0xFF;

		video_header_nano[3] = (end_az)&0xFF;
		video_header_nano[2] = (end_az>>8)&0xFF;

		video_header_nano[7] = start_range&0xFF;
		video_header_nano[6] = (start_range>>8)&0xFF;
		video_header_nano[5] = (start_range>>16)&0xFF;
		video_header_nano[4] = (start_range>>24)&0xFF;

		video_header_nano[11] = (cell_duration)&0xFF;
		video_header_nano[10] = (cell_duration>>8)&0xFF;
		video_header_nano[9] = (cell_duration>>16)&0xFF;
		video_header_nano[8] = (cell_duration>>24)&0xFF;

		ms_mem(cpy)(msg+current_msg_index,video_header_nano, CAT240_LENGTH_VIDEO_HEADER);
		current_msg_index += CAT240_LENGTH_VIDEO_HEADER;
	}

	{
		/// Assigned the video cell resolution
		uint8_t video_cell_resolution[CAT240_LENGTH_VIDEO_CELL_RESOLUTION];
		video_cell_resolution[0] = 0;/// Spare
		video_cell_resolution[1] = CAT240_VIDEO_RESOLUTION;
		ms_mem(cpy)(msg + current_msg_index, video_cell_resolution, CAT240_LENGTH_VIDEO_CELL_RESOLUTION);
		current_msg_index += CAT240_LENGTH_VIDEO_CELL_RESOLUTION;
	}

	{
		/// Assigned video block count
		uint8_t video_block_counter_data[CAT240_V12_LENGTH_VIDEO_BLOCK_COUNTER];
		video_block_counter_data[1] = video_octet_counter&0xFF;
		video_block_counter_data[0] = (video_octet_counter>>8)&0xFF;

		video_block_counter_data[4] = (range_in_point)&0xFF;
		video_block_counter_data[3] = (range_in_point>>8)&0xFF;
		video_block_counter_data[2] = (range_in_point>>16)&0xFF;
		ms_mem(cpy)(msg+ current_msg_index, video_block_counter_data, CAT240_V12_LENGTH_VIDEO_BLOCK_COUNTER);
		current_msg_index += CAT240_V12_LENGTH_VIDEO_BLOCK_COUNTER;
	}

	{
		/// Assign repetition
		msg[current_msg_index] = rep;
		current_msg_index += 1;

		switch( CAT240_VIDEO_RESOLUTION)
		{
		case CAT240_VIDEO_RESOLUTION_1_BITS:
		{

		}
		break;
		case CAT240_VIDEO_RESOLUTION_2_BITS:
		{

		}
		break;
		case CAT240_VIDEO_RESOLUTION_4_BITS:
		{

		}
		break;
		case CAT240_VIDEO_RESOLUTION_8_BITS:
		{
			ms_mem(cpy)(msg+current_msg_index,video_data, range_in_point);
			current_msg_index += range_in_point;
		}
		break;
		case CAT240_VIDEO_RESOLUTION_16_BITS:
		{

		}
		break;
		case CAT240_VIDEO_RESOLUTION_32_BITS:
		{

		}
		break;

		default:
			break;
		}
	}
	if( sp != 0xFF )
	{
		msg[current_msg_index] = sp;
	}

	*cat240_message = msg;
}
void coding_cat240_v12_video_summary_message( unsigned char *summary, int summary_lenth, unsigned char **cat240_message, int *message_length)
{
	coding_cat240_video_summary_message(summary, summary_lenth, cat240_message,message_length);
}

void coding_cat034_north_marker(unsigned char **cat034, int *msg_length)
{
	/*
	 * This message only contains the mandatory field.
	 * There are three fields that is mandatory:
	 * 1. Message Type
	 * 2. Data Source Identifier
	 * 3. Time of Day
	 */
	int _cat034BodyDataLength;
	uint8_t *_cat034BodyData;
	int _cat034_BodyDataFillingIndex;
	const int LENGTH_OF_FRN_FIELD = 1;// This field is base on the mandatory field
	_cat034BodyDataLength = ASTERIX_HEADER_LENGTH
			+ CAT034_LENGTH_MESSAGE_TYPE
			+ CAT034_LENGTH_DATA_SOURCE_IDENTIFIER
			+ CAT034_LENGTH_TIME_OF_DAY
			+ LENGTH_OF_FRN_FIELD;

	_cat034BodyData = (unsigned char*)malloc(_cat034BodyDataLength);

	{
		//Filing header
		Init_Header(_cat034BodyData,_cat034BodyDataLength,CAT034_VALUE);
		_cat034_BodyDataFillingIndex = ASTERIX_HEADER_LENGTH;
	}
	{
		//Create the Field Reference Number for mandatory Fields
		uint8_t _cat034FRN = 0;

		Set_Fspec(_cat034FRN,CAT034_FRN_DATA_MESSAGE_TYPE);
		Set_Fspec(_cat034FRN,CAT034_FRN_DATA_SOURCE_IDENTIFIER);
		Set_Fspec(_cat034FRN,CAT034_FRN_TIME_OF_DAY);

		_cat034BodyData[_cat034_BodyDataFillingIndex] = _cat034FRN;
		_cat034_BodyDataFillingIndex = _cat034_BodyDataFillingIndex + 1;// only one byte
	}
	{
		///Data Source identifier
		uint8_t data_source_identifier[CAT034_LENGTH_DATA_SOURCE_IDENTIFIER];
		data_source_identifier[0] = (DATA_SOURCE_IDENTIFIER_VALUE>>8)&0xFF;
		data_source_identifier[1] = DATA_SOURCE_IDENTIFIER_VALUE&0xFF;
		ms_mem(cpy)(_cat034BodyData+_cat034_BodyDataFillingIndex,data_source_identifier,CAT034_LENGTH_DATA_SOURCE_IDENTIFIER);
		_cat034_BodyDataFillingIndex += CAT034_LENGTH_DATA_SOURCE_IDENTIFIER;
	}
	{
		///Filling the message type
		_cat034BodyData[_cat034_BodyDataFillingIndex] = CAT034_MESSAGE_TYPE_NORTH_MARK;
		_cat034_BodyDataFillingIndex += CAT034_LENGTH_MESSAGE_TYPE;
	}
	{
		///Filling time of day
		long long int _elapsedSinceMidnight;
		const int LSB_TIME_OF_DAY = 128;
		get_elapsed_since_midnight(&_elapsedSinceMidnight);

		_elapsedSinceMidnight = (LSB_TIME_OF_DAY*((double)_elapsedSinceMidnight/1000000.0));

		_cat034BodyData[_cat034_BodyDataFillingIndex] = (_elapsedSinceMidnight>>16)&0xFF;
		_cat034BodyData[_cat034_BodyDataFillingIndex+1] = (_elapsedSinceMidnight>>8)&0xFF;
		_cat034BodyData[_cat034_BodyDataFillingIndex+2] = (_elapsedSinceMidnight>>0)&0xFF;

	}

	*msg_length = _cat034BodyDataLength;
	*cat034 = _cat034BodyData;
}


void coding_cat034_sector_crossing( unsigned int azimuth, unsigned char **cat034, int *msg_length)
{
	/*
	 * This message only contains the mandatory field.
	 * There are three fields those is mandatory:
	 * 1. Message Type
	 * 2. Data Source Identifier
	 * 3. Sector Number
	 * 4. Time of Day
	 */
	int _cat034BodyDataLength;
	uint8_t *_cat034BodyData;
	int _cat034_BodyDataFillingIndex;
	const int LENGTH_OF_FRN_FIELD = 1;// This field is base on the mandatory field

	_cat034BodyDataLength = ASTERIX_HEADER_LENGTH
			+ CAT034_LENGTH_MESSAGE_TYPE
			+ CAT034_LENGTH_DATA_SOURCE_IDENTIFIER
			+ CAT034_LENGTH_TIME_OF_DAY
			+ CAT034_LENGTH_SECTOR_NUMBER
			+ LENGTH_OF_FRN_FIELD;

	_cat034BodyData = (unsigned char*)malloc(_cat034BodyDataLength);

	{
		//Filing header
		Init_Header(_cat034BodyData,_cat034BodyDataLength,CAT034_VALUE);
		_cat034_BodyDataFillingIndex = ASTERIX_HEADER_LENGTH;
	}
	{
		//Create the Field Reference Number for mandatory Fields
		uint8_t _cat034FRN = 0;

		Set_Fspec(_cat034FRN,CAT034_FRN_DATA_MESSAGE_TYPE);
		Set_Fspec(_cat034FRN,CAT034_FRN_DATA_SOURCE_IDENTIFIER);
		Set_Fspec(_cat034FRN,CAT034_FRN_TIME_OF_DAY);
		Set_Fspec(_cat034FRN,CAT034_FRN_SECTOR_NUMBER);

		_cat034BodyData[_cat034_BodyDataFillingIndex] = _cat034FRN;
		_cat034_BodyDataFillingIndex = _cat034_BodyDataFillingIndex + 1;// only one byte
	}
	{
		///Data Source identifier
		uint8_t data_source_identifier[CAT034_LENGTH_DATA_SOURCE_IDENTIFIER];
		data_source_identifier[0] = (DATA_SOURCE_IDENTIFIER_VALUE>>8)&0xFF;
		data_source_identifier[1] = DATA_SOURCE_IDENTIFIER_VALUE&0xFF;
		ms_mem(cpy)(_cat034BodyData+_cat034_BodyDataFillingIndex,data_source_identifier,CAT034_LENGTH_DATA_SOURCE_IDENTIFIER);
		_cat034_BodyDataFillingIndex += CAT034_LENGTH_DATA_SOURCE_IDENTIFIER;
	}
	{
		///Filling the message type
		_cat034BodyData[_cat034_BodyDataFillingIndex] = CAT034_MESSAGE_TYPE_SECTOR_CROSSING;
		_cat034_BodyDataFillingIndex += CAT034_LENGTH_MESSAGE_TYPE;
	}
	{
		///Filling time of day
		long long int _elapsedSinceMidnight;
		const int LSB_TIME_OF_DAY = 128;
		get_elapsed_since_midnight(&_elapsedSinceMidnight);

		_elapsedSinceMidnight = (LSB_TIME_OF_DAY*((double)_elapsedSinceMidnight/1000000.0));

		_cat034BodyData[_cat034_BodyDataFillingIndex] = (_elapsedSinceMidnight>>16)&0xFF;
		_cat034BodyData[_cat034_BodyDataFillingIndex+1] = (_elapsedSinceMidnight>>8)&0xFF;
		_cat034BodyData[_cat034_BodyDataFillingIndex+2] = (_elapsedSinceMidnight>>0)&0xFF;

		_cat034_BodyDataFillingIndex = _cat034_BodyDataFillingIndex + CAT034_LENGTH_TIME_OF_DAY;
	}
	{
		const double SECTOR_SCALE_FROM_360_TO_256 = 1.40625;
		/// Sector Number
		_cat034BodyData[_cat034_BodyDataFillingIndex] = (unsigned char)(azimuth/SECTOR_SCALE_FROM_360_TO_256);
		//_cat034_BodyDataFillingIndex = _cat034_BodyDataFillingIndex + CAT034_LENGTH_SECTOR_NUMBER;
	}

	*msg_length = _cat034BodyDataLength;
	*cat034 = _cat034BodyData;
}

// modify by thanhvn : 26/02/2020
void coding_cat048_plot_report( int plot_phase, int plot_range,unsigned char type_of_plot, unsigned char **cat048, int *msg_length)
{
	/*
	 * This message only contains the mandatory field.
	 * There are three fields that is mandatory:
	 * 1. Data Source Identifier
	 * 2. Time of Day
	 * 3. Measured Position
	 * 4. Target Report Descriptor
	 */
	int _cat048BodyDataLength;
	uint8_t *_cat048BodyData;
	int _cat048_BodyDataFillingIndex;
	const int LENGTH_OF_FRN_FIELD = 2;// This field is base on the mandatory field

	_cat048BodyDataLength = ASTERIX_HEADER_LENGTH  //3
			+ CAT048_LENGTH_DATA_SOURCE_IDENTIFIER //2
			+ CAT048_LENGTH_TIME_OF_DAY 			//3
			+ CAT048_LENGTH_TARGET_REPORT_DESCRIPTOR //2
			+ CAT048_LENGTH_POSITION_IN_POLAR		//4
			+ LENGTH_OF_FRN_FIELD					//2
			+ CAT048_LENGTH_TRACK_NUMBER;			//2

	_cat048BodyData = (unsigned char*)malloc(_cat048BodyDataLength);

	{
		//Filing header
		Init_Header(_cat048BodyData,_cat048BodyDataLength,CAT048_VALUE);
		_cat048_BodyDataFillingIndex = ASTERIX_HEADER_LENGTH;
	}
	{
		//Create the Field Reference Number for mandatory Fields
		uint8_t _cat048FRN = 0;

		Set_Fspec(_cat048FRN,CAT048_FRN_DATA_SOURCE_IDENTIFIER);
		Set_Fspec(_cat048FRN,CAT048_FRN_TIME_OF_DAY);
		Set_Fspec(_cat048FRN,CAT048_FRN_POSITION_IN_POLAR);
		Set_Fspec(_cat048FRN,CAT048_FRN_TARGET_REPORT_DESCRIPTOR);
		Set_Fspec(_cat048FRN,ASTERIX_FRN_FX);

		_cat048BodyData[_cat048_BodyDataFillingIndex] = _cat048FRN;
		_cat048_BodyDataFillingIndex = _cat048_BodyDataFillingIndex + 1;// only one byte

		_cat048FRN=0;
		Set_Fspec(_cat048FRN,CAT048_FRN_TRACK_NUMBER);
		_cat048BodyData[_cat048_BodyDataFillingIndex] = _cat048FRN;
		_cat048_BodyDataFillingIndex = _cat048_BodyDataFillingIndex + 1;// only one byte
	}
	{
		///Data Source identifier
		uint8_t data_source_identifier[CAT048_LENGTH_DATA_SOURCE_IDENTIFIER];
		data_source_identifier[0] = (DATA_SOURCE_IDENTIFIER_VALUE>>8)&0xFF;
		data_source_identifier[1] = DATA_SOURCE_IDENTIFIER_VALUE&0xFF;
		ms_mem(cpy)(_cat048BodyData+_cat048_BodyDataFillingIndex,data_source_identifier,CAT048_LENGTH_DATA_SOURCE_IDENTIFIER);
		_cat048_BodyDataFillingIndex += CAT048_LENGTH_DATA_SOURCE_IDENTIFIER;
	}
	{
		///Filling time of day
		long long int _elapsedSinceMidnight;
		const int LSB_TIME_OF_DAY = 128;
		get_elapsed_since_midnight(&_elapsedSinceMidnight);

		_elapsedSinceMidnight = (LSB_TIME_OF_DAY*((double)_elapsedSinceMidnight/1000000.0));

		_cat048BodyData[_cat048_BodyDataFillingIndex] = (_elapsedSinceMidnight>>16)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+1] = (_elapsedSinceMidnight>>8)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+2] = (_elapsedSinceMidnight>>0)&0xFF;

		_cat048_BodyDataFillingIndex = _cat048_BodyDataFillingIndex + CAT048_LENGTH_TIME_OF_DAY;
	}

	// add by thanhvn
	// add type_of_plot infomation
	{
		uint8_t target_report_descriptor[CAT048_LENGTH_TARGET_REPORT_DESCRIPTOR];
		target_report_descriptor[0] = 0x01;  				// FX = 1 : Extension into next extent
		if (type_of_plot == 0x05) {
			target_report_descriptor[0] |= 0b01000000;		// type of plot == 0x05 =>> second surveilance radar (NPA)
		}
		target_report_descriptor[1] = 0x00;
		if((type_of_plot == 0x01) || (type_of_plot == 0x02) || (type_of_plot == 0x04)){
			target_report_descriptor[1] &= ~(0x01 << 2); 	// bits-3/2  == 0x01 : Friendly target
			target_report_descriptor[1] |= 	(0x01 << 1);
			target_report_descriptor[1] |= 	(0x01 << 3);	// Military identification
		}else{
			target_report_descriptor[1] |= 	(0x01 << 2); 	// bits-3/2  == 0x11 : No Reply
			target_report_descriptor[1] |= 	(0x01 << 1);
			target_report_descriptor[1] &= ~(0x01 << 3);	// Military identification
		}
		ms_mem(cpy)(_cat048BodyData+_cat048_BodyDataFillingIndex,target_report_descriptor,CAT048_LENGTH_TARGET_REPORT_DESCRIPTOR);
		_cat048_BodyDataFillingIndex += CAT048_LENGTH_TARGET_REPORT_DESCRIPTOR;
	}

	//
	{
		const int NAUTICAL_MILE_TO_METTER = 1852;
		const int RANGE_LSB=256;
		plot_range = (plot_range*RANGE_LSB)/NAUTICAL_MILE_TO_METTER; //Convert the range from meter to nautical mile unit
		/// Sector Number
		_cat048BodyData[_cat048_BodyDataFillingIndex+0] = (plot_range>>8)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+1] = (plot_range>>0)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+2] = (plot_phase>>8)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+3] = (plot_phase>>0)&0xFF;

		_cat048_BodyDataFillingIndex = _cat048_BodyDataFillingIndex + CAT048_LENGTH_POSITION_IN_POLAR;
	}

	{
		_cat048BodyData[_cat048_BodyDataFillingIndex] = 0;
		_cat048BodyData[_cat048_BodyDataFillingIndex + 1] = 0;
	}

	*msg_length = _cat048BodyDataLength;
//	printf("msglen:  %hhx   %d",_cat048BodyData[0],_cat048BodyDataLength);
	*cat048 = _cat048BodyData;
}

void coding_cat048_plot_3d_report( int plot_phase, int plot_range, int plot_altitude, unsigned char **cat048, int *msg_length)
{
	/*
	 * This message only contains the mandatory field.
	 * There are three fields that is mandatory:
	 * 1. Data Source Identifier
	 * 2. Time of Day
	 * 3. Measured Position
	 */
	int _cat048BodyDataLength;
	uint8_t *_cat048BodyData;
	int _cat048_BodyDataFillingIndex;
	const int LENGTH_OF_FRN_FIELD = 1;// This field is base on the mandatory field

	_cat048BodyDataLength = ASTERIX_HEADER_LENGTH
			+ CAT048_LENGTH_DATA_SOURCE_IDENTIFIER
			+ CAT048_LENGTH_TIME_OF_DAY
			+ CAT048_LENGTH_POSITION_IN_POLAR
			+ CAT048_LENGTH_HEIGHT_MEASURE_3D
			+ CAT048_LENGTH_TRACK_NUMBER
			+ LENGTH_OF_FRN_FIELD*3; ///There are three RRN in this frame

	_cat048BodyData = (unsigned char*)malloc(_cat048BodyDataLength);

	{
		//Filing header
		Init_Header(_cat048BodyData,_cat048BodyDataLength,CAT048_VALUE);
		_cat048_BodyDataFillingIndex = ASTERIX_HEADER_LENGTH;
	}
	{
		//Create the Field Reference Number for mandatory Fields
		//First frame
		uint8_t _cat048FRN = 0;
		Set_Fspec(_cat048FRN,CAT048_FRN_DATA_SOURCE_IDENTIFIER);
		Set_Fspec(_cat048FRN,CAT048_FRN_TIME_OF_DAY);
		Set_Fspec(_cat048FRN,CAT048_FRN_POSITION_IN_POLAR);
		Set_Fspec(_cat048FRN,ASTERIX_FRN_FX);

		_cat048BodyData[_cat048_BodyDataFillingIndex] = _cat048FRN;
		_cat048_BodyDataFillingIndex = _cat048_BodyDataFillingIndex + 1;// only one byte

		///second frame
		_cat048FRN = 0;
		Set_Fspec(_cat048FRN,CAT048_FRN_TRACK_NUMBER);
		Set_Fspec(_cat048FRN,ASTERIX_FRN_FX);
		_cat048BodyData[_cat048_BodyDataFillingIndex] = _cat048FRN;
		_cat048_BodyDataFillingIndex = _cat048_BodyDataFillingIndex + 1;// only one byte

		///Third frame
		_cat048FRN = 0;
		Set_Fspec(_cat048FRN,CAT048_FRN_HEIGHT_MEASURE_3D);
		_cat048BodyData[_cat048_BodyDataFillingIndex] = _cat048FRN;
		_cat048_BodyDataFillingIndex = _cat048_BodyDataFillingIndex + 1;// only one byte
	}
	{
		///Data Source identifier
		uint8_t data_source_identifier[CAT048_LENGTH_DATA_SOURCE_IDENTIFIER];
		data_source_identifier[0] = (DATA_SOURCE_IDENTIFIER_VALUE>>8)&0xFF;
		data_source_identifier[1] = DATA_SOURCE_IDENTIFIER_VALUE&0xFF;
		ms_mem(cpy)(_cat048BodyData+_cat048_BodyDataFillingIndex,data_source_identifier,CAT048_LENGTH_DATA_SOURCE_IDENTIFIER);
		_cat048_BodyDataFillingIndex += CAT048_LENGTH_DATA_SOURCE_IDENTIFIER;
	}
	{
		///Filling time of day
		long long int _elapsedSinceMidnight;
		const int LSB_TIME_OF_DAY = 128;
		get_elapsed_since_midnight(&_elapsedSinceMidnight);

		_elapsedSinceMidnight = (LSB_TIME_OF_DAY*((double)_elapsedSinceMidnight/1000000.0));

		_cat048BodyData[_cat048_BodyDataFillingIndex] = (_elapsedSinceMidnight>>16)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+1] = (_elapsedSinceMidnight>>8)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+2] = (_elapsedSinceMidnight>>0)&0xFF;

		_cat048_BodyDataFillingIndex = _cat048_BodyDataFillingIndex + CAT048_LENGTH_TIME_OF_DAY;
	}

	{
		const int NAUTICAL_MILE_TO_METTER = 1852;
		const int RANGE_LSB=256;
		plot_range = (plot_range*RANGE_LSB)/NAUTICAL_MILE_TO_METTER; //Convert the range from meter to nautical mile unit
		/// Sector Number
		_cat048BodyData[_cat048_BodyDataFillingIndex+0] = (plot_range>>8)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+1] = (plot_range>>0)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+2] = (plot_phase>>8)&0xFF;
		_cat048BodyData[_cat048_BodyDataFillingIndex+3] = (plot_phase>>0)&0xFF;

		_cat048_BodyDataFillingIndex = _cat048_BodyDataFillingIndex + CAT048_LENGTH_POSITION_IN_POLAR;
	}
	{
		_cat048BodyData[_cat048_BodyDataFillingIndex] = 0;
		_cat048BodyData[_cat048_BodyDataFillingIndex + 1] = 0;
		_cat048_BodyDataFillingIndex += CAT048_LENGTH_TRACK_NUMBER;
	}
	{
		const double HIGHT_3D_METTER_TO_FOOT = 3.28084;
		const int ALTITUDE_LSB=25;///1 LSB = 25ft
		plot_altitude = (plot_range*HIGHT_3D_METTER_TO_FOOT)/ALTITUDE_LSB; //Convert the range from meter to nautical mile unit

		_cat048BodyData[_cat048_BodyDataFillingIndex+0] = (plot_altitude>>8)&0x7F;
		_cat048BodyData[_cat048_BodyDataFillingIndex+1] = (plot_altitude>>0)&0xFF;

	}

	*msg_length = _cat048BodyDataLength;
	*cat048 = _cat048BodyData;
}
