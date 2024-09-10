/*
 * ICM_20948.c
 *
 *  Created on: Jan 20, 2022
 *      Author: Vivek
 */

#include "ICM_20948.h"
#include "RmcI2C.h"
#include "math.h"
#include "common_utils.h"

#define sq(x)  (x*x)
#define M_PI        3.14159265358979323846

char RawAcc_x[20],RawAcc_y[20],RawAcc_z[20];
char CorrAcc_x[20],CorrAcc_y[20],CorrAcc_z[20];
char Gval_x[20],Gval_y[20],Gval_z[20];
char RawGyro_x[20],RawGyro_y[20],RawGyro_z[20];
char CorGyro_x[20],CorGyro_y[20],CorGyro_z[20];
char Mag_x[20],Mag_y[20],Mag_z[20];
char ResultG[20];

float resultantG;

int i2cAddress;

uint8_t currentBank;
uint8_t buffer[20];
uint8_t accRangeFactor;
uint8_t gyrRangeFactor;
uint8_t regVal;   // intermediate storage of register values

xyzFloat corrAccRaw;
xyzFloat accRaw;
xyzFloat gVal;
xyzFloat gyrRaw;
xyzFloat gyr;
xyzFloat magValue;
xyzFloat accOffsetVal;
xyzFloat accCorrFactor;
xyzFloat gyrOffsetVal;

ICM20948_fifoType fifoType;

/**************************************************************************************
 * @brief     Get ICM_20948 Motion Tracking sensor data
 * @param[in]
 * @retval
 **************************************************************************************/
void ICM_20948_get(void)
{
    vTaskDelay (50);

    readSensor ();
    getAccRawValues (&accRaw);
    getCorrectedAccRawValues (&corrAccRaw);
    getGValues (&gVal);
    resultantG = (float) getResultantG (gVal);
    getGyrValues (&gyr);
    getMagValues (&magValue); // returns magnetic flux density [µT]

    vTaskDelay (50);
}

/**************************************************************************************
 * @brief     Initialize ICM_20948 Motion Tracking sensor
 * @param[in]
 * @retval
 **************************************************************************************/

bool icm20948_init(void)
{
    reset_ICM20948 ();
    accOffsetVal.x = 0.0;
    accOffsetVal.y = 0.0;
    accOffsetVal.z = 0.0;

    accCorrFactor.x = 1.0;
    accCorrFactor.y = 1.0;
    accCorrFactor.z = 1.0;

    accRangeFactor = 1.0;

    gyrOffsetVal.x = 0.0;
    gyrOffsetVal.y = 0.0;
    gyrOffsetVal.z = 0.0;

    gyrRangeFactor = 1.0;

    fifoType = ICM20948_FIFO_ACC;
    sleep (false);
    writeRegister8 (2, ICM20948_ODR_ALIGN_EN, 1);

    writeRegister8 (2, ICM20948_ODR_ALIGN_EN, 1);
    return 0;
}

/**************************************************************************************
 * @brief     Set offset values for ICM_20948 Motion Tracking sensor
 * @param[in]
 * @retval
 **************************************************************************************/

void autoOffsets(void)
{
    xyzFloat accRawVal, gyrRawVal;
    accOffsetVal.x = 0.0;
    accOffsetVal.y = 0.0;
    accOffsetVal.z = 0.0;

    /* Set to lowest noise */
    setGyrDLPF (ICM20948_DLPF_6);
    /* Set to highest resolution */
    setGyrRange (ICM20948_GYRO_RANGE_250);
    setAccRange (ICM20948_ACC_RANGE_2G);
    setAccDLPF (ICM20948_DLPF_6);
    vTaskDelay (50);

    for (int i = 0; i < 50; i++)
    {
        readSensor ();
        getAccRawValues (&accRawVal);
        accOffsetVal.x += accRawVal.x;
        accOffsetVal.y += accRawVal.y;
        accOffsetVal.z += accRawVal.z;
        vTaskDelay (10);
    }

    accOffsetVal.x /= 50;
    accOffsetVal.y /= 50;
    accOffsetVal.z /= 50;
    accOffsetVal.z -= 16384.0;

    for (int i = 0; i < 50; i++)
    {
        readSensor ();
        getGyrRawValues (&gyrRawVal);

        gyrOffsetVal.x += gyrRawVal.x;
        gyrOffsetVal.y += gyrRawVal.y;
        gyrOffsetVal.z += gyrRawVal.z;
        _delay (1);
    }

    gyrOffsetVal.x /= 50;
    gyrOffsetVal.y /= 50;
    gyrOffsetVal.z /= 50;
}

/**************************************************************************************
 * @brief     Set Gyrometer offset ICM_20948 Motion Tracking sensor
 * @param[in] xOffset  yOffset zOffset : x,y,z axis offset value
 * @retval
 **************************************************************************************/

void setGyrOffsets(float xOffset, float yOffset, float zOffset)
{
    gyrOffsetVal.x = xOffset;
    gyrOffsetVal.y = yOffset;
    gyrOffsetVal.z = zOffset;
}

/* Sub Functions */

