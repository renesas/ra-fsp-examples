/*
 * ICP_20100.h
 *
 *  Created on: Jan 20, 2022
 *      Author: Vivek
 */

#ifndef ICP_20100_H_
#define ICP_20100_H_

#include <stdbool.h>
#include "hal_data.h"

//#define INT_PIN    GPIO_PORT_0_PIN_3
#define ERROR      -1

/**
 * @brief Pressure 20 register map.
 * @details Specified register map of Pressure 20 Click driver.
 */
#define PRESSURE20_REG_TRIM1_MSB                0x05
#define PRESSURE20_REG_TRIM2_LSB                0x06
#define PRESSURE20_REG_TRIM2_MSB                0x07
#define PRESSURE20_REG_DEVICE_ID                0x0C
#define PRESSURE20_REG_IO_DRIVE_STRENGTH        0x0D
#define PRESSURE20_REG_OTP_CONFIG1              0xAC
#define PRESSURE20_REG_OTP_MR_LSB               0xAD
#define PRESSURE20_REG_OTP_MR_MSB               0xAE
#define PRESSURE20_REG_OTP_MRA_LSB              0xAF
#define PRESSURE20_REG_OTP_MRA_MSB              0xB0
#define PRESSURE20_REG_OTP_MRB_LSB              0xB1
#define PRESSURE20_REG_OTP_MRB_MSB              0xB2
#define PRESSURE20_REG_OTP_ADDRESS_REG          0xB5
#define PRESSURE20_REG_OTP_COMMAND_REG          0xB6
#define PRESSURE20_REG_OTP_RDATA                0xB8
#define PRESSURE20_REG_OTP_STATUS               0xB9
#define PRESSURE20_REG_OTP_DBG2                 0xBC
#define PRESSURE20_REG_MASTER_LOCK              0xBE
#define PRESSURE20_REG_OTP_STATUS2              0xBF
#define PRESSURE20_REG_MODE_SELECT              0xC0
#define PRESSURE20_REG_INTERRUPT_STATUS         0xC1
#define PRESSURE20_REG_INTERRUPT_MASK           0xC2
#define PRESSURE20_REG_FIFO_CONFIG              0xC3
#define PRESSURE20_REG_FIFO_FILL                0xC4
#define PRESSURE20_REG_SPI_MODE                 0xC5
#define PRESSURE20_REG_PRESS_ABS_LSB            0xC7
#define PRESSURE20_REG_PRESS_ABS_MSB            0xC8
#define PRESSURE20_REG_PRESS_DELTA_LSB          0xC9
#define PRESSURE20_REG_PRESS_DELTA_MSB          0xCA
#define PRESSURE20_REG_DEVICE_STATUS            0xCD
#define PRESSURE20_REG_I3C_INFO                 0xCE
#define PRESSURE20_REG_VERSION                  0xD3
#define PRESSURE20_REG_DUMMY                    0xEE
#define PRESSURE20_REG_PRESS_DATA_0             0xFA
#define PRESSURE20_REG_PRESS_DATA_1             0xFB
#define PRESSURE20_REG_PRESS_DATA_2             0xFC
#define PRESSURE20_REG_TEMP_DATA_0              0xFD
#define PRESSURE20_REG_TEMP_DATA_1              0xFE
#define PRESSURE20_REG_TEMP_DATA_2              0xFF


/**
 * @brief Pressure 20 TRIM settings.
 * @details Specified TRIM settings of Pressure 20 Click driver.
 */
#define PRESSURE20_TRIM1_MSB_PEFE_OFFSET_MASK   0x3F
#define PRESSURE20_TRIM2_LSB_HFOSC_MASK         0x7F
#define PRESSURE20_TRIM2_MSB_PEFE_GAIN_MASK     0x70
#define PRESSURE20_TRIM2_MSB_BG_PTAT_MASK       0x0F

/**
 * @brief Pressure 20 OTP address and command settings.
 * @details Specified OTP address and command settings of Pressure 20 Click driver.
 */
