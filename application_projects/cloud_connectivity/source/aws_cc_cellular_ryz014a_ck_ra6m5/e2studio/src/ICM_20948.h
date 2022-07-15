/*
 * ICM_20948.h
 *
 *  Created on: Jan 20, 2022
 *      Author: Vivek
 */

#ifndef ICM_20948_H_
#define ICM_20948_H_

#include <stdbool.h>
#include "hal_data.h"
#include "user_choice.h"

/* Typedefs */
typedef enum
{
    ub_0 = 0 << 4,
    ub_1 = 1 << 4,
    ub_2 = 2 << 4,
    ub_3 = 3 << 4
} userbank;

#define Bank_0                       (uint8_t) 0
#define Bank_1                       (uint8_t) 1
#define Bank_2                       (uint8_t) 2
#define Bank_3                       (uint8_t) 3

#define AK09916_ADDRESS 0x0C

/* Registers ICM20948 USER BANK 0*/
#define ICM20948_WHO_AM_I            (uint8_t) 0x00
#define ICM20948_USER_CTRL           (uint8_t) 0x03
#define ICM20948_LP_CONFIG           (uint8_t) 0x05
#define ICM20948_PWR_MGMT_1          (uint8_t) 0x06
#define ICM20948_PWR_MGMT_2          (uint8_t) 0x07
#define ICM20948_INT_PIN_CFG         (uint8_t) 0x0F
#define ICM20948_INT_ENABLE          (uint8_t) 0x10
#define ICM20948_INT_ENABLE_1        (uint8_t) 0x11
#define ICM20948_INT_ENABLE_2        (uint8_t) 0x12
#define ICM20948_INT_ENABLE_3        (uint8_t) 0x13
#define ICM20948_I2C_MST_STATUS      (uint8_t) 0x17
#define ICM20948_INT_STATUS          (uint8_t) 0x19
#define ICM20948_INT_STATUS_1        (uint8_t) 0x1A
#define ICM20948_INT_STATUS_2        (uint8_t) 0x1B
#define ICM20948_INT_STATUS_3        (uint8_t) 0x1C
#define ICM20948_DELAY_TIME_H        (uint8_t) 0x28
#define ICM20948_DELAY_TIME_L        (uint8_t) 0x29
#define ICM20948_ACCEL_OUT           (uint8_t) 0x2D // accel data registers begin
#define ICM20948_GYRO_OUT            (uint8_t) 0x33 // gyro data registers begin
#define ICM20948_TEMP_OUT            (uint8_t) 0x39
#define ICM20948_EXT_SLV_SENS_DATA_00  (uint8_t) 0x3B
#define ICM20948_EXT_SLV_SENS_DATA_01  (uint8_t) 0x3C
#define ICM20948_FIFO_EN_1           (uint8_t) 0x66
#define ICM20948_FIFO_EN_2           (uint8_t) 0x67
#define ICM20948_FIFO_RST            (uint8_t) 0x68
#define ICM20948_FIFO_MODE           (uint8_t) 0x69
#define ICM20948_FIFO_COUNT          (uint8_t) 0x70
#define ICM20948_FIFO_R_W            (uint8_t) 0x72
#define ICM20948_DATA_RDY_STATUS     (uint8_t) 0x74
#define ICM20948_FIFO_CFG            (uint8_t) 0x76

/* Registers ICM20948 USER BANK 1*/
#define ICM20948_SELF_TEST_X_GYRO    (uint8_t) 0x02
#define ICM20948_SELF_TEST_Y_GYRO    (uint8_t) 0x03
#define ICM20948_SELF_TEST_Z_GYRO    (uint8_t) 0x04
#define ICM20948_SELF_TEST_X_ACCEL   (uint8_t) 0x0E
#define ICM20948_SELF_TEST_Y_ACCEL   (uint8_t) 0x0F
#define ICM20948_SELF_TEST_Z_ACCEL   (uint8_t) 0x10
#define ICM20948_XA_OFFS_H           (uint8_t) 0x14
#define ICM20948_XA_OFFS_L           (uint8_t) 0x15
#define ICM20948_YA_OFFS_H           (uint8_t) 0x17
#define ICM20948_YA_OFFS_L           (uint8_t) 0x18
#define ICM20948_ZA_OFFS_H           (uint8_t) 0x1A
#define ICM20948_ZA_OFFS_L           (uint8_t) 0x1B
#define ICM20948_TIMEBASE_CORR_PLL   (uint8_t) 0x28