/**************************************************************************************
 * @brief     Read ICM_20948 Motion Tracking sensor Identification Number
 * @param[in]
 * @retval    TRUE if identified as a ICM 20948,FALSE otherwise
 **************************************************************************************/

uint8_t whoAmI(void)
{
    uint8_t icm20948_id = readRegister8 (0, ICM20948_WHO_AM_I);
    if (icm20948_id == 0xEA)
        return true;
    else
        return false;
}

/**************************************************************************************
 * @brief     Set Accelerometer range ICM_20948 Motion Tracking sensor
 * @param[in] accRange Gravitational force limit enumeration
 * @retval
 **************************************************************************************/

void setAccRange(ICM20948_accRange accRange)
{
    regVal = readRegister8 (2, ICM20948_ACCEL_CONFIG);
    regVal = (uint8_t) (regVal & ~(ICM20948_PWR_MGMT_1 ));
    regVal = (uint8_t) (regVal | (accRange << Marco_One ));
    writeRegister8 (MACRO_TWO, ICM20948_ACCEL_CONFIG, regVal);
    accRangeFactor = (uint8_t) (Marco_One << accRange);
}

/**************************************************************************************
 * @brief     Set Accelerometer Low Pass Filter frequency
 * @param[in] dlpf Low Pass Filter frequency
 * @retval
 **************************************************************************************/

void setAccDLPF(ICM20948_dlpf dlpf)
{
    regVal = readRegister8 (2, ICM20948_ACCEL_CONFIG);
    if (dlpf == ICM20948_DLPF_OFF)
    {
        regVal &= 0xFE;
        writeRegister8 (2, ICM20948_ACCEL_CONFIG, regVal);
        return;
    }
    else
    {
        regVal |= 0x01;
        regVal &= 0xC7;
        regVal = (uint8_t) (regVal | (dlpf << 3));
    }
    writeRegister8 (Bank_2, ICM20948_ACCEL_CONFIG, regVal);
}

/**************************************************************************************
 * @brief     Set Accelerometer Sample Rate Divider
 * @param[in] accSplRateDiv  Sample Rate Divider value
 * @retval
 **************************************************************************************/

void setAccSampleRateDivider(uint16_t accSplRateDiv)
{
    writeRegister8 (Bank_2, ICM20948_ACCEL_SMPLRT_DIV_1, (uint8_t) accSplRateDiv);
}

/**************************************************************************************
 * @brief     Set Gyroscope range
 * @param[in] gyroRange  Gyroscope range enumeration
 * @retval
 **************************************************************************************/

void setGyrRange(ICM20948_gyroRange gyroRange)
{
    regVal = readRegister8 (Bank_2, ICM20948_GYRO_CONFIG_1);
    regVal = (uint8_t) (regVal & (~(ICM20948_YG_OFFS_USRL )));
    regVal |= (uint8_t) (gyroRange << (uint8_t) 1);
    writeRegister8 (Bank_2, ICM20948_GYRO_CONFIG_1, regVal);
    gyrRangeFactor = (uint8_t) (Marco_One << gyroRange);
}

/**************************************************************************************
 * @brief     Set Gyroscope Low Pass Filter Frequency
 * @param[in] dlpf  Low Pass Filter Frequency enumeration
 * @retval
 **************************************************************************************/
void setGyrDLPF(ICM20948_dlpf dlpf)
{
    regVal = readRegister8 (Bank_2, ICM20948_GYRO_CONFIG_1);
    if (dlpf == ICM20948_DLPF_OFF)
    {
        regVal &= 0xFE;
        writeRegister8 (Bank_2, ICM20948_GYRO_CONFIG_1, regVal);
        return;
    }
    else
    {
        regVal |= 0x01;
        regVal &= 0xC7;
        regVal |= (uint8_t) (dlpf << 3);
    }
    writeRegister8 (Bank_2, ICM20948_GYRO_CONFIG_1, regVal);
}

/**************************************************************************************
 * @brief     Set Gyroscope Sample Rate Divider
 * @param[in] gyrSplRateDiv  Sample Rate Divider Value
 * @retval
 **************************************************************************************/

void setGyrSampleRateDivider(uint8_t gyrSplRateDiv)
{
    writeRegister8 (Bank_2, ICM20948_GYRO_SMPLRT_DIV, gyrSplRateDiv);
}

/**************************************************************************************
 * @brief     Read Sensor Data
 * @param[in]
 * @retval
 **************************************************************************************/
void readSensor(void)
{
    readAllData (buffer);
}

/**************************************************************************************
 * @brief     Get Accelerometer raw data
 * @param[in] a pointer to axis structure
 * @retval
 **************************************************************************************/

void getAccRawValues(xyzFloat *a)
{
    a->x = (int16_t) (((buffer[0]) << 8) | buffer[1]) * 1.0;
    a->y = (int16_t) (((buffer[2]) << 8) | buffer[3]) * 1.0;
    a->z = (int16_t) (((buffer[4]) << 8) | buffer[5]) * 1.0;
}

/**************************************************************************************
 * @brief     Get Corrected Accelerometer raw data
 * @param[in] a pointer to axis structure
 * @retval
 **************************************************************************************/
