/*
 * iff_clone.cpp
 *
 *  Created on: Oct 21, 2020
 *      Author:Minh
 */

#include "iff_clone.h"

IffInfoT Iff_info_array[CONST_MAX_IFF_RANGE];

static int isTarget(IffInfoT * IffInfo);
static int check_with_of_range(IffInfoT * IffInfo);
static int check_with_of_phase(IffInfoT * IffInfo);

int iff_thres_update_range = 2;
int iff_range_thres_min = 0;
int iff_range_thres_max = 2;
int iff_phase_thres_max = 500;
int iff_phase_thres_min = 30;
int with_of_phase;

IffTargetT IffTarget;
unsigned int center_of_range;
unsigned int center_of_phase;

int target_ready=0;

void iff_init() {
	printf("\nIFF init ....\n");
	memset(Iff_info_array, 0, (CONST_MAX_IFF_RANGE) * sizeof(IffInfoT));
}

void iff_processing(mTraceData RawData) {
	int i, j;
	struct timespec tm;
	int isEmpty = 0, IndEmpty = 0, isUpdating = 0, isDataAvail = 0;

	unsigned int nzPos[50]; // non zero position in array 150 bytes data
	int nzPosIndex = 0;

	for (int i = 0; i < ELEMENT_SIZE; i++) {
		if (RawData.data[i]
				== SystemConfigurations::getInstance()->getFindChar()) {
			nzPos[nzPosIndex] = i;
			nzPosIndex++;
		}
	}

	for (j = 0; j < CONST_MAX_IFF_RANGE; j++) {
		if (Iff_info_array[j].isDetecing == 1) {
			Iff_info_array[j].counter++;

		}
	}

	//	if (nzPosIndex == 0)
	//		return; // no data to process

	for (i = 0; i < nzPosIndex; i++) { // quet du lieu ben trong mang RawData
		//		if(RawData->iff_pulse[i] == 0) break;	// thoat vong lap neu nhu khong con du lieu

		isEmpty = 0;
		IndEmpty = 0;
		isUpdating = 0;
		isDataAvail = 1;

		for (j = 0; j < CONST_MAX_IFF_RANGE; j++) { // quet du lieu ben trong mang Iff_info_array
			if (Iff_info_array[j].isDetecing == 1) { //

				if ((abs(nzPos[i] - Iff_info_array[j].range_end)
						< iff_thres_update_range) || // Check dieu kien update du lieu
						(abs(nzPos[i] - Iff_info_array[j].range_start)
								< iff_thres_update_range)) {

					//					printf("\n[IFF Processing] Updating data at index %d\n", j);

					Iff_info_array[j].range_start
							= Iff_info_array[j].range_start < nzPos[i] ? Iff_info_array[j].range_start
									: nzPos[i];
					Iff_info_array[j].range_end = Iff_info_array[j].range_end
							> nzPos[i] ? Iff_info_array[j].range_end : nzPos[i];
					Iff_info_array[j].phase_end = RawData.az;
					Iff_info_array[j].counter = 0;
					isUpdating = 1;
					Iff_info_array[j].size++;

					break;
				}
			} else {
				if (isEmpty == 0) {
					isEmpty = 1;
					IndEmpty = j;
				}

			}
		}

		//		printf("\n\n\n%d\n\n\n",IndEmpty);

		if ((isUpdating == 0) && (isDataAvail == 1)) { // neu nhu khong update du lieu vao Iff_info_array ma can phai ghi mot Info moi
			if (isEmpty == 1) {

				//				printf("\n[IFF Processing] Add new data at index %d\n",
				//						IndEmpty);

				Iff_info_array[IndEmpty].range_start = nzPos[i];
				Iff_info_array[IndEmpty].range_end = nzPos[i];
				Iff_info_array[IndEmpty].phase_start = RawData.az;
				Iff_info_array[IndEmpty].phase_end = RawData.az;
				Iff_info_array[IndEmpty].counter = 0;
				Iff_info_array[IndEmpty].isDetecing = 1;
			} else {
				//				printf("\n[IFF Processing] Queue full\n");
			}
		}

	}

	// check dieu kien ket thuc iff plot
	for (i = 0; i < CONST_MAX_IFF_RANGE; i++) {
		//		if(i==0)printf("%d\n",Iff_info_array[0].counter);
		if (Iff_info_array[i].counter >= 10) {
			while (target_ready != 0)
				usleep(100);
			if (isTarget(&Iff_info_array[i]) == 1) {
				// create IFF Plot
				target_ready = 1;

				IffTarget.center_of_phase = center_of_phase;
				IffTarget.center_of_range = center_of_range;

//				printf("range_start = %d %d\n", Iff_info_array[i].range_start,
//						i);
//				printf("range_end = %d\n", Iff_info_array[i].range_end);
//				printf("phase_start = %d\n", Iff_info_array[i].phase_start);
//				printf("phase_end = %d\n", Iff_info_array[i].phase_end);
//				printf("size= %d\n", Iff_info_array[i].size);
//				printf("[IFF PLOT] Range: %u, Phase: %u\n", center_of_range,
//						center_of_phase);

				//				msg_create_plot_36D6(iff_message , IffTarget.center_of_range, IffTarget.center_of_phase, OO_IFF);


				clock_gettime(CLOCK_REALTIME, &tm);
				tm.tv_sec += 1;
				//				if (0 > mq_timedsend(config_params.mq_plot, (char *)iff_message, MSG_36D6_PLOT_LEN, 0, &tm)) {
				//					printf("Timeout, plot channel %s is full\n", MQ_PLOT_QUEUE_NAME);
				//				}
			}
			// clear Info
			Iff_info_array[i].range_start = 0;
			Iff_info_array[i].range_end = 0;
			Iff_info_array[i].phase_start = 0;
			Iff_info_array[i].phase_end = 0;
			Iff_info_array[i].isDetecing = 0;
			Iff_info_array[i].counter = 0;
			Iff_info_array[i].size = 0;
		}
	}
}

static int isTarget(IffInfoT * IffInfo) {

	if ((check_with_of_range(IffInfo) == 1) && (check_with_of_phase(IffInfo)
			== 1)) {
		return 1;
	} else {
		return 0;
	}

}

static int check_with_of_range(IffInfoT * IffInfo) { // kiem tra dieu kien range
	unsigned short with_of_range = 0;

	with_of_range = IffInfo->range_end - IffInfo->range_start;
	if (with_of_range >= iff_range_thres_min && with_of_range
			<= iff_range_thres_max) {
		center_of_range = IffInfo->range_start + (with_of_range >> 1);
		//		center_of_range = center_of_range * RANGE_STEP_SIZE;
		return 1;
	} else {
		return 0;
	}
}

static int check_with_of_phase(IffInfoT * IffInfo) { /// kiem tra dieu  kien phase
	unsigned short with_of_phase = 0;

	with_of_phase
			= (IffInfo->phase_end > IffInfo->phase_start) ? (IffInfo->phase_end
					- IffInfo->phase_start) : (IffInfo->phase_end + (6000
					- IffInfo->phase_start));

	if ((with_of_phase >= iff_phase_thres_min) && (with_of_phase
			<= iff_phase_thres_max)) {
		center_of_phase = IffInfo->phase_start + (with_of_phase >> 1);
		center_of_phase = (center_of_phase <= 6000) ? center_of_phase
				: (center_of_phase - 6000);
		//		center_of_phase = center_of_phase >> 2;
		return 1;
	} else {
		return 0;
	}
}

