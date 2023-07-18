/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define I_U (BSP_IO_PORT_00_PIN_00)
#define I_V (BSP_IO_PORT_00_PIN_01)
#define I_W (BSP_IO_PORT_00_PIN_02)
#define VOLT_BUS (BSP_IO_PORT_00_PIN_04)
#define POT_IN (BSP_IO_PORT_00_PIN_05)
#define HALL_V (BSP_IO_PORT_00_PIN_06)
#define HALL_U (BSP_IO_PORT_00_PIN_08)
#define VOLT_W (BSP_IO_PORT_00_PIN_13)
#define VOLT_V (BSP_IO_PORT_00_PIN_14)
#define HALL_W (BSP_IO_PORT_00_PIN_15)
#define ENC_B (BSP_IO_PORT_01_PIN_00)
#define ENC_A (BSP_IO_PORT_01_PIN_01)
#define V_LOWER (BSP_IO_PORT_01_PIN_02)
#define V_UPPER (BSP_IO_PORT_01_PIN_03)
#define OC_DETECT (BSP_IO_PORT_01_PIN_04)
#define ENC_Z (BSP_IO_PORT_01_PIN_05)
#define LED2 (BSP_IO_PORT_01_PIN_06)
#define W_UPPER (BSP_IO_PORT_01_PIN_11)
#define W_LOWER (BSP_IO_PORT_01_PIN_12)
#define LED1 (BSP_IO_PORT_01_PIN_13)
#define SW2 (BSP_IO_PORT_02_PIN_00)
#define SW1 (BSP_IO_PORT_03_PIN_04)
#define U_LOWER (BSP_IO_PORT_04_PIN_08)
#define U_UPPER (BSP_IO_PORT_04_PIN_09)
#define RMW_RX (BSP_IO_PORT_04_PIN_10)
#define RMW_TX (BSP_IO_PORT_04_PIN_11)
#define VOLT_U (BSP_IO_PORT_05_PIN_00)
extern const ioport_cfg_t g_bsp_pin_cfg; /* RA6T3 MCK */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER

#endif /* BSP_PIN_CFG_H_ */