void getCorrectedAccRawValues(xyzFloat *a)
{
    getAccRawValues (a);
    correctAccRawValues (a);
}

/**************************************************************************************
 * @brief     Get  Accelerometer  data
 * @param[in] a pointer to axis structure
 * @retval
 **************************************************************************************/
void getGValues(xyzFloat *a)
{
    xyzFloat accRawVal;
    getCorrectedAccRawValues (&accRawVal);

    a->x = accRawVal.x * accRangeFactor / 16384.0;
    a->y = accRawVal.y * accRangeFactor / 16384.0;
    a->z = accRawVal.z * accRangeFactor / 16384.0;
}

/**************************************************************************************
 * @brief     Get  Accelerometer Raw  data from FIFO
 * @param[in] a pointer to axis structure
 * @retval
 **************************************************************************************/
void getAccRawValuesFromFifo(xyzFloat *a)
{
    readICM20948xyzValFromFifo (a);
}

/**************************************************************************************
 * @brief     Get  Accelerometer Raw  data from FIFO
 * @param[in] a pointer to axis structure
 * @retval
 **************************************************************************************/

void getCorrectedAccRawValuesFromFifo(xyzFloat *a)
{
    getAccRawValuesFromFifo (a);
    correctAccRawValues (a);
}

/**************************************************************************************
 * @brief     Get  Resultant G force
 * @param[in] gVal1     pointer to axis structure
 * @retval    resultant Resultant G force
 **************************************************************************************/

double getResultantG(xyzFloat gVal1)
{
    double resultant = 0.0;
    resultant = sqrt (sq(gVal1.x) + sq(gVal1.y) + sq(gVal1.z));
    return resultant;
}

/**************************************************************************************
 * @brief     Get  Gyroscope Raw Values
 * @param[in] a    pointer to axis structure
 * @retval
 **************************************************************************************/

void getGyrRawValues(xyzFloat *a)
{
    a->x = (int16_t) ((int16_t) ((buffer[6]) << 8) | buffer[7]) * 1.0;
    a->y = (int16_t) ((int16_t) ((buffer[8]) << 8) | buffer[9]) * 1.0;
    a->z = (int16_t) ((int16_t) ((buffer[10]) << 8) | buffer[11]) * 1.0;
}

/**************************************************************************************
 * @brief     Get corrected Gyroscope raw value
 * @param[in] a    pointer to axis structure
 * @retval
 **************************************************************************************/
void getCorrectedGyrRawValues(xyzFloat *a)
{
    getGyrRawValues (a);
    correctGyrRawValues (a);
}

/**************************************************************************************
 * @brief     Get  Gyroscope  value
 * @param[in] a    pointer to axis structure
 * @retval
 **************************************************************************************/
void getGyrValues(xyzFloat *a)
{
    getCorrectedGyrRawValues (a);

    a->x = a->x * gyrRangeFactor * 250.0 / 32768.0;
    a->y = a->y * gyrRangeFactor * 250.0 / 32768.0;
    a->z = a->z * gyrRangeFactor * 250.0 / 32768.0;
}
/**************************************************************************************
 * @brief     Get  Magnometer  value
 * @param[in] a    pointer to axis structure
 * @retval
 **************************************************************************************/
void getMagValues(xyzFloat *a)
{
    int16_t x, y, z;

    x = (int16_t) ((int16_t) ((buffer[15]) << 8) | buffer[14]);
    y = (int16_t) ((int16_t) ((buffer[17]) << 8) | buffer[16]);
    z = (int16_t) ((int16_t) ((buffer[19]) << 8) | buffer[18]);

    a->x = x * AK09916_MAG_LSB;
    a->y = y * AK09916_MAG_LSB;
    a->z = z * AK09916_MAG_LSB;
}

/********* Power, Sleep, Standby *********/

/**************************************************************************************
 * @brief     Enable  Power  Cycle
 * @param[in]  cycle
 * @retval
 **************************************************************************************/
void enableCycle(ICM20948_cycle cycle)
{
    regVal = readRegister8 (0, ICM20948_LP_CONFIG);
    regVal &= 0x0F;
    regVal |= cycle;
    writeRegister8 (0, ICM20948_LP_CONFIG, regVal);
}

/**************************************************************************************
 * @brief     Enable Low Power mode
 * @param[in]  enLP enable low power
 * @retval
 **************************************************************************************/
void enableLowPower(bool enLP)
{
    regVal = readRegister8 (0, ICM20948_PWR_MGMT_1);
    if (enLP)
    {
        regVal |= ICM20948_LP_EN;
    }
    else
    {
        regVal &= (uint8_t) (~ICM20948_LP_EN );
    }
    writeRegister8 (0, ICM20948_PWR_MGMT_1, regVal);
}

/**************************************************************************************
 * @brief     Enable Sensor sleep
 * @param[in]  sleep enable sleep mode
 * @retval
 **************************************************************************************/
void sleep(bool sleep)
{
    regVal = readRegister8 (0, ICM20948_PWR_MGMT_1);
    if (sleep)
    {
        regVal |= ICM20948_SLEEP;
    }
    else
    {
        regVal &= (uint8_t) (~ICM20948_SLEEP );
    }
    writeRegister8 (0, ICM20948_PWR_MGMT_1, regVal);
}

