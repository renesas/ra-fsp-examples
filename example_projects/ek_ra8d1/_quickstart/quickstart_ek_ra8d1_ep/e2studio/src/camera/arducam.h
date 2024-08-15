/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : arducam.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#ifndef __ARDUCAM_H
#define __ARDUCAM_H
#include <stdbool.h>
#include "hal_data.h"
#include "r_typedefs.h"

// #include "delay.h"

#define OV2640_CHIPID_HIGH    (0x0A)
#define OV2640_CHIPID_LOW     (0x0B)

#define OV5642_CHIPID_HIGH    (0x300a)
#define OV5642_CHIPID_LOW     (0x300b)

#define OV5640_CHIPID_HIGH    (0x300a)
#define OV5640_CHIPID_LOW     (0x300b)

extern byte     sensor_model;
extern byte     sensor_addr;
extern uint32_t length;
extern uint8_t  is_header;
#define BMP                      (0)
#define JPEG                     (1)

#define OV7670                   (0)
#define MT9D111_A                (1)
#define OV7675                   (2)
#define OV5642                   (3)
#define OV3640                   (4)
#define OV2640                   (5)
#define OV9655                   (6)
#define MT9M112                  (7)
#define OV7725                   (8)
#define OV7660                   (9)
#define MT9M001                  (10)
#define OV5640                   (11)
#define MT9D111_B                (12)
#define OV9650                   (13)
#define MT9V111                  (14)
#define MT9T112                  (15)
#define MT9D112                  (16)

#define OV2640_160x120           (0)   /*160x120 */
#define OV2640_176x144           (1)   /*176x144 */
#define OV2640_320x240           (2)   /*320x240 */
#define OV2640_352x288           (3)   /*352x288 */
#define OV2640_640x480           (4)   /*640x480 */
#define OV2640_800x600           (5)   /*800x600 */
#define OV2640_1024x768          (6)   /*1024x768 */
#define OV2640_1280x1024         (7)   /*1280x1024 */
#define OV2640_1600x1200         (8)   /*1600x1200 */

#define OV5642_320x240           (0)   /*320x240 */
#define OV5642_640x480           (1)   /*640x480 */
#define OV5642_1024x768          (2)   /*1024x768 */
#define OV5642_1280x960          (3)   /*1280x960 */
#define OV5642_1600x1200         (4)   /*1600x1200 */
#define OV5642_2048x1536         (5)   /*2048x1536 */
#define OV5642_2592x1944         (6)   /*2592x1944 */

#define OV5640_320x240           (0)   /*320x240 */
#define OV5640_352x288           (1)   /*352x288 */
#define OV5640_640x480           (2)   /*640x480 */
#define OV5640_800x480           (3)   /*800x480 */
#define OV5640_1024x768          (4)   /*1024x768 */
#define OV5640_1280x960          (5)   /*1280x960 */
#define OV5640_1600x1200         (6)   /*1600x1200 */
#define OV5640_2048x1536         (7)   /*2048x1536 */
#define OV5640_2592x1944         (8)   /*2592x1944 */

/****************************************************/
/* I2C Control Definition*/
/****************************************************/
#define I2C_ADDR_8BIT            (0)
#define I2C_ADDR_16BIT           (1)
#define I2C_REG_8BIT             (0)
#define I2C_REG_16BIT            (1)
#define I2C_DAT_8BIT             (0)
#define I2C_DAT_16BIT            (1)

/* Register initialization tables for SENSORs */
/* Terminating list entry for reg */
#define SENSOR_REG_TERM_8BIT     (0xFF)
#define SENSOR_REG_TERM_16BIT    (0xFFFF)

/* Terminating list entry for val */
#define SENSOR_VAL_TERM_8BIT     (0xFF)
#define SENSOR_VAL_TERM_16BIT    (0xFFFF)

// Define maximum frame buffer size
#if (defined OV2640_MINI_2MP)
 #define MAX_FIFO_SIZE           (0x5FFFF)  // 384KByte
#elif (defined OV5642_MINI_5MP || defined OV5642_MINI_5MP_BIT_ROTATION_FIXED || defined ARDUCAM_SHIELD_REVC)
 #define MAX_FIFO_SIZE           (0x7FFFF)  // 512KByte
#else
 #define MAX_FIFO_SIZE           (0x7FFFFF) /*8MByte */
#endif

/****************************************************/
/* ArduChip registers definition*/
/****************************************************/
#define RWBIT                  (0x80)  /*READ AND WRITE BIT IS BIT[7] */

#define ARDUCHIP_TEST1         (0x00)  /*TEST register */

#if !(defined OV2640_MINI_2MP)
 #define ARDUCHIP_FRAMES       (0x01)  /*FRAME control register, Bit[2:0] = Number of frames to be captured */
                                       /*On 5MP_Plus platforms bit[2:0] = 7 means continuous capture until frame buffer is full */
#endif