#define PRESSURE20_OTP_ADDRESS_OFFSET           0xF8
#define PRESSURE20_OTP_ADDRESS_GAIN             0xF9
#define PRESSURE20_OTP_ADDRESS_HFOSC            0xFA
#define PRESSURE20_OTP_COMMAND_READ_ACTION      0x10


/**
 * @brief Pressure 20 OTP settings.
 * @details Specified OTP settings of Pressure 20 Click driver.
 */
#define PRESSURE20_OTP_MRA_LSB                  0x04
#define PRESSURE20_OTP_MRA_MSB                  0x04
#define PRESSURE20_OTP_MRB_LSB                  0x21
#define PRESSURE20_OTP_MRB_MSB                  0x20
#define PRESSURE20_OTP_MR_LSB                   0x10
#define PRESSURE20_OTP_MR_MSB                   0x80

/**
 * @brief Pressure 20 OTP_CONFIG1 register settings.
 * @details Specified OTP_CONFIG1 register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_OTP_WRITE_SWITCH             0x02
#define PRESSURE20_OTP_ENABLE                   0x01
#define PRESSURE20_OTP_DISABLE                  0x00

/**
 * @brief Pressure 20 OTP_DBG2 register settings.
 * @details Specified OTP_DBG2 register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_OTP_RESET_SET                0x80
#define PRESSURE20_OTP_RESET_CLEAR              0x00

/**
 * @brief Pressure 20 OTP_STATUS register settings.
 * @details Specified OTP_STATUS register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_BUSY                         0x01

/**
 * @brief Pressure 20 OTP_STATUS2 register settings.
 * @details Specified OTP_STATUS2 register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_BOOT_UP_STATUS               0x01

/**
 * @brief Pressure 20 MASTER_LOCK register settings.
 * @details Specified MASTER_LOCK register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_MASTER_LOCK                  0x00
#define PRESSURE20_MASTER_UNLOCK                0x1F

/**
 * @brief Pressure 20 MODE_SELECT register settings.
 * @details Specified MODE_SELECT register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_MEAS_CONFIG_MODE0_ODR_25HZ   0x00
#define PRESSURE20_MEAS_CONFIG_MODE1_ODR_120HZ  0x20
#define PRESSURE20_MEAS_CONFIG_MODE2_ODR_40HZ   0x40
#define PRESSURE20_MEAS_CONFIG_MODE3_ODR_2HZ    0x60
#define PRESSURE20_MEAS_CONFIG_MODE4            0x80
#define PRESSURE20_FORCED_MEAS_TRIGGER          0x10
#define PRESSURE20_MEAS_MODE_TRIGGER            0x00
#define PRESSURE20_MEAS_MODE_CONTINUOUS         0x08
#define PRESSURE20_POWER_MODE_NORMAL            0x00
#define PRESSURE20_POWER_MODE_ACTIVE            0x04
#define PRESSURE20_FIFO_READOUT_MODE_PRESS_1ST  0x00
#define PRESSURE20_FIFO_READOUT_MODE_TEMP_ONLY  0x01
#define PRESSURE20_FIFO_READOUT_MODE_TEMP_1ST   0x02
#define PRESSURE20_FIFO_READOUT_MODE_PRESS_ONLY 0x03

/**
 * @brief Pressure 20 DEVICE_STATUS register settings.
 * @details Specified DEVICE_STATUS register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_MODE_SYNC_STATUS             0x01

/**
 * @brief Pressure 20 FIFO_FILL register settings.
 * @details Specified FIFO_FILL register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_FIFO_FLUSH                   0x80
#define PRESSURE20_FIFO_EMPTY                   0x40
#define PRESSURE20_FIFO_FULL                    0x20
#define PRESSURE20_FIFO_LEVEL_EMPTY             0x00
#define PRESSURE20_FIFO_LEVEL_1                 0x01
#define PRESSURE20_FIFO_LEVEL_2                 0x02
#define PRESSURE20_FIFO_LEVEL_3                 0x03
#define PRESSURE20_FIFO_LEVEL_4                 0x04
#define PRESSURE20_FIFO_LEVEL_5                 0x05
#define PRESSURE20_FIFO_LEVEL_6                 0x06
#define PRESSURE20_FIFO_LEVEL_7                 0x07
#define PRESSURE20_FIFO_LEVEL_8                 0x08
#define PRESSURE20_FIFO_LEVEL_9                 0x09
#define PRESSURE20_FIFO_LEVEL_10                0x0A
#define PRESSURE20_FIFO_LEVEL_11                0x0B
#define PRESSURE20_FIFO_LEVEL_12                0x0C
#define PRESSURE20_FIFO_LEVEL_13                0x0D
#define PRESSURE20_FIFO_LEVEL_14                0x0E
#define PRESSURE20_FIFO_LEVEL_15                0x0F
#define PRESSURE20_FIFO_LEVEL_FULL              0x10

/**
 * @brief Pressure 20 INTERRUPT_STATUS register settings.
 * @details Specified INTERRUPT_STATUS register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_INT_STAT_PRESS_DELTA         0x40
#define PRESSURE20_INT_STAT_PRESS_ABS           0x20
#define PRESSURE20_INT_STAT_FIFO_WMK_LOW        0x08
#define PRESSURE20_INT_STAT_FIFO_WMK_HIGH       0x04
#define PRESSURE20_INT_STAT_FIFO_UNDERFLOW      0x02
#define PRESSURE20_INT_STAT_FIFO_OVERFLOW       0x01

/**
 * @brief Pressure 20 INTERRUPT_MASK register settings.
 * @details Specified INTERRUPT_MASK register settings of Pressure 20 Click driver.
 */