/******** Angles and Orientation *********/
/**************************************************************************************
 * @brief    Get Angles
 * @param[in]  a  pointer to axis structure
 * @retval
 **************************************************************************************/
void getAngles(xyzFloat *a)
{

    xyzFloat _gVal;
    getGValues (&_gVal);
    if (_gVal.x > 1.0)
    {
        _gVal.x = 1.0;
    }
    else if (_gVal.x < -1.0)
    {
        _gVal.x = -1.0;
    }
    a->x = (asin (_gVal.x)) * 57.296;

    if (_gVal.y > 1.0)
    {
        _gVal.y = 1.0;
    }
    else if (_gVal.y < -1.0)
    {
        _gVal.y = -1.0;
    }
    a->y = (asin (_gVal.y)) * 57.296;

    if (_gVal.z > 1.0)
    {
        _gVal.z = 1.0;
    }
    else if (_gVal.z < -1.0)
    {
        _gVal.z = -1.0;
    }
    a->z = (asin (_gVal.z)) * 57.296;
}

/**************************************************************************************
 * @brief    Get Orientation
 * @param[in]
 * @retval orientation
 **************************************************************************************/

ICM20948_orientation getOrientation(void)
{
    xyzFloat angleVal;
    getAngles (&angleVal);
    ICM20948_orientation orientation = ICM20948_FLAT;
    if ((uint32_t) (angleVal.x) < (uint32_t) 45)
    {      // |x| < 45
        if ((uint32_t) (angleVal.y) < (uint32_t) 45)
        {      // |y| < 45
            if (angleVal.z > 0)
            {          //  z  > 0
                orientation = ICM20948_FLAT;
            }
            else
            {                        //  z  < 0
                orientation = ICM20948_FLAT_1;
            }
        }
        else
        {                         // |y| > 45
            if (angleVal.y > 0)
            {         //  y  > 0
                orientation = ICM20948_XY;
            }
            else
            {                       //  y  < 0
                orientation = ICM20948_XY_1;
            }
        }
    }
    else
    {                           // |x| >= 45
        if (angleVal.x > 0)
        {           //  x  >  0
            orientation = ICM20948_YX;
        }
        else
        {                       //  x  <  0
            orientation = ICM20948_YX_1;
        }
    }
    return orientation;
}

/**************************************************************************************
 * @brief    Get Pitch value
 * @param[in]
 * @retval pitch  pitch value
 **************************************************************************************/
float getPitch(void)
{
    xyzFloat angleVal;
    getAngles (&angleVal);
    float pitch = (float) ((atan2 (
            angleVal.x,
            (double) (sqrt (abs ((int) (angleVal.x * angleVal.y + angleVal.z * angleVal.z)))) * (double) 180.0) / M_PI));
    return pitch;
}

/**************************************************************************************
 * @brief    Get Roll value
 * @param[in]
 * @retval roll  pitch value
 **************************************************************************************/
float getRoll()
{
    xyzFloat angleVal;
    getAngles (&angleVal);
    float roll = (float) ((atan2 (angleVal.y, angleVal.z) * 180.0) / M_PI);
    return roll;
}

/************** Interrupts ***************/
/**************************************************************************************
 * @brief   Set Interrupt  Pin Polarity
 * @param[in] pol polarity
 * @retval
 **************************************************************************************/
void setIntPinPolarity(ICM20948_intPinPol pol)
{
    regVal = readRegister8 (0, ICM20948_INT_PIN_CFG);
    if (pol)
    {
        regVal |= ICM20948_INT1_ACTL;
    }
    else
    {
        regVal &= (uint8_t) ~ICM20948_INT1_ACTL;
    }
    writeRegister8 (0, ICM20948_INT_PIN_CFG, regVal);
}

/**************************************************************************************
 * @brief   Enable Interrupt Latch
 * @param[in] latch
 * @retval
 **************************************************************************************/
void enableIntLatch(bool latch)
{
    regVal = readRegister8 (0, ICM20948_INT_PIN_CFG);
    if (latch)
    {
        regVal |= ICM20948_INT_1_LATCH_EN;
    }
    else
    {
        regVal &= (uint8_t) ~ICM20948_INT_1_LATCH_EN;
    }
    writeRegister8 (0, ICM20948_INT_PIN_CFG, regVal);
}

/**************************************************************************************
 * @brief   Enable Clear Interrupt by Any read
 * @param[in] clearByAnyRead
 * @retval
 **************************************************************************************/
void enableClearIntByAnyRead(bool clearByAnyRead)
{
    regVal = readRegister8 (0, ICM20948_INT_PIN_CFG);
    if (clearByAnyRead)
    {
        regVal |= ICM20948_INT_ANYRD_2CLEAR;
    }
    else
    {
        regVal &= (uint8_t) ~ICM20948_INT_ANYRD_2CLEAR;
    }
    writeRegister8 (0, ICM20948_INT_PIN_CFG, regVal);
}

/**************************************************************************************
 * @brief   setFSyncIntPolarity
 * @param[in] pol polarity
 * @retval
 **************************************************************************************/