#define ARDUCHIP_MODE          (0x02)  /*Mode register */
#define MCU2LCD_MODE           (0x00)
#define CAM2LCD_MODE           (0x01)
#define LCD2MCU_MODE           (0x02)

#define ARDUCHIP_TIM           (0x03)  /*Timming control */
#if !(defined OV2640_MINI_2MP)
 #define HREF_LEVEL_MASK       (0x01)  /*0 = High active , 1 = Low active */
 #define VSYNC_LEVEL_MASK      (0x02)  /*0 = High active , 1 = Low active */
 #define LCD_BKEN_MASK         (0x04)  /*0 = Enable, 1 = Disable */
 #if (defined ARDUCAM_SHIELD_V2)
  #define PCLK_REVERSE_MASK    (0x08)  // 0 = Normal PCLK, 1 = REVERSED PCLK
 #else
  #define PCLK_DELAY_MASK      (0x08)  /*0 = data no delay,1 = data delayed one PCLK */
 #endif

/*#define MODE_MASK                  0x10  //0 = LCD mode, 1 = FIFO mode */
#endif

/*#define FIFO_PWRDN_MASK               0x20  //0 = Normal operation, 1 = FIFO power down */
/*#define LOW_POWER_MODE              0x40      //0 = Normal mode, 1 = Low power mode */

#define ARDUCHIP_FIFO          (0x04)  /*FIFO and I2C control */
#define FIFO_CLEAR_MASK        (0x01)
#define FIFO_START_MASK        (0x02)
#define FIFO_RDPTR_RST_MASK    (0x10)
#define FIFO_WRPTR_RST_MASK    (0x20)

#define ARDUCHIP_GPIO          (0x06)  /*GPIO Write Register */
#if !(defined(ARDUCAM_SHIELD_V2) || defined(ARDUCAM_SHIELD_REVC))
 #define GPIO_RESET_MASK       (0x01)  /*0 = Sensor reset, 1 =  Sensor normal operation */
 #define GPIO_PWDN_MASK        (0x02)  /*0 = Sensor normal operation, 1 = Sensor standby */
 #define GPIO_PWREN_MASK       (0x04)  /*0 = Sensor LDO disable, 1 = sensor LDO enable */
#endif

#define BURST_FIFO_READ        (0x3C)  /*Burst FIFO read operation */
#define SINGLE_FIFO_READ       (0x3D)  /*Single FIFO read operation */

#define ARDUCHIP_REV           (0x40)  /*ArduCHIP revision */
#define VER_LOW_MASK           (0x3F)
#define VER_HIGH_MASK          (0xC0)

#define ARDUCHIP_TRIG          (0x41)  /*Trigger source */
#define VSYNC_MASK             (0x01)
#define SHUTTER_MASK           (0x02)
#define CAP_DONE_MASK          (0x08)

#define FIFO_SIZE1             (0x42)  /*Camera write FIFO size[7:0] for burst to read */
#define FIFO_SIZE2             (0x43)  /*Camera write FIFO size[15:8] */
#define FIFO_SIZE3             (0x44)  /*Camera write FIFO size[18:16] */

#ifndef _SENSOR_
 #define _SENSOR_
struct sensor_reg
{
    uint16_t reg;
    uint16_t val;
};
#endif

/*ArduCAM CS define */

// #define        CS_PORT        GPIOB
// #define        CS_PIN        GPIO_Pin_1

/*ArduCAM LED define */
#define        LED_PORT    GPIOB
#define        LED_PIN     GPIO_Pin_5

void ArduCAM_Init(byte model);
void ArduCAM_CS_init(void);
void ArduCAM_LED_init(void);

void OV2640_set_JPEG_size(uint8_t size);
void OV5642_set_JPEG_size(uint8_t size);
void OV5640_set_JPEG_size(uint8_t size);
void set_format(byte fmt);

void    flush_fifo(void);
void    start_capture(void);
void    clear_fifo_flag(void);
uint8_t read_fifo(void);

uint8_t read_reg(uint8_t addr);
void    write_reg(uint8_t addr, uint8_t data);

uint32_t read_fifo_length(void);
void     set_fifo_burst(void);

void    set_bit(uint8_t addr, uint8_t bit);
void    clear_bit(uint8_t addr, uint8_t bit);
uint8_t get_bit(uint8_t addr, uint8_t bit);
void    set_mode(uint8_t mode);

uint8_t bus_write(int address, int value);
uint8_t bus_read(int address);
uint8_t read_fifo_burst(void);

byte wrSensorReg8_8(int regID, int regDat);
int  wrSensorRegs8_8(const struct sensor_reg *);
byte rdSensorReg8_8(uint8_t regID, uint8_t * regDat);

byte wrSensorReg16_8(int regID, int regDat);
int  wrSensorRegs16_8(const struct sensor_reg reglist[]);
byte rdSensorReg16_8(uint16_t regID, uint8_t * regDat);

#endif
