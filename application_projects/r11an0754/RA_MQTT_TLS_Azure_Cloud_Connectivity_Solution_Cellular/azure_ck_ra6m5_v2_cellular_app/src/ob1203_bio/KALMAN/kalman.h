#ifndef __KALMAN_H__
#define __KALMAN_H__
#include <stdint.h>

#include "ob1203_bio/SPO2/SPO2.h"


#define FIXED_BITS 3
#define NO_JUMPS 0
#define JUMPS_OK 1

#define MIN_DATA_STD 4<<FIXED_BITS
#define HR_MIN_STD_1F 8<<FIXED_BITS
#define RR_MIN_STD_1F 4<<FIXED_BITS //this is the breath period in intervals. FOr example 70 internal with 100 sample rate means we are looking for std_dev of less than 4 intervals or about 3 seconds * threshold.
#define RR_KALMAN_THRESHOLD_2X 5
#define ZC_MIN_STD_1F 8<<FIXED_BITS
#define ZC_KALMAN_THRESHOLD_2X 5
#define SPO2_MIN_STD_1F 4<<FIXED_BITS
#define SPO2_KALMAN_THRESHOLD_2X 4

#define B2B_KALMAN_THRESHOLD_2X 5
#define B2B_MIN_STD_1F 7<<FIXED_BITS

#define RR_KALMAN_LENGTH 13 //5 second average
#define RR_DATA_LENGTH 30
#define RR_MAX_OUTLIER_COUNT 18
#define RR_MAX_ALG_FAIL_COUNT 18

#define MAX_OUTLIER_COUNT 3
#define MAX_ALG_FAIL_COUNT 3

#ifdef COMPUTE_RR //defined in SPO2.h
	#define ZC_KALMAN_LENGTH 10
	#define ZC_DATA_LENGTH 20
	#define ZC_MAX_OUTLIER_COUNT 5
	#define ZC_MAX_ALG_FAIL_COUNT 5

	#define HR_KALMAN_LENGTH 20//number of algorithm runs to make a running average over (20*0.4 = 8 seconds)
	#define HR_DATA_LENGTH 25 //a little longer to include sinus rhythm variation which can be large
	#define HR_KALMAN_THRESHOLD_2X  5//the multiplier for the standard deviation to use for the kalman filter
	#define HR_MAX_OUTLIER_COUNT 5
	#define HR_MAX_ALG_FAIL_COUNT 5

	#define SPO2_KALMAN_LENGTH 20 //length of algorithm runs (e.g. x0.4 sec) that are averaged on the display (e.g. 4 secs)
	#define SPO2_DATA_LENGTH 25
	#define SPO2_MAX_OUTLIER_COUNT 5
	#define SPO2_MAX_ALG_FAIL_COUNT 5
#else
	#define ZC_KALMAN_LENGTH 4
	#define ZC_DATA_LENGTH 8
	#define ZC_MAX_OUTLIER_COUNT 3
	#define ZC_MAX_ALG_FAIL_COUNT 3

	#define HR_KALMAN_LENGTH 8 //number of points to make a running average over
	#define HR_DATA_LENGTH 15
	#define HR_KALMAN_THRESHOLD_2X  5//the multiplier for the standard deviation to use for the kalman filter
	#define HR_MAX_OUTLIER_COUNT 3
	#define HR_MAX_ALG_FAIL_COUNT 3

	#define SPO2_KALMAN_LENGTH 8 //length of algorithm runs (e.g. x0.4 sec) that are averaged on the display (10 secs)
	#define SPO2_DATA_LENGTH 8
	#define SPO2_MAX_OUTLIER_COUNT 3
	#define SPO2_MAX_ALG_FAIL_COUNT 3
#endif

#define NO_RATIO 0
#define USE_RATIO 1

#define KF kalman_filters[num] //shorthand notation to make code readable

//make an array of structs for kalman

/*variables*/
typedef struct kalman {
	uint8_t kalman_length;
	uint16_t *kalman_array;
	uint8_t kalman_ind;
	uint16_t kalman_avg;
	uint16_t *data_array;
	uint8_t data_array_length;
	uint8_t data_ind;
	uint8_t outlier_cnt;
	uint8_t alg_fail_cnt;
	uint8_t max_outlier_cnt;
	uint8_t max_alg_fail_cnt;
	uint8_t max_kalman_length;
	uint8_t max_data_length;
	uint8_t min_data_std;
	uint8_t kalman_threshold_2x;
	uint8_t do_reset_kalman;
	uint16_t data_std;
	uint16_t big_avg;
	//  uint32_t data_std_out;
	//  uint32_t data_mean_out;
	uint8_t jumps_ok; //use this for spo2 to allow upward jumps of up to 2x the usual threshold during fast SpO2 recovery
}kalman_t;


#ifdef COMPUTE_RR
#define NUM_KALMAN_FILTERS 5
#else
#define NUM_KALMAN_FILTERS 4
#endif

enum filters {zc_filt,hr_filt,spo2_filt,b2b_filt,rr_filt,};
extern kalman_t kalman_filters[NUM_KALMAN_FILTERS]; //declare an array of kalman filter structs
extern const uint8_t max_outlier_cnt_init[5];

/*functions*/
void init_kalman(kalman_t* kalman_filters);
void reset_kalman(uint8_t filter_num);
void run_kalman(uint8_t filter_num, uint32_t new_data, uint8_t ratio);
uint16_t get_std_dev(uint16_t *array, uint8_t array_length);
uint16_t get_avg(uint16_t *array, uint8_t array_length);
void get_big_avg(uint8_t filter_num);

#endif