void setFSyncIntPolarity(ICM20948_intPinPol pol)
{
    regVal = readRegister8 (0, ICM20948_INT_PIN_CFG);
    if (pol)
    {
        regVal |= ICM20948_ACTL_FSYNC;
    }
    else
    {
        regVal &= (uint8_t) ~ICM20948_ACTL_FSYNC;
    }
    writeRegister8 (0, ICM20948_INT_PIN_CFG, regVal);
}

/**************************************************************************************
 * @brief   Enable Interrupt
 * @param[in] intType Interrupt type
 * @retval
 **************************************************************************************/
void enableInterrupt(ICM20948_intType intType)
{
    switch (intType)
    {
        case ICM20948_FSYNC_INT:
            regVal = readRegister8 (0, ICM20948_INT_PIN_CFG);
            regVal |= ICM20948_FSYNC_INT_MODE_EN;
            writeRegister8 (0, ICM20948_INT_PIN_CFG, regVal); // enable FSYNC as interrupt pin
            regVal = readRegister8 (0, ICM20948_INT_ENABLE);
            regVal |= 0x80;
            writeRegister8 (0, ICM20948_INT_ENABLE, regVal); // enable wake on FSYNC interrupt
        break;

        case ICM20948_WOM_INT:
            regVal = readRegister8 (0, ICM20948_INT_ENABLE);
            regVal |= 0x08;
            writeRegister8 (0, ICM20948_INT_ENABLE, regVal);
            regVal = readRegister8 (2, ICM20948_ACCEL_INTEL_CTRL);
            regVal |= 0x02;
            writeRegister8 (2, ICM20948_ACCEL_INTEL_CTRL, regVal);
        break;

        case ICM20948_DMP_INT:
            regVal = readRegister8 (0, ICM20948_INT_ENABLE);
            regVal |= 0x02;
            writeRegister8 (0, ICM20948_INT_ENABLE, regVal);
        break;

        case ICM20948_DATA_READY_INT:
            writeRegister8 (0, ICM20948_INT_ENABLE_1, 0x01);
        break;

        case ICM20948_FIFO_OVF_INT:
            writeRegister8 (0, ICM20948_INT_ENABLE_2, 0x01);
        break;

        case ICM20948_FIFO_WM_INT:
            writeRegister8 (0, ICM20948_INT_ENABLE_3, 0x01);
        break;
    }
}

/**************************************************************************************
 * @brief   Disable Interrupt
 * @param[in] intType Interrupt type
 * @retval
 **************************************************************************************/
void disableInterrupt(ICM20948_intType intType)
{
    switch (intType)
    {
        case ICM20948_FSYNC_INT:
            regVal = readRegister8 (Bank_0, (uint8_t) ICM20948_INT_PIN_CFG);
            regVal = (uint8_t) (regVal & (~(uint8_t) (ICM20948_FSYNC_INT_MODE_EN )));
            writeRegister8 (Bank_0, ICM20948_INT_PIN_CFG, regVal);
            regVal = readRegister8 (Bank_0, (uint8_t) ICM20948_INT_ENABLE);
            regVal = (uint8_t) (regVal & (~(uint8_t) (ICM20948_INT1_ACTL )));
            writeRegister8 (Bank_0, ICM20948_INT_ENABLE, regVal);
        break;

        case ICM20948_WOM_INT:
            regVal = readRegister8 (Bank_0, (uint8_t) ICM20948_INT_ENABLE);
            regVal = (uint8_t) (regVal & ~(ICM20948_ZG_OFFS_USRL ));
            writeRegister8 (Bank_0, ICM20948_INT_ENABLE, regVal);
            regVal = readRegister8 (Bank_2, (uint8_t) ICM20948_ACCEL_INTEL_CTRL);
            regVal = (uint8_t) (regVal & (~(ICM20948_GYRO_CONFIG_2 )));
            writeRegister8 (Bank_2, ICM20948_ACCEL_INTEL_CTRL, regVal);
        break;

        case ICM20948_DMP_INT:
            regVal = readRegister8 (Bank_0, ICM20948_INT_ENABLE);
            regVal = (uint8_t) (regVal & ~(ICM20948_GYRO_CONFIG_2 ));
            writeRegister8 (Bank_0, ICM20948_INT_ENABLE, regVal);
        break;

        case ICM20948_DATA_READY_INT:
            writeRegister8 (Bank_0, ICM20948_INT_ENABLE_1, 0x00);
        break;

        case ICM20948_FIFO_OVF_INT:
            writeRegister8 (Bank_0, ICM20948_INT_ENABLE_2, 0x00);
        break;

        case ICM20948_FIFO_WM_INT:
            writeRegister8 (Bank_0, ICM20948_INT_ENABLE_3, 0x00);
        break;
    }
}

/**************************************************************************************
 * @brief   Read And Clear Interrupts
 * @param[in]
 * @retval intSource Interrupt Source
 **************************************************************************************/