/* Registers ICM20948 USER BANK 2*/
#define ICM20948_GYRO_SMPLRT_DIV     (uint8_t) 0x00
#define ICM20948_GYRO_CONFIG_1       (uint8_t) 0x01
#define ICM20948_GYRO_CONFIG_2       (uint8_t) 0x02
#define ICM20948_XG_OFFS_USRH        (uint8_t) 0x03
#define ICM20948_XG_OFFS_USRL        (uint8_t) 0x04
#define ICM20948_YG_OFFS_USRH        (uint8_t) 0x05
#define ICM20948_YG_OFFS_USRL        (uint8_t) 0x06
#define ICM20948_ZG_OFFS_USRH        (uint8_t) 0x07
#define ICM20948_ZG_OFFS_USRL        (uint8_t) 0x08
#define ICM20948_ODR_ALIGN_EN        (uint8_t) 0x09
#define ICM20948_ACCEL_SMPLRT_DIV_1  (uint8_t) 0x10
#define ICM20948_ACCEL_SMPLRT_DIV_2  (uint8_t) 0x11
#define ICM20948_ACCEL_INTEL_CTRL    (uint8_t) 0x12
#define ICM20948_ACCEL_WOM_THR       (uint8_t) 0x13
#define ICM20948_ACCEL_CONFIG        (uint8_t) 0x14
#define ICM20948_ACCEL_CONFIG_2      (uint8_t) 0x15
#define ICM20948_FSYNC_CONFIG        (uint8_t) 0x52
#define ICM20948_TEMP_CONFIG         (uint8_t) 0x53
#define ICM20948_MOD_CTRL_USR        (uint8_t) 0x54

/* Registers ICM20948 USER BANK 3*/
#define ICM20948_I2C_MST_ODR_CFG     (uint8_t) 0x00
#define ICM20948_I2C_MST_CTRL        (uint8_t) 0x01
#define ICM20948_I2C_MST_DELAY_CTRL  (uint8_t) 0x02
#define ICM20948_I2C_SLV0_ADDR       (uint8_t) 0x03
#define ICM20948_I2C_SLV0_REG        (uint8_t) 0x04
#define ICM20948_I2C_SLV0_CTRL       (uint8_t) 0x05
#define ICM20948_I2C_SLV0_DO         (uint8_t) 0x06

/* Registers ICM20948 ALL BANKS */
#define ICM20948_REG_BANK_SEL        (uint8_t) 0x7F

/* Registers AK09916 */
#define AK09916_WIA_1    (uint8_t) 0x00 // Who I am, Company ID
#define AK09916_WIA_2    (uint8_t) 0x01 // Who I am, Device ID
#define AK09916_STATUS_1 (uint8_t) 0x10
#define AK09916_HXL      (uint8_t) 0x11
#define AK09916_HXH      (uint8_t) 0x12
#define AK09916_HYL      (uint8_t) 0x13
#define AK09916_HYH      (uint8_t) 0x14
#define AK09916_HZL      (uint8_t) 0x15
#define AK09916_HZH      (uint8_t) 0x16
#define AK09916_STATUS_2 (uint8_t) 0x18
#define AK09916_CNTL_2   (uint8_t) 0x31
#define AK09916_CNTL_3   (uint8_t) 0x32

/* Register Bits */
#define ICM20948_RESET              (uint8_t) 0x80
#define ICM20948_I2C_MST_EN         (uint8_t) 0x20
#define ICM20948_SLEEP              (uint8_t) 0x40
#define ICM20948_LP_EN              (uint8_t) 0x20
#define ICM20948_BYPASS_EN          (uint8_t) 0x02
#define ICM20948_GYR_EN             (uint8_t) 0x07
#define ICM20948_ACC_EN             (uint8_t) 0x38
#define ICM20948_FIFO_EN            (uint8_t) 0x40
#define ICM20948_INT1_ACTL          (uint8_t) 0x80
#define ICM20948_INT_1_LATCH_EN     (uint8_t) 0x20
#define ICM20948_ACTL_FSYNC         (uint8_t) 0x08
#define ICM20948_INT_ANYRD_2CLEAR   (uint8_t) 0x10
#define ICM20948_FSYNC_INT_MODE_EN  (uint8_t) 0x06
#define AK09916_16_BIT              (uint8_t) 0x10
#define AK09916_OVF                 (uint8_t) 0x08
#define AK09916_READ                (uint8_t) 0x80

