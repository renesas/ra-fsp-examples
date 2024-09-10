/*Description: This is a simple zero order Kalman filter that is designed to take an average of datapoints within
 * a useful range of variance. Outliers are not included in the kalman average but are used to calculate the signal
 * variance.
 * It is called recursively and can be set to allow large "jumps" upward in signal, as well do a ratiometric
 * comparison (relative variance).
 * It can also provide a useful "big_avg" which is the average of all signals more than half the size of the
 * max. This is useful for excluding repeated low signals which would otherwise confuse the Kalman by
 * widening the variance limits until the false signals were allowable.*/

#if defined(DEBUG)
#include <stdio.h>
#endif
#include <string.h>
#include <math.h>
#include "KALMAN.h"

const uint8_t max_alg_fail_cnt_init[] = { ZC_MAX_ALG_FAIL_COUNT, HR_MAX_ALG_FAIL_COUNT , SPO2_MAX_ALG_FAIL_COUNT ,ZC_MAX_ALG_FAIL_COUNT, RR_MAX_ALG_FAIL_COUNT };
const uint8_t max_outlier_cnt_init[] = {ZC_MAX_OUTLIER_COUNT , HR_MAX_OUTLIER_COUNT,  SPO2_MAX_OUTLIER_COUNT , ZC_MAX_OUTLIER_COUNT,RR_MAX_OUTLIER_COUNT};
const uint8_t max_data_length_init[] = {ZC_DATA_LENGTH, HR_DATA_LENGTH, SPO2_DATA_LENGTH, ZC_DATA_LENGTH, RR_DATA_LENGTH};
const uint8_t max_kalman_length_init[] = { ZC_KALMAN_LENGTH, HR_KALMAN_LENGTH, SPO2_KALMAN_LENGTH, ZC_KALMAN_LENGTH,RR_KALMAN_LENGTH};
const uint8_t min_data_std_init[] = { ZC_MIN_STD_1F , HR_MIN_STD_1F , SPO2_MIN_STD_1F , B2B_MIN_STD_1F,RR_MIN_STD_1F};
const uint8_t kalman_threshold_2x_init[] = { ZC_KALMAN_THRESHOLD_2X , HR_KALMAN_THRESHOLD_2X , SPO2_KALMAN_THRESHOLD_2X , B2B_KALMAN_THRESHOLD_2X,RR_KALMAN_THRESHOLD_2X};
const uint8_t jumps_ok_init[] = {NO_JUMPS, NO_JUMPS, JUMPS_OK, NO_JUMPS, NO_JUMPS};

kalman_t kalman_filters[NUM_KALMAN_FILTERS]; //declare an array of kalman filter structs

uint16_t array0[ZC_KALMAN_LENGTH];
uint16_t array1[HR_KALMAN_LENGTH];
uint16_t array2[SPO2_KALMAN_LENGTH];
uint16_t array3[ZC_KALMAN_LENGTH]; //B2B is same length as ZC
uint16_t array4[RR_KALMAN_LENGTH];
uint16_t array5[ZC_DATA_LENGTH];
uint16_t array6[HR_DATA_LENGTH];
uint16_t array7[SPO2_DATA_LENGTH];
uint16_t array8[ZC_DATA_LENGTH]; //B2B is same length as ZC
uint16_t array9[RR_DATA_LENGTH];


void reset_kalman(uint8_t filter_num) {
  kalman_filters[filter_num].kalman_ind = 0;
  kalman_filters[filter_num].data_ind = 0;
  kalman_filters[filter_num].kalman_length = 0;
  kalman_filters[filter_num].alg_fail_cnt = 0;
  kalman_filters[filter_num].outlier_cnt = 0;
  kalman_filters[filter_num].kalman_avg = 0;
  kalman_filters[filter_num].do_reset_kalman = 0;
  kalman_filters[filter_num].big_avg = 0;
  kalman_filters[filter_num].data_array_length = 0;
}

