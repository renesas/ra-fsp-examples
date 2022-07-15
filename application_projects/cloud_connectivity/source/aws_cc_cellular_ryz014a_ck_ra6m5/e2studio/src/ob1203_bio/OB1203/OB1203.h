#ifndef __OB1203_H__
#define __OB1203_H__

#define LEAN_OB1203 //undefine unused functions


#include "bsp_api.h"

#define I2C_DELAY 10 //added this after switch to CC-RL to stabilize I2C.
#define GAIN_CHANGE_ALLOWED
//#define DEBUG
#define OB1203_ADDR 0xA6
#define true 1
#define false 0

//Define registers
#define REG_STATUS_0        0x00
#define REG_STATUS_1        0x01
#define REG_PS_DATA         0x02
#define REG_LS_W_DATA       0x04
#define REG_LS_G_DATA       0x07
#define REG_LS_B_DATA       0x0A
#define REG_LS_R_DATA       0x0D
#define REG_LS_C_DATA       0x10
#define REG_TEMP_DATA       0x13
#define REG_MAIN_CTRL_0     0x15
#define REG_MAIN_CTRL_1     0x16
#define REG_PS_LED_CURR     0x17
#define REG_PS_CAN_PULSES   0x19
#define REG_PS_PWIDTH_RATE  0x1A
#define REG_PS_CAN_DIG      0x1B
#define REG_PS_MOV_AVG_HYS  0x1D
#define REG_PS_THRES_HI     0x1E
#define REG_PS_THRES_LO     0x20
#define REG_LS_RES_RATE     0x22
#define REG_LS_GAIN         0x23
#define REG_LS_THRES_HI     0x24
#define REG_LS_THRES_LO     0x27
#define REG_LS_THRES_VAR    0x2A
#define REG_INT_CFG_0       0x2B
#define REG_PS_INT_CFG_1    0x2C
#define REG_INT_PST         0x2D
#define REG_PPG_PS_GAIN     0x2E
#define REG_PPG_PS_CFG      0x2F
#define REG_PPG_IRLED_CURR  0x30
#define REG_PPG_RLED_CURR   0x32
#define REG_PPG_CAN_ANA     0x34
#define REG_PPG_AVG         0x35
#define REG_PPG_PWIDTH_RATE 0x36
#define REG_FIFO_CFG        0x37
#define REG_FIFO_WR_PTR     0x38
#define REG_FIFO_RD_PTR     0x39
#define REG_FIFO_OVF_CNT    0x3A
#define REG_FIFO_DATA  0x3B
#define REG_PART_ID         0x3D
#define REG_DEVICE_CFG      0x4D
#define REG_OSC_TRIM        0x3E
#define REG_LED_TRIM        0x3F
#define REG_BIO_TRIM        0x40
#define REG_DIG_LED1_TRIM	0x42
#define REG_DIG_LED2_TRIM	0x43

//Define settings
//STATUS_0
#define POR_STATUS          0x80
#define LS_INT_STATUS       0x02
#define LS_NEW_DATA         0x01
//STATUS_1
#define LED_DRIVER_STATUS   0x40
#define FIFO_AFULL_STATUS   0x20
#define PPG_DATA_STATUS     0x10
#define PS_LOGIC_STATUS     0x04
#define PS_INT_STATUS       0x02
#define PS_NEW_DATA         0x01
//MAIN_CTRL_0
#define SW_RESET            0x01<<7
#define LS_SAI_ON           0x01<<3
#define LS_SAI_OFF          0x00
#define ALS_MODE            0x00
#define RGB_MODE            0x01<<1
#define LS_OFF              0x00
#define LS_ON               0x01
//MAIN_CTRL_1
#define PS_SAI_ON           0x01<<3
#define PS_SAI_OFF          0x00
#define PS_MODE             0x00
#define HR_MODE             0x01<<1
#define SPO2_MODE           0x02<<1
#define PPG_PS_ON           0x01
#define PPG_PS_OFF          0x00
#define TEMP_ON             0x01<<7
#define TEMP_OFF            0x00
//PS_CAN_PULSES
#define PS_CAN_ANA_0        0x00 //off
#define PS_CAN_ANA_1        0x01<<6 //50% of FS
#define PS_CAN_ANA_2        0x02<<6 //100% of FS
#define PS_CAN_ANA_3        0x03<<6 //150% of FS
#define PS_PULSES(x)    (( (x) & (0x07) )<<3) //where x = 0..5 and num pulses = 2^x        
//PS_PWIDTH_RATE
#define PS_PWIDTH(x)   (x & 0x03)<<4 //where x = 0..3
#define PS_RATE(x)          (x & 0x07) //where x = 0..7      
//PS_MOV_AVG_HYS
#define PS_AVG_ON           1<<7
#define PS_AVG_OFF          0
#define PS_HYS_LEVEL(x)     x>>1 //where x=0..256
//LS_RES_RATE
#define LS_RES(x)           (x & 0x07)<<4 //where x=0..7
#define LS_RATE(x)          (x & 0x07) //where x=0..7        