/* Others */
#define AK09916_WHO_AM_I            0x4809
#define ICM20948_ROOM_TEMP_OFFSET   0.0f
#define ICM20948_T_SENSITIVITY      333.87f
#define AK09916_MAG_LSB             0.1495f

/* Enums */
typedef enum ICM20948_CYCLE {
    ICM20948_NO_CYCLE              = 0x00,
    ICM20948_GYR_CYCLE             = 0x10,
    ICM20948_ACC_CYCLE             = 0x20,
    ICM20948_ACC_GYR_CYCLE         = 0x30,
    ICM20948_ACC_GYR_I2C_MST_CYCLE = 0x70
} ICM20948_cycle;

typedef enum ICM20948_INT_PIN_POL {
    ICM20948_ACT_HIGH, ICM20948_ACT_LOW
} ICM20948_intPinPol;

typedef enum ICM20948_INT_TYPE {
    ICM20948_FSYNC_INT      = 0x01,
    ICM20948_WOM_INT        = 0x02,
    ICM20948_DMP_INT        = 0x04,
    ICM20948_DATA_READY_INT = 0x08,
    ICM20948_FIFO_OVF_INT   = 0x10,
    ICM20948_FIFO_WM_INT    = 0x20
} ICM20948_intType;

typedef enum ICM20948_FIFO_TYPE {
    ICM20948_FIFO_ACC        = 0x10,
    ICM20948_FIFO_GYR        = 0x0E,
    ICM20948_FIFO_ACC_GYR    = 0x1E
} ICM20948_fifoType;

typedef enum ICM20948_FIFO_MODE_CHOICE {
    ICM20948_CONTINUOUS, ICM20948_STOP_WHEN_FULL
} ICM20948_fifoMode;

typedef enum ICM20948_GYRO_RANGE {
    ICM20948_GYRO_RANGE_250, ICM20948_GYRO_RANGE_500, ICM20948_GYRO_RANGE_1000, ICM20948_GYRO_RANGE_2000
} ICM20948_gyroRange;

typedef enum ICM20948_DLPF {
    ICM20948_DLPF_0, ICM20948_DLPF_1, ICM20948_DLPF_2, ICM20948_DLPF_3, ICM20948_DLPF_4, ICM20948_DLPF_5,
    ICM20948_DLPF_6, ICM20948_DLPF_7, ICM20948_DLPF_OFF
} ICM20948_dlpf;

typedef enum ICM20948_GYRO_AVG_LOW_PWR {
    ICM20948_GYR_AVG_1, ICM20948_GYR_AVG_2, ICM20948_GYR_AVG_4, ICM20948_GYR_AVG_8, ICM20948_GYR_AVG_16,
    ICM20948_GYR_AVG_32, ICM20948_GYR_AVG_64, ICM20948_GYR_AVG_128
} ICM20948_gyroAvgLowPower;

typedef enum ICM20948_ACC_RANGE {
    ICM20948_ACC_RANGE_2G, ICM20948_ACC_RANGE_4G, ICM20948_ACC_RANGE_8G, ICM20948_ACC_RANGE_16G
} ICM20948_accRange;

typedef enum ICM20948_ACC_AVG_LOW_PWR {
    ICM20948_ACC_AVG_4, ICM20948_ACC_AVG_8, ICM20948_ACC_AVG_16, ICM20948_ACC_AVG_32
} ICM20948_accAvgLowPower;

typedef enum ICM20948_WOM_COMP {
    ICM20948_WOM_COMP_DISABLE, ICM20948_WOM_COMP_ENABLE
} ICM20948_womCompEn;

typedef enum AK09916_OP_MODE {
    AK09916_PWR_DOWN           = 0x00,
    AK09916_TRIGGER_MODE       = 0x01,
    AK09916_CONT_MODE_10HZ     = 0x02,
    AK09916_CONT_MODE_20HZ     = 0x04,
    AK09916_CONT_MODE_50HZ     = 0x06,
    AK09916_CONT_MODE_100HZ    = 0x08
} AK09916_opMode;