void init_kalman(kalman_t* kalman_filt) {
	for (int n=0;n<NUM_KALMAN_FILTERS;n++) {
		reset_kalman(n);
		kalman_filt[n].max_alg_fail_cnt = max_alg_fail_cnt_init[n];
		kalman_filt[n].max_outlier_cnt = max_outlier_cnt_init[n];
		kalman_filt[n].max_data_length = max_data_length_init[n];
		kalman_filt[n].max_kalman_length = max_kalman_length_init[n];
		kalman_filt[n].min_data_std = min_data_std_init[n];
		kalman_filt[n].kalman_threshold_2x = kalman_threshold_2x_init[n];
		kalman_filt[n].jumps_ok = jumps_ok_init[n];
	}
//point struct arrays to pre-created arrays
	kalman_filt[0].kalman_array = array0;
	kalman_filt[1].kalman_array = array1;
	kalman_filt[2].kalman_array = array2;
	kalman_filt[3].kalman_array = array3;
	kalman_filt[4].kalman_array = array4;

	kalman_filt[0].data_array = array5;
	kalman_filt[1].data_array = array6;
	kalman_filt[2].data_array = array7;
	kalman_filt[3].data_array = array8;
	kalman_filt[4].data_array = array9;
}

/************************************************************************************
 * Name:       uint_sqrt
 * Function:   32 bit fixed point square root method
 * Parameters: val
 * Return:     square root of val
 ************************************************************************************/
static uint32_t uint_sqrt(uint32_t val)
{
	//integer sqrt function from http://www.azillionmonkeys.com/qed/sqroot.html
	uint32_t temp, g = 0, b = 0x8000, bshft = 15;
	do
	{
		if (val >= (temp = (((g << 1) + b) << bshft--)))
		{
			g += b;
			val -= temp;
		}
	} while (b >>= 1);
	return g;
}

uint16_t get_std_dev(uint16_t *array, uint8_t array_length) {
  uint32_t var =0;
  int32_t less_avg = 0; //data with mean subtracted
  uint16_t avg = get_avg(array,array_length); //get average value of data in array

  //calculate variance
  for (uint8_t n=0; n<array_length; n++) {
    less_avg = (int32_t)array[n]-(int32_t)avg; //cast as int and subtract avg from data
    var += (uint32_t) (less_avg*less_avg);  //add square of deviation from avg
  }
  var /= (uint32_t) array_length; //divide variance by number of samples
    return (uint16_t) uint_sqrt( var ); //cast as uint32 and take square root to get rms (aka standard deviation)
}


uint16_t get_avg (uint16_t *array, uint8_t array_length) {
  int32_t avg = 0;
  for (uint8_t n = 0; n<array_length; n++) {
    avg += array[n];
  }
  return (uint16_t) (avg / (uint32_t)array_length);
}