#define PRESSURE20_INT_MASK_RESERVED            0x80
#define PRESSURE20_INT_MASK_PRESS_DELTA         0x40
#define PRESSURE20_INT_MASK_PRESS_ABS           0x20
#define PRESSURE20_INT_MASK_FIFO_WMK_LOW        0x08
#define PRESSURE20_INT_MASK_FIFO_WMK_HIGH       0x04
#define PRESSURE20_INT_MASK_FIFO_UNDERFLOW      0x02
#define PRESSURE20_INT_MASK_FIFO_OVERFLOW       0x01
#define PRESSURE20_INT_MASK_ALL                 0xFF

/**
 * @brief Pressure 20 Device ID value.
 * @details Specified Device ID value of Pressure 20 Click driver.
 */
#define PRESSURE20_DEVICE_ID                    0x63

/**
 * @brief Pressure 20 pressure and temperature calculation values.
 * @details Specified pressure and temperature calculation values of Pressure 20 Click driver.
 */
#define PRESSURE20_PRESSURE_RES_RAW             0x020000ul
#define PRESSURE20_PRESSURE_RES_MBAR            400
#define PRESSURE20_PRESSURE_OFFSET_MBAR         700
#define PRESSURE20_TEMPERATURE_RES_RAW          0x040000ul
#define PRESSURE20_TEMPERATURE_RES_C            65
#define PRESSURE20_TEMPERATURE_OFFSET_C         25

/**
 * @brief Pressure 20 SPI Read/Write command.
 * @details Specified SPI Read/Write command of Pressure 20 Click driver.
 */
#define PRESSURE20_SPI_READ_REG                 0x3C
#define PRESSURE20_SPI_WRITE_REG                0x33

/**
 * @brief Pressure 20 device address setting.
 * @details Specified setting for device slave address selection of
 * Pressure 20 Click driver.
 */
#define PRESSURE20_DEVICE_ADDRESS_0             0x63
#define PRESSURE20_DEVICE_ADDRESS_1             0x64

#define DUMMY_READ_ADDR       0x00

/**
 * @brief Dummy data.
 * @details Definition of dummy data.
 */
#define DUMMY             0x00

int8_t ICP_RmCom_I2C_w( uint8_t *val, uint8_t num);
int8_t ICP_RmCom_I2C_r(uint8_t *val, uint8_t num);

int8_t pressure20_write_register(uint8_t reg, uint8_t *data_in, uint32_t len);
int8_t pressure20_read_register(uint8_t reg, uint8_t *data_out, uint32_t len);