typedef enum ICM20948_ORIENTATION {
    ICM20948_FLAT, ICM20948_FLAT_1, ICM20948_XY, ICM20948_XY_1, ICM20948_YX, ICM20948_YX_1
} ICM20948_orientation;

typedef struct  {
    double x;
    double y;
    double z;
}xyzFloat;

/* Basic settings */
bool icm20948_init();
void autoOffsets();
void setGyrOffsets(float xOffset, float yOffset, float zOffset);
uint8_t whoAmI();
void setAccRange(ICM20948_accRange accRange);
void setAccDLPF(ICM20948_dlpf dlpf);
void setAccSampleRateDivider(uint16_t accSplRateDiv);
void setGyrRange(ICM20948_gyroRange gyroRange);
void setGyrDLPF(ICM20948_dlpf dlpf);
void setGyrSampleRateDivider(uint8_t gyrSplRateDiv);
void setTempDLPF(ICM20948_dlpf dlpf);
void setI2CMstSampleRate(uint8_t rateExp);
void ICM_20948_get();

/* x,y,z results */
void readSensor();
void getAccRawValues(xyzFloat*a);
void getCorrectedAccRawValues(xyzFloat*a);
void getGValues(xyzFloat*a);
void getAccRawValuesFromFifo(xyzFloat*a);
void getCorrectedAccRawValuesFromFifo(xyzFloat*a);
double getResultantG(xyzFloat gVal);
float getTemperature();
void getGyrRawValues(xyzFloat*a);
void getCorrectedGyrRawValues(xyzFloat*a);
void getGyrValues(xyzFloat*a);
xyzFloat getGyrValuesFromFifo();
void getMagValues(xyzFloat*a);

/* Angles and Orientation */
void getAngles(xyzFloat*a);
ICM20948_orientation getOrientation();
float getPitch();
float getRoll();

/* Power, Sleep, Standby */
void enableCycle(ICM20948_cycle cycle);
void enableLowPower(bool enLP);
void setGyrAverageInCycleMode(ICM20948_gyroAvgLowPower avg);
void setAccAverageInCycleMode(ICM20948_accAvgLowPower avg);
void sleep(bool sleep);

/* Interrupts */
void setIntPinPolarity(ICM20948_intPinPol pol);
void enableIntLatch(bool latch);
void enableClearIntByAnyRead(bool clearByAnyRead);
void setFSyncIntPolarity(ICM20948_intPinPol pol);
void enableInterrupt(ICM20948_intType intType);
void disableInterrupt(ICM20948_intType intType);
uint8_t readAndClearInterrupts();
bool checkInterrupt(uint8_t source, ICM20948_intType type);
void setWakeOnMotionThreshold(uint8_t womThresh, ICM20948_womCompEn womCompEn);


/* FIFO */
void enableFifo(bool fifo);
void setFifoMode(ICM20948_fifoMode mode);
void startFifo(ICM20948_fifoType fifo);
void stopFifo();
void resetFifo();
int16_t getFifoCount();
int16_t getNumberOfFifoDataSets();
void findFifoBegin();


/* Magnetometer */
bool initMagnetometer();
int16_t whoAmIMag();
void setMagOpMode(AK09916_opMode opMode);
void resetMag();

void setClockToAutoSelect();
void correctAccRawValues(xyzFloat *a);
void correctGyrRawValues(xyzFloat *a);
void switchBank(uint8_t newBank);
fsp_err_t writeRegister8(uint8_t bank, uint8_t reg, uint8_t val);
fsp_err_t writeRegister16(uint8_t bank, uint8_t reg, uint16_t val);
uint8_t readRegister8(uint8_t bank, uint8_t reg);
int16_t readRegister16(uint8_t bank, uint8_t reg);
void readAllData(uint8_t* data);
void readICM20948xyzValFromFifo(xyzFloat*a);
void writeAK09916Register8(uint8_t reg, uint8_t val);
uint8_t readAK09916Register8(uint8_t reg);
int16_t readAK09916Register16(uint8_t reg);
uint8_t reset_ICM20948();
void enableI2CMaster();
void enableMagDataRead(uint8_t reg, uint8_t bytes);

#endif /* ICM_20948_H_ */