void run_kalman(uint8_t num, uint32_t new_data, uint8_t ratio) {
	//ratio = 1 means compare normalize std dev to avg
	//ratio = 0 means use std dev as absolute number
	//kalman_filters[num  is written as KF for brevity
  uint16_t up_thresh = 0;
  uint16_t down_thresh = 0;

  uint8_t outlier = 0;
  if ( (KF.kalman_avg == 0) || (KF.do_reset_kalman == 1) ) {
	  reset_kalman(num); //reset the average if the previous average was invalid
      if(new_data != 0) { //if the sample is valid
		KF.data_ind = 0; //reset the data index (don't have to do this but is simplifies the program)
		KF.kalman_ind = 0; //reset the array index
		KF.kalman_array[KF.kalman_ind] = new_data; //put new data in the array
		KF.kalman_length++;
		KF.kalman_avg = new_data;
		KF.kalman_ind++; //increment this for next time
		KF.data_array[KF.data_ind] = new_data; //ouput new data as the new average (lacking other information as yet)
		KF.data_ind++; //increment this for next time
        if (KF.data_array_length < KF.max_data_length) {
        	KF.data_array_length++; //increase array length
        }
        get_big_avg(num);
      } else {
    	  KF.kalman_avg = 0; //average is zero because the first sample is invalid
      }
  } else if (new_data != 0) { //not resetting and valid sample case (typical case)
	KF.data_std = get_std_dev(KF.data_array, KF.data_array_length); //calc the standard deviation of the data using an integer method for sqrt instead of floating point method
	if (ratio) {
		if(KF.data_std < ((int32_t)KF.min_data_std*(int32_t)KF.kalman_avg)/128) {//min_data_std is express in terms of 1/128 fraction of the kalman_avg
			KF.data_std = ((int32_t)KF.min_data_std*(int32_t)KF.kalman_avg)/128;
		}
	} else {//min_data_std is expressed as an absolute number
		if (KF.data_std < KF.min_data_std) {
			KF.data_std = KF.min_data_std; //constrain data variance to a minimum value for stability
		}
	}

    outlier = 0;
    if (new_data > KF.kalman_avg) { //new data is bigger than kalman average
      up_thresh = KF.kalman_threshold_2x*KF.data_std;
      if (KF.jumps_ok == 0) up_thresh = up_thresh>>1; //divide by two for normal case
      if( new_data - KF.kalman_avg > up_thresh) { //jumping high case for SpO2
    	  KF.outlier_cnt++;
    	  outlier = 1;
      }
    } else { //new_data is smaller than kalman_avg
      down_thresh = (KF.kalman_threshold_2x*KF.data_std)>>1;
      if (KF.kalman_avg - new_data > down_thresh) {
    	  KF.outlier_cnt++;
    	  outlier = 1;
      }
    }
    if (outlier == 0) { //not an outlier, update the filter
      if (KF.kalman_length < KF.max_kalman_length) {
    	  KF.kalman_length++;
      }
      KF.kalman_array[KF.kalman_ind] = new_data; //add new data to the kalman array
      KF.kalman_avg = get_avg(KF.kalman_array, KF.kalman_length);
      KF.alg_fail_cnt = 0; //zero the consecutive algorithm fails
      KF.outlier_cnt = 0; //zero the consecutive outlier fails
      KF.kalman_ind++;
      if (KF.kalman_ind >= KF.max_kalman_length) {
    	  KF.kalman_ind = 0;
      }
    }
    if (KF.data_array_length < KF.max_data_length) {
	  KF.data_array_length++;
	}
    //for all new data not equal to zero, store values for calc of std dev.
	KF.data_array[KF.data_ind] = new_data;
	get_big_avg(num);
	KF.data_ind++;
	if (KF.data_ind >= KF.max_data_length) {
	  KF.data_ind = 0;
	}
  } else if (new_data == 0) {
	  KF.alg_fail_cnt++;
    if (KF.alg_fail_cnt == KF.max_alg_fail_cnt){
    	KF.do_reset_kalman = 1;
    }
  }
  if (KF.outlier_cnt >= KF.max_outlier_cnt) {
	  KF.do_reset_kalman = 1;
  }
//  data_std_out = data_std;
}//end of run_kalman


void get_big_avg(uint8_t num){
	/* This is function to throw small data points (noise) out of the average
	 * Runs through the data and compares each data point to the max.
	 * Intended for positive data.
	 * If the data is less than half of the max we don't include it in the big_average.
	 * If the data is greater than or equal to half of the max, we keep it in the average.
	 */
	KF.big_avg = KF.data_array[0];
	uint8_t max_ind = 0;
	uint8_t num_valid = 1;
	uint32_t sum = 0;
	for (int n = 1; n<KF.data_array_length;n++) {//get the maximum
		if ( KF.data_array[n] > KF.big_avg) {
			KF.big_avg = KF.data_array[n];
			max_ind = n;
		}
	}
	sum = (uint32_t)KF.big_avg;
	for (int n = 0;n<KF.data_array_length;n++) {//include greater than half the max
		if (n != max_ind) {
			if ( KF.data_array[n] > (KF.big_avg>>1) ) {
				sum += (uint32_t)KF.data_array[n];
				num_valid++;
			}
		}
	}
	KF.big_avg = sum/num_valid;
}