uint8_t readAndClearInterrupts(void)
{
    uint8_t intSource = 0;
    regVal = readRegister8 (Bank_0, ICM20948_I2C_MST_STATUS);
    if (regVal & 0x80)
    {
        intSource |= 0x01;
    }
    regVal = readRegister8 (0, ICM20948_INT_STATUS);
    if (regVal & 0x08)
    {
        intSource |= 0x02;
    }
    if (regVal & 0x02)
    {
        intSource |= 0x04;
    }
    regVal = readRegister8 (0, ICM20948_INT_STATUS_1);
    if (regVal & 0x01)
    {
        intSource |= 0x08;
    }
    regVal = readRegister8 (0, ICM20948_INT_STATUS_2);
    if (regVal & 0x01)
    {
        intSource |= 0x10;
    }
    regVal = readRegister8 (0, ICM20948_INT_STATUS_3);
    if (regVal & 0x01)
    {
        intSource |= 0x20;
    }
    return intSource;
}

/**************************************************************************************
 * @brief   Check Interrupts
 * @param[in]   'source' Interrupt Source,
 * @param[in]   'type' Interrupt type
 * @retval      source Interrupt Source
 **************************************************************************************/
bool checkInterrupt(uint8_t source, ICM20948_intType type)
{
    source &= type;
    return source;
}

/**************************************************************************************
 * @brief   Set WakeOn MotionThreshold
 * @param[in]   'womThresh' Threshold level,
 * @param[in]   'womCompEn' Comparison Enable
 * @retval
 **************************************************************************************/

void setWakeOnMotionThreshold(uint8_t womThresh, ICM20948_womCompEn womCompEn)
{
    regVal = readRegister8 (2, ICM20948_ACCEL_INTEL_CTRL);
    if (womCompEn)
    {
        regVal |= 0x01;
    }
    else
    {
        regVal = (uint8_t) (regVal & (~(0x01)));
    }
    writeRegister8 (Bank_2, ICM20948_ACCEL_INTEL_CTRL, regVal);
    writeRegister8 (Bank_2, ICM20948_ACCEL_WOM_THR, womThresh);
}

/***************** FIFO ******************/
/**************************************************************************************
 * @brief   Enable Fifo
 * @param[in]   'fifo'  bool
 * @param[in]
 * @retval
 **************************************************************************************/
void enableFifo(bool fifo)
{
    regVal = readRegister8 (Bank_0, ICM20948_USER_CTRL);
    if (fifo)
    {
        regVal |= ICM20948_FIFO_EN;
    }
    else
    {
        regVal = (uint8_t) (regVal & (~ICM20948_FIFO_EN ));
    }
    writeRegister8 (Bank_0, ICM20948_USER_CTRL, regVal);
}

/**************************************************************************************
 * @brief   SetFifoMode
 * @param[in]   'mode' fifoMode enumeration
 * @param[in]
 * @retval
 **************************************************************************************/
void setFifoMode(ICM20948_fifoMode mode)
{
    if (mode)
    {
        regVal = 0x01;
    }
    else
    {
        regVal = 0x00;
    }
    writeRegister8 (Bank_0, ICM20948_FIFO_MODE, regVal);
}

/**************************************************************************************
 * @brief   Start Fifo
 * @param[in]   'fifo' fifoType enumeration
 * @param[in]
 * @retval
 **************************************************************************************/
void startFifo(ICM20948_fifoType fifo)
{
    fifoType = fifo;
    writeRegister8 (Bank_0, ICM20948_FIFO_EN_2, fifoType);
}

/**************************************************************************************
 * @brief  Stop  Fifo
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void stopFifo(void)
{
    writeRegister8 (Bank_0, ICM20948_FIFO_EN_2, 0);
}

/**************************************************************************************
 * @brief   Reset Fifo
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void resetFifo(void)
{
    writeRegister8 (Bank_0, ICM20948_FIFO_RST, 0x01);
    writeRegister8 (Bank_0, ICM20948_FIFO_RST, 0x00);
}

/**************************************************************************************
 * @brief  Get Fifo count
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
int16_t getFifoCount(void)
{
    int16_t regVal16 = (int16_t) readRegister16 (0, ICM20948_FIFO_COUNT);
    return regVal16;
}

/**************************************************************************************
 * @brief  Get Number Of Fifo Data Sets
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
int16_t getNumberOfFifoDataSets(void)
{
    int16_t numberOfSets = getFifoCount ();

    if ((fifoType == ICM20948_FIFO_ACC) || (fifoType == ICM20948_FIFO_GYR))
    {
        numberOfSets /= 6;
    }
    else if (fifoType == ICM20948_FIFO_ACC_GYR)
    {
        numberOfSets /= 12;
    }

    return numberOfSets;
}
/**************************************************************************************
 * @brief  Find Fifo Begin
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void findFifoBegin(void)
{
    int16_t count = getFifoCount ();
    int16_t start = 0;

    if ((fifoType == ICM20948_FIFO_ACC) || (fifoType == ICM20948_FIFO_GYR))
    {
        start = count % 6;
        for (int i = 0; i < start; i++)
        {
            readRegister8 (0, ICM20948_FIFO_R_W);
        }
    }
    else if (fifoType == ICM20948_FIFO_ACC_GYR)
    {
        start = count % 12;
        for (int i = 0; i < start; i++)
        {
            readRegister8 (0, ICM20948_FIFO_R_W);
        }
    }
}

/************** Magnetometer **************/
/**************************************************************************************
 * @brief  Initialize Magnetometer
 * @param[in]
 * @param[in]
 * @retval  True
 **************************************************************************************/