#define LS_RES_20          0x00
#define LS_RES_19          0x01
#define LS_RES_18          0x02
#define LS_RES_17          0x03
#define LS_RES_16         0x04
#define LS_RES_13          0x05

//LS_GAIN
#define LS_GAIN(x)          (x & 0x03)
#define LS_GAIN_1           0x00
#define LS_GAIN_3           0x01
#define LS_GAIN_6           0x10
#define LS_GAIN_20          0x11

//LS_THRES_VAR
#define LS_THRES_VAR(x)     (x & 0x07)
//INT_CFG_0
#define LS_INT_SEL_W        0
#define LS_INT_SEL_G        1<<4
#define LS_INT_SEL_R        2<<4
#define LS_INT_SEL_B        3<<4
#define LS_THRES_INT_MODE  0
#define LS_VAR_INT_MODE     1<<1
#define LS_INT_ON           1
#define LS_INT_OFF          0
//INT_CFG_1
#define AFULL_INT_ON        1<<5
#define AFULL_INT_OFF       0
#define PPG_INT_ON          1<<4
#define PPG_INT_OFF          0
#define PS_INT_READ_CLEARS  0<<1
#define PS_INT_LOGIC 1
#define PS_INT_ON           1
#define PS_INT_OFF          0

//INT_PST
#define LS_PERSIST(x)       (x & 0x0F)<<4
#define PS_PERSIST(x)       (x & 0x0F)
//PPG_PS_GAIN
#define PPG_PS_GAIN_1       0
#define PPG_PS_GAIN_1P5     1
#define PPG_PS_GAIN_2       2
#define PPG_PS_GAIN_4       3
#define PPG_LED_SETTLING(x) (x & 0x03)<<2 // 0=0us, 1 = 5us, 2 = 10us (Default), 3=20us
#define PPG_ALC_TRACK(x)    (x & 0x03) // 0 = 10us, 1 (20us) DEFAULT ,2 = 30us, 3 = 60us
//PPG_PS_CFG
#define PPG_POW_SAVE_ON     1<<6
#define PPG_POW_SAVE_OFF    0
#define LED_FLIP_ON         1<<3
#define LED_FLIP_OFF        0
#define DIFF_OFF            2
#define ALC_OFF             1
#define DIFF_ON             0
#define ALC_ON              0
#define SIGNAL_OUT          1<<2
#define OFFSET_OUT          0
//PPG_CAN_ANA
#define PPG_CH1_CAN(x)      (x & 0x03)<<2
#define PPG_CH2_CAN(x)      (x & 0x03)
//PPG_AVG
#define PPG_AVG(x)          (x & 0x07)<<4
//PPG_PWIDTH_RATE
#define PPG_PWIDTH(x)       (x & 0x07)<<4
#define PPG_FREQ_PRODUCTION       0
#define PPG_FREQ_PREPRODUCTION       1<<3
#define PPG_PERIOD(x)         (x & 0x07)
//FIFO_CFG
#define FIFO_ROLL_ON        1<<4
#define FIFO_ROLL_OFF       0
#define AFULL_ADVANCE_WARNING(x)   (x & 0x0F)
#define MAX_PPG_GAIN 3
#define POR_TIME_MS 10 

#define PS_THRESH_HI 0x0600 //x3A98for whaleteq testing 7.5k counts was 004C
#define TARGET_COUNTS 0x30000
//#define BIO_THRESHOLD 100000 //about 1/2 of TARGET_COUNTS and a value below which PS_THRESH_HI is reached in prox mode

#define PS_THRESH_TEST 0x00c8

//#define BIO_THRESHOLD 2000 //about 1/10 of TARGET_COUNTS to keep in BIO mode during whaleteq adjustments.

//#define MAX_LOW_SAMPLES 500//30//50 //one second // was 30
//#define TOL1 6000//6000		// defines where the LED current is adjusted to: TARGET_COUNTS +- TOL1
//#define TOL1 1000
#define TOL1 6000
#define TOL2 0x7530 //30k counts ( < +/-2^15) -- so we can represent values as 18 bit data minus setpoint in int16_t
//#define TOL2 0x1000 //
//#define STEP 8
#define STEP 8
#define IN_RANGE_PERSIST 4
#define IR_MAX_CURRENT 0x03FF//
#define R_MAX_CURRENT 0x01FF  //0x01FF
#define MAX_TARGET (0x38000UL) //220k
#define MIN_TARGET (0xC000UL) //48k
#define IR_START_CURRENT (870) //85% of 1024 full scale current
#define R_START_CURRENT (435) // 85% of 512 full scale current
extern uint8_t i2c_addr;

