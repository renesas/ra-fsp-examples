/***********************************************************************************************************************
 * File Name    : ov7670_registers.h
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef OV7670_REGISTERS_H_
#define OV7670_REGISTERS_H_

/* Register addresses */
#define GAIN               0x00u
#define BLUE               0x01u
#define RED                0x02u
#define VREF               0x03u
#define COM1               0x04u
#define BAVE               0x05u
#define GbAVE              0x06u
#define AECHH              0x07u
#define RAVE               0x08u
#define COM2               0x09u
#define COM3               0x0Cu
#define COM4               0x0Du
#define COM5               0x0Eu
#define COM6               0x0Fu
#define AECH               0x10u
#define CLKRC              0x11u
#define COM7               0x12u
#define COM8               0x13u
#define COM9               0x14u
#define COM10              0x15u
#define HSTART             0x17u
#define HSTOP              0x18u
#define VSTRT              0x19u
#define VSTOP              0x1Au
#define PSHFT              0x1Bu
#define MVFP               0x1Eu
#define LAEC               0x1Fu
#define ADCCTR0            0x20u
#define ADCCTR1            0x21u
#define ADCCTR2            0x22u
#define ADCCTR3            0x23u
#define AEW                0x24u
#define AEB                0x25u
#define VPT                0x26u
#define BBIAS              0x27u
#define GbBIAS             0x28u
#define EXHCH              0x2Au
#define EXHCL              0x2Bu
#define RBIAS              0x2Cu
#define ADVFL              0x2Du
#define ADVFH              0x2Eu
#define YAVE               0x2Fu
#define HSYST              0x30u
#define HSYEN              0x31u
#define HREF               0x32u
#define CHLF               0x33u
#define ARBLM              0x34u
#define ADC                0x37u
#define ACOM               0x38u
#define OFON               0x39u
#define TSLB               0x3Au
#define COM11              0x3Bu
#define COM12              0x3Cu
#define COM13              0x3Du
#define COM14              0x3Eu
#define EDGE               0x3Fu
#define COM15              0x40u
#define COM16              0x41u
#define COM17              0x42u
#define AWBC1              0x43u
#define AWBC2              0x44u
#define AWBC3              0x45u
#define AWBC4              0x46u
#define AWBC5              0x47u
#define AWBC6              0x48u
#define REG4B              0x4Bu
#define DNSTH              0x4Cu
#define MTX1               0x4Fu
#define MTX2               0x50u
#define MTX3               0x51u
#define MTX4               0x52u
#define MTX5               0x53u
#define MTX6               0x54u
#define BRIGHT             0x55u
#define CONTRAS            0x56u
#define CONTRASCENTER      0x57u
#define MTXS               0x58u
#define LCC1               0x62u
#define LCC2               0x63u
#define LCC3               0x64u
#define LCC4               0x65u
#define LCC5               0x66u
#define MANU               0x67u
#define MANV               0x68u
#define GFIX               0x69u
#define GGAIN              0x6Au
#define DBLV               0x6Bu
#define AWBCTR3            0x6Cu
#define AWBCTR2            0x6Du
#define AWBCTR1            0x6Eu
#define AWBCTR0            0x6Fu
#define SCALING_XSC        0x70u
#define SCALING_YSC        0x71u
#define SCALING_DCWCTR     0x72u
#define SCALING_PCLK_DIV   0x73u
#define REG74              0x74u
#define REG75              0x75u
#define REG76              0x76u
#define REG77              0x77u
#define SLOP               0x7Au
#define GAM1               0x7Bu
#define GAM2               0x7Cu
#define GAM3               0x7Du
#define GAM4               0x7Eu
#define GAM5               0x7Fu
#define GAM6               0x80u
#define GAM7               0x81u
#define GAM8               0x82u
#define GAM9               0x83u
#define GAM10              0x84u
#define GAM11              0x85u
#define GAM12              0x86u
#define GAM13              0x87u
#define GAM14              0x88u
#define GAM15              0x89u
#define RGB444             0x8Cu
#define DM_LNL             0x92u
#define DM_LNH             0x93u
#define LCC6               0x94u
#define LCC7               0x95u
#define BD50ST             0x9Du
#define BD60ST             0x9Eu
#define HAECC1             0x9Fu
#define HAECC2             0xA0u
#define SCALING_PCLK_DELAY 0xA2u
#define NT_CTRL            0xA4u
#define BD50MAX            0xA5u
#define HAECC3             0xA6u
#define HAECC4             0xA7u
#define HAECC5             0xA8u
#define HAECC6             0xA9u
#define HAECC7             0xAAu
#define BD60MAX            0xABu
#define STR_OPT            0xACu
#define STR_R              0xADu
#define STR_G              0xAEu
#define STR_B              0xAFu
#define ABLC1              0xB1u
#define THL_ST             0xB3u
#define THL_DLT            0xB5u
#define AD_CHB             0xBEu
#define AD_CHR             0xBFu
#define AD_CHGb            0xC0u
#define AD_CHGr            0xC1u
#define SATCTR             0xC9u
/* End Register addresses */

#define END_OF_TABLE 0xFFu

/* Register BITS */

/* COM7 */
#define COM7_RESET (1u << 7)
#define COM7_RGB   ((1u << 2) | (0u << 0))

/* COM8 */
#define  COM8_FASTAEC_EN   (1u << 7)
#define  COM8_AECSTEP_SIZE (1u << 6)
#define  COM8_BFILT_EN     (1u << 5)
#define  COM8_AGC_EN       (1u << 2)
#define  COM8_AWB_EN       (1u << 1)
#define  COM8_AEC_EN       (1u << 0)

/* COM11 */
#define  COM11_EXP_TIMING (1u << 1)
#define  COM11_D56_AUTO   (1u << 4)

/* COM15 */
#define COM15_RGB565 (1u << 4)

/* COM 16 */
#define   COM16_AWBGAIN_EN (1u << 3)


#endif /* OV7670_REGISTERS_H_ */