bool initMagnetometer(void)
{
    enableI2CMaster ();
    resetMag ();
    reset_ICM20948 ();
    sleep (false);
    writeRegister8 (2, ICM20948_ODR_ALIGN_EN, 1); // aligns ODR
    enableI2CMaster ();

    if (!(whoAmIMag () == AK09916_WHO_AM_I))
    {
        return false;
    }
    setMagOpMode (AK09916_CONT_MODE_100HZ);
    return true;
}

/**************************************************************************************
 * @brief  whoAmIMag : Read Magnotometer Identification
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
int16_t whoAmIMag(void)
{
    return readAK09916Register16 (AK09916_WIA_1);
}

/**************************************************************************************
 * @brief Set Magnotometer Operation Mode
 * @param[in] opMode  Operation mode enumeration
 * @param[in]
 * @retval
 **************************************************************************************/
void setMagOpMode(AK09916_opMode opMode)
{
    writeAK09916Register8 (AK09916_CNTL_2, opMode);
    vTaskDelay (10);
    if (opMode != AK09916_PWR_DOWN)
    {
        enableMagDataRead (AK09916_HXL, 0x08);
    }
}

/**************************************************************************************
 * @brief Reset Magnotometer
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void resetMag(void)
{
    writeAK09916Register8 (AK09916_CNTL_3, 0x01);
    vTaskDelay (100);
}

/************************************************
 Private Functions
 *************************************************/
/**************************************************************************************
 * @brief Set Clock To Auto Select
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void setClockToAutoSelect(void)
{
    regVal = readRegister8 (0, ICM20948_PWR_MGMT_1);
    regVal |= 0x01;
    writeRegister8 (0, ICM20948_PWR_MGMT_1, regVal);
    vTaskDelay (10);
}

/**************************************************************************************
 * @brief Correct Acc Raw Values
 * @param[in] a pointer to axis structure
 * @param[in]
 * @retval
 **************************************************************************************/

void correctAccRawValues(xyzFloat *a)
{
    a->x = (a->x - (accOffsetVal.x / accRangeFactor)) / accCorrFactor.x;
    a->y = (a->y - (accOffsetVal.y / accRangeFactor)) / accCorrFactor.y;
    a->z = (a->z - (accOffsetVal.z / accRangeFactor)) / accCorrFactor.z;
}

/**************************************************************************************
 * @brief Correct Accelerometer Raw Values
 * @param[in] a pointer to axis structure
 * @param[in]
 * @retval
 **************************************************************************************/
void correctGyrRawValues(xyzFloat *a)
{
    a->x -= (gyrOffsetVal.x / gyrRangeFactor);
    a->y -= (gyrOffsetVal.y / gyrRangeFactor);
    a->z -= (gyrOffsetVal.z / gyrRangeFactor);
}

/**************************************************************************************
 * @brief Switch Bank
 * @param[in] newBank
 * @param[in]
 * @retval
 **************************************************************************************/
void switchBank(uint8_t newBank)
{
    if (newBank != currentBank)
    {
        currentBank = newBank;
        currentBank = (uint8_t) (currentBank << 4);
        RmCom_I2C_w (ICM20948_REG_BANK_SEL, &currentBank, (uint8_t) 1);
    }
}

/**************************************************************************************
 * @brief Write Register 8
 * @param[in] bank
 * @param[in] reg
 * @param[in] val
 * @retval    Ret  FSP_SUCCESS or Error code
 **************************************************************************************/
fsp_err_t writeRegister8(uint8_t bank, uint8_t reg, uint8_t val)
{
    switchBank (bank);
    fsp_err_t Ret = 0;
    Ret = RmCom_I2C_w (reg, &val, (uint8_t) 1);
    return Ret;
}

/**************************************************************************************
 * @brief Write Register 16
 * @param[in] bank
 * @param[in] reg
 * @param[in] val
 * @retval    Ret FSP_SUCCESS or Error code
 **************************************************************************************/
fsp_err_t writeRegister16(uint8_t bank, uint8_t reg, uint16_t val)
{
    fsp_err_t Ret = 0;
    switchBank (bank);
    uint8_t MSByte = (uint8_t) ((val >> 8) & 0xFF);
    uint8_t LSByte = (uint8_t) (val & 0xFF);
    uint8_t WriteByte[2];
    WriteByte[0] = (uint8_t) MSByte;
    WriteByte[1] = LSByte;
    Ret = RmCom_I2C_w (reg, WriteByte, (uint8_t) 2);
    return Ret;
}

/**************************************************************************************
 * @brief Read Register 8
 * @param[in] bank
 * @param[in] reg
 * @param[in]
 * @retval   regValue regeister value
 **************************************************************************************/
uint8_t readRegister8(uint8_t bank, uint8_t reg)
{
    switchBank (bank);
    uint8_t regValue = 0;
    RmCom_I2C_r (reg, &regValue, (uint8_t) 1);
    return regValue;
}

