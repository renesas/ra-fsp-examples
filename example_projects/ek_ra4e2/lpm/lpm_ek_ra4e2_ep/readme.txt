/***********************************************************************************************************************
* Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
The project also illustrates the methods to reduce MCU power consumption and restores the pre-LPM states of peripheral modules.  
EP will perform different low power mode transitions based on the user selection of Low Power Mode, Request and 
Cancel Sources in the RA Configurator.

2. Hardware and Software Requirements:
   Software:
		e2studio
		JLink V6.52b or later
   Hardware:
		Renesas RA kits: EK-RA2A1, EK-RA2L1, EK-RA2E1, EK-RA2E2, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5,EK-RA4E2, EK-RA6E2, RSSK-RA6T1,EK-RA4W1,FPB-RA6E1,FPB-RA4E1,MCK-RA6T2
		Digital Multi-timeter to measurement MCU's current (Icc)
		One jumper wire.
    
3. Hardware and software setups:

The pin configuration file for LPM mode in each example project named with a suffix "-LPM"; you can change these 
configurations by selecting the file and set the ports on the FSP's pin configurator.
Unused pins in LPM modes are set as input ports to reduce MCU's power consumption, check the "Handling of Unused Pins" 
section in MCU's user manual for more details.

Four or three different LPM module's instances named g_lpm_sleep, g_lpm_sw_standby, g_lpm_sw_standby_with_snooze, 
g_lpm_deep_sw_standby are used in each LPM example project with Trigger/End sources as follows:

----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
				     |EK-RA2A1  |EK-RA2L1  |EK-RA2E1  |EK-RA4M1  |EK-RA4M2  |EK-RA4M3  |EK-RA6M1  |EK-RA6M2  |EK-RA6M3  |EK-RA6M3G |EK-RA6M4  |EK-RA6M5  |RSSK-RA6T1 |EK-RA4W1  |FPB-RA6E1  |FPB-RA4E1	  |EK-RA2E2    |MCK-RA6T2    |EK-RA4E2    |EK-RA6E2
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Sleep		|Cancel/End source   |IRQ6      |IRQ3      |IRQ3      |IRQ0      |IRQ10-DS  |IRQ10-DS  |IRQ0-DS   |IRQ0-DS   |IRQ13-DS  |IRQ13-DS  |IRQ10-DS  |IRQ10-DS  |IRQ6-DS    |IRQ4	|IRQ1-DS    |IRQ1-DS 	  |IRQ1-DS     |IRQ12-DS     |IRQ7        |IRQ7
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SW Standby	|Cancel/End source   |IRQ6	|IRQ3      |IRQ3      |IRQ0-DS   |IRQ10-DS  |IRQ10-DS  |IRQ0-DS   |IRQ13-DS  |IRQ5-DS   |IRQ13-DS  |IRQ10-DS  |IRQ10-DS  |IRQ6-DS    |IRQ4	|IRQ1-DS    |IRQ1-DS	  |IRQ1-DS     |IRQ12-DS     |IRQ7        |IRQ7
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Snooze		|Request Source	     |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI  |AGT1_AGTI |AGT1_AGTI  |AGT1_AGTI 	  |AGT1_AGTI   |AGT1_AGTI    |AGT1_AGTI   |AGT1_AGTI
		|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		|Cancel/End source   |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI |AGT1_AGTI  |AGT1_AGTI	|AGT1_AGTI  |AGT1_AGTI	  |AGT1_AGTI   |AGT1_AGTI    |AGT1_AGTI   |AGT1_AGTI
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Deep SW Stansby	|Cancel/End source|					         |IRQ10-DS  |IRQ10-DS  |IRQ0-DS   |IRQ0-DS   |IRQ13-DS  |IRQ13-DS  |IRQ10-DS  |IRQ10-DS  |IRQ6-DS       	|IRQ1-DS    |IRQ1-DS                   |IRQ12-DS     |IRQ7        |IRQ7
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	

Notes:
 If using RTOS, Systick timer must be stopped before entering the Sleep mode because any interrupt will cancel the sleep mode. 
 The timer must be re-started after waking up. 

Below are the hardware modifications required for measuring the MCU's current.
EK-RA2A1 kit, EK-RA4M1:
 1)Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
 
EK-RA6M1 kit:
 1)Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
 2)Connect a jumper wire from J2-Pin5(P415) to J4-Pin12(P206/IRQ0-DS). 
   Since the user push-button S1 used P415/IRQ8, it won't cancel Deep Software Standby mode.
   This step will enable P206/IRQ0-DS as the canceling source for all of LPM modes.

EK-RA6M2 kit:
 1)Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
 2)Connect a jumper wire from J1-Pin37(P105) to J1-Pin3(P206/IRQ0-DS). 
   Since the user push-button S1 used P105/IRQ0, it won't cancel Deep Software Standby mode.
   This step will enable P206/IRQ0-DS as the canceling source for all of LPM modes.

EK-RA4W1 kit:
 1))Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of SS20.

   
To measure MCU's current (Icc) on EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2 kits directly, 
using a Digital Multi-meter connecting between pin1 and pin3 of E6 and on EK-RA4W1 measure
current between pin1 and pin2 of J7; ; this is the recommended method.

You can also measure and calculate the Icc by capturing the voltage drops across a resistor from 
5ohm to 20ohm connected between pin1 and pin3 of E6.

EK-RA2L1, EK-RA2E1, EK-RA2E2, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5 kit :
 1)Remove R2.(Not applicable to EK-RA2L1, EK-RA2E1, EK-RA2E2)
 2)Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
 3)To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the kit connectors.

FPB-RA6E1,FPB-RA4E1 Kit:
 1)Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
 2)To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins (CN1) on the kit connectors.


RSSK-RA6T1:
Not applicable

MCK-RA6T2
Not applicable


Notes:
  To get more accurate MCU's current (Icc) on EK-RA6M1, you should cut traces between E10, E50 and E52 
  to isolate the Icc from the Analog power supply current AIcc.

4. Steps to run LPM Example Projects as follows:
   1) Download example project to RA kits using USB Debug port.
   2) Power-cycle the kits.
   3) Press the user push-button S1 to enter and cancel LPM modes. It will toggle LED1 before entering each LPM mode.
	  Transition sequences:
	  1. Normal → Sleep → Normal.
	  2. Normal → SW Standby → Normal.
	  3. Normal → SW Standby → Snooze → SW Standby → Normal. In SW Standby with Snooze enabled, MCU will switch between SW Standby and Snooze every 6 seconds.
	     Normal → SW Standby → Snooze → Normal.
	  4. Normal → SW Standby → Deep SW Standby → Normal. 	  