int8_t pressure20_check_communication();
int8_t pressure20_write_mode_select(uint8_t mode_select );
int8_t pressure20_clear_interrupts();
int8_t pressure20_read_otp_data (uint8_t otp_address, uint8_t *data_out);
int8_t pressure20_read_register_data(uint8_t reg, uint8_t *data_out, uint32_t len);
int8_t pressure20_read_data(float *pressure, float *temperature);

int8_t pressure20_default_cfg();
int8_t application_init ( void );

//gpio_level_t pressure20_get_int_pin();
int8_t dummy_read();

///* Typedefs */
//typedef enum
//{
//    ub_0 = 0 << 4,
//    ub_1 = 1 << 4,
//    ub_2 = 2 << 4,
//    ub_3 = 3 << 4
//} userbank;
//
//#define AK09916_ADDRESS 0x0C
//
///* Registers ICM20948 USER BANK 0*/
//#define ICM20948_WHO_AM_I            0x00
//#define ICM20948_USER_CTRL           0x03
//#define ICM20948_LP_CONFIG           0x05
//#define ICM20948_PWR_MGMT_1          0x06
//#define ICM20948_PWR_MGMT_2          0x07
//#define ICM20948_INT_PIN_CFG         0x0F
//#define ICM20948_INT_ENABLE          0x10
//#define ICM20948_INT_ENABLE_1        0x11
//#define ICM20948_INT_ENABLE_2        0x12
//#define ICM20948_INT_ENABLE_3        0x13
//#define ICM20948_I2C_MST_STATUS      0x17
//#define ICM20948_INT_STATUS          0x19
//#define ICM20948_INT_STATUS_1        0x1A
//#define ICM20948_INT_STATUS_2        0x1B
//#define ICM20948_INT_STATUS_3        0x1C
//#define ICM20948_DELAY_TIME_H        0x28
//#define ICM20948_DELAY_TIME_L        0x29
//#define ICM20948_ACCEL_OUT           0x2D // accel data registers begin
//#define ICM20948_GYRO_OUT            0x33 // gyro data registers begin
//#define ICM20948_TEMP_OUT            0x39
//#define ICM20948_EXT_SLV_SENS_DATA_00  0x3B
//#define ICM20948_EXT_SLV_SENS_DATA_01  0x3C
//#define ICM20948_FIFO_EN_1           0x66
//#define ICM20948_FIFO_EN_2           0x67
//#define ICM20948_FIFO_RST            0x68
//#define ICM20948_FIFO_MODE           0x69
//#define ICM20948_FIFO_COUNT          0x70
//#define ICM20948_FIFO_R_W            0x72
//#define ICM20948_DATA_RDY_STATUS     0x74
//#define ICM20948_FIFO_CFG            0x76
//
///* Registers ICM20948 USER BANK 1*/
//#define ICM20948_SELF_TEST_X_GYRO    0x02
//#define ICM20948_SELF_TEST_Y_GYRO    0x03
//#define ICM20948_SELF_TEST_Z_GYRO    0x04
//#define ICM20948_SELF_TEST_X_ACCEL   0x0E
//#define ICM20948_SELF_TEST_Y_ACCEL   0x0F
//#define ICM20948_SELF_TEST_Z_ACCEL   0x10
//#define ICM20948_XA_OFFS_H           0x14
//#define ICM20948_XA_OFFS_L           0x15
//#define ICM20948_YA_OFFS_H           0x17
//#define ICM20948_YA_OFFS_L           0x18
//#define ICM20948_ZA_OFFS_H           0x1A
//#define ICM20948_ZA_OFFS_L           0x1B
//#define ICM20948_TIMEBASE_CORR_PLL   0x28
//
///* Registers ICM20948 USER BANK 2*/
//#define ICM20948_GYRO_SMPLRT_DIV     0x00
//#define ICM20948_GYRO_CONFIG_1       0x01
//#define ICM20948_GYRO_CONFIG_2       0x02
//#define ICM20948_XG_OFFS_USRH        0x03
//#define ICM20948_XG_OFFS_USRL        0x04
//#define ICM20948_YG_OFFS_USRH        0x05
//#define ICM20948_YG_OFFS_USRL        0x06
//#define ICM20948_ZG_OFFS_USRH        0x07
//#define ICM20948_ZG_OFFS_USRL        0x08
//#define ICM20948_ODR_ALIGN_EN        0x09
//#define ICM20948_ACCEL_SMPLRT_DIV_1  0x10
//#define ICM20948_ACCEL_SMPLRT_DIV_2  0x11
//#define ICM20948_ACCEL_INTEL_CTRL    0x12
//#define ICM20948_ACCEL_WOM_THR       0x13
//#define ICM20948_ACCEL_CONFIG        0x14
//#define ICM20948_ACCEL_CONFIG_2      0x15
//#define ICM20948_FSYNC_CONFIG        0x52
//#define ICM20948_TEMP_CONFIG         0x53
//#define ICM20948_MOD_CTRL_USR        0x54
//
///* Registers ICM20948 USER BANK 3*/
//#define ICM20948_I2C_MST_ODR_CFG     0x00
//#define ICM20948_I2C_MST_CTRL        0x01
//#define ICM20948_I2C_MST_DELAY_CTRL  0x02
//#define ICM20948_I2C_SLV0_ADDR       0x03
//#define ICM20948_I2C_SLV0_REG        0x04
//#define ICM20948_I2C_SLV0_CTRL       0x05
//#define ICM20948_I2C_SLV0_DO         0x06
//
///* Registers ICM20948 ALL BANKS */
//#define ICM20948_REG_BANK_SEL        0x7F
//
//
///* Registers AK09916 */
//#define AK09916_WIA_1    0x00 // Who I am, Company ID
//#define AK09916_WIA_2    0x01 // Who I am, Device ID
//#define AK09916_STATUS_1 0x10
//#define AK09916_HXL      0x11
//#define AK09916_HXH      0x12
//#define AK09916_HYL      0x13
//#define AK09916_HYH      0x14
//#define AK09916_HZL      0x15
//#define AK09916_HZH      0x16
//#define AK09916_STATUS_2 0x18
//#define AK09916_CNTL_2   0x31
//#define AK09916_CNTL_3   0x32
//
///* Register Bits */
//#define ICM20948_RESET              0x80
//#define ICM20948_I2C_MST_EN         0x20
//#define ICM20948_SLEEP              0x40
//#define ICM20948_LP_EN              0x20
//#define ICM20948_BYPASS_EN          0x02
//#define ICM20948_GYR_EN             0x07
//#define ICM20948_ACC_EN             0x38
//#define ICM20948_FIFO_EN            0x40
//#define ICM20948_INT1_ACTL          0x80
//#define ICM20948_INT_1_LATCH_EN     0x20
//#define ICM20948_ACTL_FSYNC         0x08
//#define ICM20948_INT_ANYRD_2CLEAR   0x10
//#define ICM20948_FSYNC_INT_MODE_EN  0x06
//#define AK09916_16_BIT              0x10
//#define AK09916_OVF                 0x08
//#define AK09916_READ                0x80
//
///* Others */
//#define AK09916_WHO_AM_I            0x4809
//#define ICM20948_ROOM_TEMP_OFFSET   0.0f
//#define ICM20948_T_SENSITIVITY      333.87f
//#define AK09916_MAG_LSB             0.1495f
//
//
///* Enums */
//
//typedef enum ICM20948_CYCLE {
//    ICM20948_NO_CYCLE              = 0x00,
//    ICM20948_GYR_CYCLE             = 0x10,
//    ICM20948_ACC_CYCLE             = 0x20,
//    ICM20948_ACC_GYR_CYCLE         = 0x30,
//    ICM20948_ACC_GYR_I2C_MST_CYCLE = 0x70
//} ICM20948_cycle;
//
//typedef enum ICM20948_INT_PIN_POL {
//    ICM20948_ACT_HIGH, ICM20948_ACT_LOW
//} ICM20948_intPinPol;
//
//typedef enum ICM20948_INT_TYPE {
//    ICM20948_FSYNC_INT      = 0x01,
//    ICM20948_WOM_INT        = 0x02,
//    ICM20948_DMP_INT        = 0x04,
//    ICM20948_DATA_READY_INT = 0x08,
//    ICM20948_FIFO_OVF_INT   = 0x10,
//    ICM20948_FIFO_WM_INT    = 0x20
//} ICM20948_intType;
//
//typedef enum ICM20948_FIFO_TYPE {
//    ICM20948_FIFO_ACC        = 0x10,
//    ICM20948_FIFO_GYR        = 0x0E,
//    ICM20948_FIFO_ACC_GYR    = 0x1E
//} ICM20948_fifoType;
//
//typedef enum ICM20948_FIFO_MODE_CHOICE {
//    ICM20948_CONTINUOUS, ICM20948_STOP_WHEN_FULL
//} ICM20948_fifoMode;
//
//typedef enum ICM20948_GYRO_RANGE {
//    ICM20948_GYRO_RANGE_250, ICM20948_GYRO_RANGE_500, ICM20948_GYRO_RANGE_1000, ICM20948_GYRO_RANGE_2000
//} ICM20948_gyroRange;
//
//typedef enum ICM20948_DLPF {
//    ICM20948_DLPF_0, ICM20948_DLPF_1, ICM20948_DLPF_2, ICM20948_DLPF_3, ICM20948_DLPF_4, ICM20948_DLPF_5,
//    ICM20948_DLPF_6, ICM20948_DLPF_7, ICM20948_DLPF_OFF
//} ICM20948_dlpf;
//
//typedef enum ICM20948_GYRO_AVG_LOW_PWR {
//    ICM20948_GYR_AVG_1, ICM20948_GYR_AVG_2, ICM20948_GYR_AVG_4, ICM20948_GYR_AVG_8, ICM20948_GYR_AVG_16,
//    ICM20948_GYR_AVG_32, ICM20948_GYR_AVG_64, ICM20948_GYR_AVG_128
//} ICM20948_gyroAvgLowPower;
//
//typedef enum ICM20948_ACC_RANGE {
//    ICM20948_ACC_RANGE_2G, ICM20948_ACC_RANGE_4G, ICM20948_ACC_RANGE_8G, ICM20948_ACC_RANGE_16G
//} ICM20948_accRange;
//
//typedef enum ICM20948_ACC_AVG_LOW_PWR {
//    ICM20948_ACC_AVG_4, ICM20948_ACC_AVG_8, ICM20948_ACC_AVG_16, ICM20948_ACC_AVG_32
//} ICM20948_accAvgLowPower;
//
//typedef enum ICM20948_WOM_COMP {
//    ICM20948_WOM_COMP_DISABLE, ICM20948_WOM_COMP_ENABLE
//} ICM20948_womCompEn;
//
//typedef enum AK09916_OP_MODE {
//    AK09916_PWR_DOWN           = 0x00,
//    AK09916_TRIGGER_MODE       = 0x01,
//    AK09916_CONT_MODE_10HZ     = 0x02,
//    AK09916_CONT_MODE_20HZ     = 0x04,
//    AK09916_CONT_MODE_50HZ     = 0x06,
//    AK09916_CONT_MODE_100HZ    = 0x08
//} AK09916_opMode;
//
//typedef enum ICM20948_ORIENTATION {
//    ICM20948_FLAT, ICM20948_FLAT_1, ICM20948_XY, ICM20948_XY_1, ICM20948_YX, ICM20948_YX_1
//} ICM20948_orientation;
//
//typedef struct  {
//    float x;
//    float y;
//    float z;
//}xyzFloat;
//
//
//
//
//
//
//
///* Basic settings */
//
//bool icm20948_init();
//void autoOffsets();
//void setAccOffsets(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
//void setGyrOffsets(float xOffset, float yOffset, float zOffset);
//uint8_t whoAmI();
//void enableAcc(bool enAcc);
//void setAccRange(ICM20948_accRange accRange);
//void setAccDLPF(ICM20948_dlpf dlpf);
//void setAccSampleRateDivider(uint16_t accSplRateDiv);
//void enableGyr(bool enGyr);
//void setGyrRange(ICM20948_gyroRange gyroRange);
//void setGyrDLPF(ICM20948_dlpf dlpf);
//void setGyrSampleRateDivider(uint8_t gyrSplRateDiv);
//void setTempDLPF(ICM20948_dlpf dlpf);
//void setI2CMstSampleRate(uint8_t rateExp);
//
//
///* x,y,z results */
//
//void readSensor();
//xyzFloat getAccRawValues();
//xyzFloat getCorrectedAccRawValues();
//xyzFloat getGValues();
//xyzFloat getAccRawValuesFromFifo();
//xyzFloat getCorrectedAccRawValuesFromFifo();
//xyzFloat getGValuesFromFifo();
//float getResultantG(xyzFloat gVal);
//float getTemperature();
//xyzFloat getGyrRawValues();
//xyzFloat getCorrectedGyrRawValues();
//xyzFloat getGyrValues();
//xyzFloat getGyrValuesFromFifo();
//xyzFloat getMagValues();
//
//
///* Angles and Orientation */
//
//xyzFloat getAngles();
//ICM20948_orientation getOrientation();
////String getOrientationAsString();
//float getPitch();
//float getRoll();
//
//
///* Power, Sleep, Standby */
//
//void enableCycle(ICM20948_cycle cycle);
//void enableLowPower(bool enLP);
//void setGyrAverageInCycleMode(ICM20948_gyroAvgLowPower avg);
//void setAccAverageInCycleMode(ICM20948_accAvgLowPower avg);
//void sleep(bool sleep);
//
//
///* Interrupts */
//
//void setIntPinPolarity(ICM20948_intPinPol pol);
//void enableIntLatch(bool latch);
//void enableClearIntByAnyRead(bool clearByAnyRead);
//void setFSyncIntPolarity(ICM20948_intPinPol pol);
//void enableInterrupt(ICM20948_intType intType);
//void disableInterrupt(ICM20948_intType intType);
//uint8_t readAndClearInterrupts();
//bool checkInterrupt(uint8_t source, ICM20948_intType type);
//void setWakeOnMotionThreshold(uint8_t womThresh, ICM20948_womCompEn womCompEn);
//
//
///* FIFO */
//
//void enableFifo(bool fifo);
//void setFifoMode(ICM20948_fifoMode mode);
//void startFifo(ICM20948_fifoType fifo);
//void stopFifo();
//void resetFifo();
//int16_t getFifoCount();
//int16_t getNumberOfFifoDataSets();
//void findFifoBegin();
//
//
///* Magnetometer */
//
//bool initMagnetometer();
//int16_t whoAmIMag();
//void setMagOpMode(AK09916_opMode opMode);
//void resetMag();
//
//
////TwoWire *_wire;
//int i2cAddress;
//uint8_t currentBank;
//uint8_t buffer[20];
//xyzFloat accOffsetVal;
//xyzFloat accCorrFactor;
//xyzFloat gyrOffsetVal;
//uint8_t accRangeFactor;
//uint8_t gyrRangeFactor;
//uint8_t regVal;   // intermediate storage of register values
//ICM20948_fifoType fifoType;
//
//void setClockToAutoSelect();
//xyzFloat correctAccRawValues(xyzFloat accRawVal);
//xyzFloat correctGyrRawValues(xyzFloat gyrRawVal);
//void switchBank(uint8_t newBank);
//uint8_t writeRegister8(uint8_t bank, uint8_t reg, uint8_t val);
//uint8_t writeRegister16(uint8_t bank, uint8_t reg, int16_t val);
//uint8_t readRegister8(uint8_t bank, uint8_t reg);
//int16_t readRegister16(uint8_t bank, uint8_t reg);
//void readAllData(uint8_t* data);
//xyzFloat readICM20948xyzValFromFifo();
//void writeAK09916Register8(uint8_t reg, uint8_t val);
//uint8_t readAK09916Register8(uint8_t reg);
//int16_t readAK09916Register16(uint8_t reg);
//uint8_t reset_ICM20948();
//void enableI2CMaster();
//void enableMagDataRead(uint8_t reg, uint8_t bytes);




#endif /* ICP_20100_H_ */
