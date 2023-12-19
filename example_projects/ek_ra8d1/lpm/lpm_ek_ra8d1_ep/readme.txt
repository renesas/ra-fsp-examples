/***********************************************************************************************************************
* Copyright [2020-2023] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
*
* This software is supplied by Renesas Electronics America Inc. and may only be used with products of Renesas Electronics Corp.
* and its affiliates (“Renesas”).  No other uses are authorized.  This software is protected under all applicable laws, 
* including copyright laws.
* Renesas reserves the right to change or discontinue this software.
* THE SOFTWARE IS DELIVERED TO YOU “AS IS,” AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT 
* PERMISSIBLE UNDER APPLICABLE LAW,DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES OF 
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE.  TO THE MAXIMUM 
* EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE (OR ANY PERSON 
* OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING, WITHOUT LIMITATION, 
* ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES;
* ANY LOST PROFITS, OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF 
* THE POSSIBILITY OF SUCH LOSS,DAMAGES, CLAIMS OR COSTS.
* **********************************************************************************************************************/
1. Project Overview:
	This project demonstrates the basic functionalities of the Low Power Mode module (LPM) on Renesas RA MCUs using Renesas FSP.
	The project also illustrates methods to reduce MCU power consumption and restore the pre-LPM states of peripheral modules.
	The EP will perform different low power mode transitions based on the user's selection of Low Power Mode, request, and cancel
	sources in the RA Configurator. The MCU will automatically enter each LPM mode and then wait for cancel sources to trigger 
	manually by an external interrupt or automatically by a timer to exit LPM mode. Turn off the blue LED and display the LPM Mode
	Name before entering each LPM mode. Turn on the blue LED and display the cancel source name after exiting each LPM mode.The EP 
	log will print to the Host PC via the UART interface at a baud rate of 115200 bps. The red LED will turn on if an error occurs.

2. Hardware Requirements:
	1 x Renesas RA board
	1 x Pmod USBUART (if the RA Board does not support J-Link VCOM functionality)
	1 x Micro USB cable
	RA Boards have to use Pmod USBUART: EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1,
	EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, FPB-RA4E1, FPB-RA6E1, MCK-RA6T2, RSSK-RA6T1.
	
3. Hardware and software setups:
	The pin configuration file for LPM mode in each example project is named with the suffix "LPM". User can change these configurations 
	by selecting the file and setting the ports on the FSP's pin configurator. Unused pins in LPM modes are set as input ports to 
	reduce the MCU's power consumption; check the "Handling of Unused Pins" section in the MCU's user manual for more details.

	Four or three different LPM module's instances named g_lpm_sleep, g_lpm_deep_sleep, g_lpm_sw_standby, g_lpm_sw_standby_with_snooze,
	g_lpm_deep_sw_standby is used in each LPM example project depending on the support of the MCU line.

	Notes:
		If using RTOS, Systick timer must be stopped before entering the Sleep mode because any interrupt will cancel the sleep mode. 
		The timer must be re-started after waking up. 
		
4. The hardware modifications required for measuring the MCU's current.
	EK-RA2A1 kit, EK-RA4M1:
	  1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
 
	EK-RA6M1 kit:
	  1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	  2) Connect a jumper wire from J2-Pin5(P415) to J4-Pin12(P206/IRQ0-DS). 
	     Since the user push-button S1 used P415/IRQ8, it won't cancel Deep Software Standby mode.
	     This step will enable P206/IRQ0-DS as the canceling source for all of LPM modes.

	EK-RA6M2 kit:
	  1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	  2) Connect a jumper wire from J1-Pin37(P105) to J1-Pin3(P206/IRQ0-DS). 
	     Since the user push-button S1 used P105/IRQ0, it won't cancel Deep Software Standby mode.
	     This step will enable P206/IRQ0-DS as the canceling source for all of LPM modes.

	EK-RA4W1 kit:
	  1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of SS20.
   
	     To measure MCU's current (Icc) on EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2 kits directly, 
	     using a Digital Multi-meter connecting between pin1 and pin3 of E6 and on EK-RA4W1 measure
	     current between pin1 and pin2 of J7; ; this is the recommended method.

	     You can also measure and calculate the Icc by capturing the voltage drops across a resistor from 
	     5 ohm to 20 ohm connected between pin1 and pin3 of E6.


	EK-RA2L1, EK-RA2E1, EK-RA2E2, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 kit:
	  1) Remove R2.(Not applicable to EK-RA2L1, EK-RA2E1, EK-RA2E2)
	  2) Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	  3) To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the 
	     kit connectors.

	FPB-RA6E1, FPB-RA4E1 Kit:
	  1) Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	  2) To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins (CN1) 
	     on the kit connectors.

	RSSK-RA6T1:
	  Not applicable

	MCK-RA6T2
	  Not applicable

	FPB-RA2E3
	  1) Connect a jumper wire from J3-Pin21(P200) to J3-Pin18(P206/IRQ0).
	  2) Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	  3) To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the 
	     kit connectors.

	Notes:
		To get more accurate MCU's current (Icc) on EK-RA6M1, you should cut traces between E10, E50 and E52 
		to isolate the Icc from the Analog power supply current AIcc.	

5. The steps to run LPM Example Projects are as follows:
	1) Download the example project to the RA board using the USB Debugport.
	2) Connect the PMOD USBUART module to the RA board via the PMOD2 port, and then connect the Pmod USBUART to the host PC via a micro-USB cable.
	   Skip if the RA board supports J-Link VCOM functionality.
	3) Open a terminal application on the host PC and connect to the COM Port provided by the J-Link onboard or Pmod USB UART.
	4) Power-cycle the kits.
	5) Press the user push-button S1 or wait 10 seconds to enter and cancel LPM mode. It will turn off blue LED before entering each LPM mode 
	   and turn on blue LED after exiting each LPM mode.
	6) The LPM mode name will display on the terminal application before entering each LPM mode, and the canceling source name will display 
	   on the terminal application after exiting each LPM mode.
	
	Transition sequences: 
	1. Normal → Sleep → Normal.									(Support all MCU lines)
	2. Normal → Deep Sleep → Normal.								(Support for RA8 MCU lines)
	3. Normal → SW Standby → Normal.								(Support all MCU lines)
	4. Normal → SW Standby → Snooze → SW Standby → Snooze → Normal.					(Support some MCU lines)
	5. Normal → Deep SW Standby → Normal.								(Support some MCU lines)