/**************************************************************************************
 * @brief Read Register 16
 * @param[in] bank
 * @param[in] reg
 * @param[in]
 * @retval   reg16Val regeister value
 **************************************************************************************/
int16_t readRegister16(uint8_t bank, uint8_t reg)
{
    switchBank (bank);
    uint8_t MSByte = 0, LSByte = 0, regValue[2];
    int16_t reg16Val = 0;
    RmCom_I2C_r (reg, regValue, 2);
    MSByte = regValue[0];
    LSByte = regValue[1];
    reg16Val = (uint8_t) (((uint16_t) (MSByte << 8)) + LSByte);
    return reg16Val;
}

/**************************************************************************************
 * @brief Read All data
 * @param[in] data
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void readAllData(uint8_t *data)
{
    switchBank (0);
    RmCom_I2C_r (ICM20948_ACCEL_OUT, data, 20);
}

/**************************************************************************************
 * @brief Read ICM20948 xyz Values From Fifo
 * @param[in] a pointer to axis structure
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void readICM20948xyzValFromFifo(xyzFloat *a)
{
    uint8_t MSByte = 0, LSByte = 0;
    //xyzFloat xyzResult = {0.0, 0.0, 0.0};
    MSByte = readRegister8 (0, ICM20948_FIFO_R_W);
    LSByte = readRegister8 (0, ICM20948_FIFO_R_W);
    a->x = ((int16_t) ((MSByte << 8) + LSByte)) * 1.0;
    MSByte = readRegister8 (0, ICM20948_FIFO_R_W);
    LSByte = readRegister8 (0, ICM20948_FIFO_R_W);
    a->y = ((int16_t) ((MSByte << 8) + LSByte)) * 1.0;
    MSByte = readRegister8 (0, ICM20948_FIFO_R_W);
    LSByte = readRegister8 (0, ICM20948_FIFO_R_W);
    a->z = ((int16_t) ((MSByte << 8) + LSByte)) * 1.0;
}

/**************************************************************************************
 * @brief Read ICM20948 xyz axis Values From Fifo
 * @param[in] a pointer to axis structure
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void writeAK09916Register8(uint8_t reg, uint8_t val)
{
    writeRegister8 (3, ICM20948_I2C_SLV0_ADDR, AK09916_ADDRESS); // write AK09916
    writeRegister8 (3, ICM20948_I2C_SLV0_REG, reg); // define AK09916 register to be written to
    writeRegister8 (3, ICM20948_I2C_SLV0_DO, val);
}

/**************************************************************************************
 * @brief Read AK09916 Register 8
 * @param[in]reg
 * @param[in]
 * @param[in]
 * @retval  regVal
 **************************************************************************************/
uint8_t readAK09916Register8(uint8_t reg)
{
    enableMagDataRead (reg, 0x01);
    enableMagDataRead (AK09916_HXL, 0x08);
    regVal = readRegister8 (0, ICM20948_EXT_SLV_SENS_DATA_00);
    return regVal;
}

/**************************************************************************************
 * @brief Read AK09916 Register 16
 * @param[in]reg
 * @param[in]
 * @param[in]
 * @retval   regValue
 **************************************************************************************/
int16_t readAK09916Register16(uint8_t reg)
{
    int16_t regValue = 0;
    enableMagDataRead (reg, 0x02);
    regValue = readRegister16 (0, ICM20948_EXT_SLV_SENS_DATA_00);
    enableMagDataRead (AK09916_HXL, 0x08);
    return regValue;
}

/**************************************************************************************
 * @brief Read ICM20948
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval True if Write Reg Successfully Or False
 **************************************************************************************/
uint8_t reset_ICM20948(void)
{
    uint8_t ack = writeRegister8 (0, ICM20948_PWR_MGMT_1, ICM20948_RESET);
    /* wait for registers to reset */
    vTaskDelay (50);
    return (ack == 0);
}

/**************************************************************************************
 * @brief Enable I2C Master
 * @param[in]
 * @param[in]
 * @param[in]
 * @retval
 **************************************************************************************/
void enableI2CMaster(void)
{
    /* enable I2C master */
    writeRegister8 (0, ICM20948_USER_CTRL, ICM20948_I2C_MST_EN);
    /* set I2C clock to 345.60 kHz */
    writeRegister8 (3, ICM20948_I2C_MST_CTRL, 0x07);
    vTaskDelay (10);
}

/**************************************************************************************
 * @brief Enable Magnetometer Data Read
 * @param[in] reg
 * @param[in] bytes
 * @param[in]
 * @retval
 **************************************************************************************/
void enableMagDataRead(uint8_t reg, uint8_t bytes)
{
    /* read AK09916 */
    writeRegister8 (3, ICM20948_I2C_SLV0_ADDR, AK09916_ADDRESS | AK09916_READ);
    /* define AK09916 register to be read */
    writeRegister8 (3, ICM20948_I2C_SLV0_REG, reg);
    /* enable read | number of byte */
    writeRegister8 (3, ICM20948_I2C_SLV0_CTRL, 0x80 | bytes);
    vTaskDelay (10);
}

