#include "SAVGOL.h"

savgol_t sg[2]; //declare an array of two structs, one for Red and one for IR.

void init_savgol(uint8_t filt_num, uint8_t sg_int) {
	if (filt_num<2) {
		sg[filt_num].num_iter = -1;
		sg[filt_num].num_fits = -1;
		sg[filt_num].sg_interval = sg_int;
		sg[filt_num].prev_sg_interval = sg_int;
		sg[filt_num].ind = -1;
	}
}

void iterate_savgol(uint8_t filt_num, int16_t _y, uint8_t sg_len, uint8_t res ) {
	/*Get (2*interval) quadratic fits through three points spaced by interval,
	Evaluate the fits at the data pointstart
	Interval must be 2^n
	Requires at least 4*interval points (2*interval in front and back) to get
	one point. So 4*interval+n points can return n+1 fits (starting at the
	2*interval'th value)
	For readability indexed array of structs sg[filt_num] is defined as SG with #define in SAVGOL.h
	*/
	int16_t pts[3];
	uint8_t ind1;
	uint8_t ind2;
	int32_t abc[3];
	int16_t xpt;
	int32_t fit1f;
#ifdef CALC_DER
	int32_t der1f;
	int32_t sec_der1f;
#endif
	int16_t loc;

	static uint8_t sg_int2x_bits;
	static uint8_t sg_int_bits;
	//reset
	if( (sg_len != _SG.prev_sg_interval) || (res == 1) ){
		//reset the filter
		init_savgol(filt_num,sg_len);
	}
	int16_t mask = (_SG.sg_interval<<1)-1;
	//setup


	if (_SG.num_iter == -1) {
		sg_int2x_bits = 1;
		while (_SG.sg_interval>>sg_int2x_bits) {
			sg_int2x_bits++;
		}
		sg_int_bits = sg_int2x_bits-1; //x2
	}


	_SG.num_iter++;
	_SG.ind++;
	_SG.ind &= mask; //loop index back

	_SG.fit = 0;
	fit1f = 0;
#ifdef CALC_DER
	_SG.der = 0;
	_SG.sec_der = 0;
	der1f = 0;
	sec_der1f = 0;
#endif


	if (_SG.num_iter <= mask) {
		_SG.fit = (_SG.num_iter == 0) ? _y : _SG.y_buffer[0]; //spit out the oldest sample while we load the buffer
	} else {//we have at least 2*sg_interval+1 samples
		ind1 = _SG.ind; //oldest sample
		ind2 = _SG.ind + _SG.sg_interval;
		ind2 &= mask; //loop index back
		pts[0] = _SG.y_buffer[ind1]; //-1 interval
		pts[1] = _SG.y_buffer[ind2]; //midpoint
		pts[2] = _y; //+1 interval
		quadratic_fit1f(pts,abc); //these coefficients have 1<<FIXED_BITS additional precision.
		for (int n=0;n<3;n++) {
			_SG.abcs[_SG.ind][n] = abc[n]; //save these coefficients
		}
		if (_SG.num_fits < mask) {//increment the number of fits
			_SG.num_fits++;
		}
		if (_SG.num_iter+1 < _SG.sg_interval<<2) {
			_SG.fit = _SG.y_buffer[_SG.ind]; //spit out saved samples until we have enough fits
		} else {//we have enough fits, do the poor man's Savitsky Golay filter
			for (int n=0;n<=_SG.num_fits;n++) {
				xpt = (_SG.sg_interval-1) - n; //e.g. for interval 4, run from -4 to +3
				loc = _SG.ind;
				loc += 1;
				loc += n; //oldest fit
				loc &= mask; //loop back
				//evaluate quadratic fits and add to running sum
				fit1f += ((_SG.abcs[loc][0] * ( (int32_t) xpt * (int32_t) xpt)) >> sg_int_bits) >> sg_int_bits ; //a * (x/interval)^2 check that we have enough fixed point resolution! E.g. if xpt = 1 and sg_interval is 16, then we are right shifting by 8 (divide by 256)
				fit1f += (_SG.abcs[loc][1] * xpt) >> sg_int_bits; // + b*(x/interval)
				fit1f += _SG.abcs[loc][2]; // + c
#ifdef CALC_DER
				der1f += (( _SG.abcs[loc][0]<<1) * (int32_t) xpt) >> sg_int_bits; //2a*(x/interval);
				der1f += _SG.abcs[loc][1]; //+b
				sec_der1f += _SG.abcs[loc][0]<<1; //2*a;
#endif
			}
			fit1f = fit1f>>sg_int2x_bits; //divide by 2*sg_interval (the max number of fits)
			_SG.fit = fit1f>>FIXED_BITS;
			#ifdef CALC_DER
			der1f = der1f>>sg_int2x_bits; //should divide by one more factor of interval but that just reduces resolution
			sec_der1f = sec_der1f>>sg_int2x_bits; //should divide by two more factors of interval but that just reduces resolution
			//remove fixed precision needed for running sum

			_SG.der = der1f>>FIXED_BITS;
			_SG.sec_der = sec_der1f>>FIXED_BITS;
			#endif
		}
	}
	_SG.y_buffer[_SG.ind] = _y; //replace oldest sample with latest data
}

void quadratic_fit(int16_t* y_vals, int16_t* abc) {
 /* based on verified simple_peak_find
 * a is quadratic term fit, b is linear term, c is constant = y[2]*/
	abc[2] = y_vals[1];
	abc[0] = (y_vals[0]+y_vals[2]-(abc[2]<<1) )>>1;
	abc[1] = (y_vals[2] - y_vals[0])>>1;
}

void quadratic_fit1f(int16_t* y_vals, int32_t* abc1f) {
 /* based on verified simple_peak_find
 * a is quadratic term fit, b is linear term, c is constant = y[2]*/
	abc1f[2] = ( (int32_t)y_vals[1])<<FIXED_BITS;
	abc1f[0] = ( ( ( (int32_t)y_vals[0])<<FIXED_BITS) + ( ( (int32_t)y_vals[2])<<FIXED_BITS)- ( abc1f[2]<<1) )>>1;
	abc1f[1] = ( ( ( (int32_t)y_vals[2])<<FIXED_BITS) - ( ( (int32_t)y_vals[0])<<FIXED_BITS) )>>1;
}
