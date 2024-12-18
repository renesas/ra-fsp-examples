#ifndef __SAVGOL_H__
#define __SAVGOL_H__

#include "../SPO2/SPO2.h"

//#define CALC_DER //Define this to calculate the derivative and second derivative
#define _SG sg[filt_num]

#ifdef _50sps
	#define SG_INT 4
#else
	#define SG_INT 8
#endif

#define MAX_SG_INT SG_INT

#define NO_RESET 0
#define DO_RESET 1


typedef struct savgol {
	int16_t ind;
	int32_t abcs[2*SG_INT][3];
	uint8_t sg_interval;
	uint8_t prev_sg_interval;
	int32_t num_iter;
	int16_t y_buffer[2*MAX_SG_INT];
	int16_t num_fits;
	int16_t fit;
#ifdef CALC_DER
	int16_t der;
	int16_t sec_der;
#endif
}savgol_t;

extern savgol_t sg[2]; //declare an array of Savitsky Golay filter structs (one for red and one for IR)


enum sg_filters{r_filt, ir_filt};

//functions
void init_savgol(uint8_t filt_num, uint8_t sg_int);
void iterate_savgol(uint8_t filt_num, int16_t y, uint8_t sg_len, uint8_t res ) ;
void quadratic_fit(int16_t* y_vals, int16_t* abc) ;
void quadratic_fit1f(int16_t* y_vals, int32_t* abc1f);

#endif