typedef struct ob1203{
	#ifndef LEAN_OB1203
	uint8_t osc_trim;
	uint8_t ls_res;
	uint8_t ls_rate;
	uint8_t ls_gain;
	uint32_t ls_thres_hi;
	uint32_t ls_thres_lo;
	uint8_t ls_sai;
	uint8_t ls_mode;
	uint8_t ls_en;
	uint8_t ls_int_sel;
	uint8_t ls_var_mode;
	uint8_t ps_sai_en;
	uint8_t temp_en;
	uint8_t ls_persist;
	uint8_t ps_persist;
	uint8_t ppg_pow_save;
	uint8_t led_flip;
	uint8_t ch1_can_ana;
	uint8_t ch2_can_ana;
	uint8_t ppg_LED_settling;
	uint8_t ppg_ALC_track;
	uint8_t ps_pulses;
	uint8_t ps_pwidth;
	uint8_t ps_rate;
	uint8_t ps_logic_mode;
	uint16_t ps_digital_can;

	uint8_t ps_avg_en;
	uint8_t ps_hys_level;
	uint8_t ls_int_en;
	uint8_t fifo_rollover_en;
	uint8_t fifo_afull_advance_warning;
	uint8_t bio_trim;
	uint8_t led_trim;
	uint8_t diff;
	uint8_t alc;
	uint8_t sig_out;
	uint8_t ppg_freq;
	#endif
	uint16_t ps_thres_hi;
	uint16_t ps_thres_lo;
	uint8_t ppg_avg;
	uint8_t ppg_pwidth;
	uint8_t ppg_period;
	uint8_t ppg_ps_mode;
	uint8_t ppg_ps_en;
	uint8_t ps_can_ana;
	uint8_t afull_int_en;
	uint8_t ppg_int_en;
	uint8_t ps_int_en;
	uint16_t ps_current;
	uint16_t ir_current;
	uint16_t r_current;
	uint8_t ppg_ps_gain;
	uint8_t writePointer;
	uint8_t readPointer;
	uint8_t fifoOverflow;
	uint8_t led1_trim;
	uint8_t led2_trim;
	uint8_t led1_orig_trim;
	uint8_t led2_orig_trim;
	uint8_t target_change;
	uint8_t ppg_gain_change;
	uint32_t target_counts[2];
	volatile uint8_t ir_in_range;
	volatile uint8_t r_in_range;
	volatile uint8_t prev_in_range;
	volatile uint8_t update_fifo_mode;
	volatile uint8_t update_current;
	uint16_t in_range[2];
}ob1203_t;

extern struct ob1203 ob;

void OB1203_init(uint8_t add);
//OB1203 (uint8_t addr = OB1203_ADDR);

// Low-level operations
void reset(void);
uint16_t get_status(void);
void writeRegister(int, uint8_t, uint8_t);
void writeBlock(int, uint8_t, uint8_t *, uint8_t);
void readBlock(int, uint8_t, uint8_t *, int);
void readRegisters(int, uint8_t, uint8_t *, int);
uint32_t bytes2uint32(uint8_t *, int);
uint32_t twoandhalfBytes2uint32(uint8_t *, int);

// High-level operations
#ifndef LEAN_OB1203
void setOscTrim(struct ob1203*);
uint8_t dataIsNew(void);
uint8_t lsIsNew(void);
uint8_t psIsNew(void);
uint8_t tempIsNew(void);
uint8_t bioIsNew(void);
void setPPGana_can(struct ob1203*);
void setDigitalCan(struct ob1203*);
uint8_t get_ps_data(uint32_t *);
uint8_t get_ls_data(uint32_t *);
uint8_t get_ps_ls_data(uint32_t *);
void init_rgb(struct ob1203*);
void init_ps_rgb(struct ob1203*);
uint8_t get_part_ID(uint8_t *);
#endif
void setMainConfig(struct ob1203*);
void setIntConfig(struct ob1203*);
void setLSthresh(struct ob1203*);
void setPSthresh(struct ob1203*);
void setPScurrent(struct ob1203*);
void setPPGcurrent(struct ob1203*);
void setPPG_PSgain_cfg(struct ob1203*);
void setPPGavg_and_rate(struct ob1203*);
void setFifoConfig(struct ob1203*);
//  void setBioTrim(struct ob1203*);
//  void setLEDTrim(struct ob1203*);
void setDigTrim(struct ob1203*);
void resetFIFO(struct ob1203*);
void init_ps(struct ob1203*);
void init_hr(struct ob1203*);
void init_spo2(struct ob1203*);
void getFifoInfo(struct ob1203*,uint8_t *fifo_info);
void getNumFifoSamplesAvailable(struct ob1203 *_ob,uint8_t *fifo_info, uint8_t *sample_info);
void getFifoSamples(uint8_t, uint8_t *);
void parseFifoSamples(uint8_t, uint8_t *, uint32_t *);
void ob1203_struct_init(struct ob1203 *);
//agc functions
void do_agc(struct ob1203*,uint32_t,uint8_t, uint8_t ppg2_mode);

#endif
