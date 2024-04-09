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
    The example project demonstrates the typical use of the LVD HAL module APIs.
    This project enables LVD0 circuit to reset MCU and LVD1 to generate maskable interrupt.

    LVD0 resets MCU only when input voltage drops below LVD0 threshold value powering off the MCU. To detect this reset,
    user has to increase back the input voltage value above LVD0 threshold value. By doing this, board gets powered up
    and LED will turn ON indicating LVD0 reset detection.
    Ex: 2.8V is LVD0 threshold value, when input Voltage drops from 2.9V to 2.7V, LVD0 resets MCU.
    User has to rise input voltage above 2.8V again to detect LVD0 reset.

    LVD1 generates a maskable interrupt only when input voltage rises above LVD1 threshold value.
    Ex: 2.9V is LVD1 threshold value, when input voltage is raised from 2.8V to 3.0V,
    LVD1 generates the maskable interrupt.

    Please note No interrupt will be generated when Voltage is already above the threshold value and increased further.
    Ex: When input voltage is raised from 3.0V to 3.1V with 2.9V as threshold voltage, no interrupt gets generated.

    LVD0 is configured in OFS1 register via the properties dialog of BSP tab of FSP configurator.

    Example project status and any failures will be displayed on JLinkRTTViewer.

2. Hardware and Software Requirements:
    External Hardware : 
        Variable DC Power supply.
        External JLink Segger with 9 pin (19 pin for MCK-RA8T1, FPB-RA0E1) connector and USB Type B to connect JLink with PC.
        External Capacitor if power supply generates spikes.

3. Hardware settings for the project:
    Wiring details as below:
    EK-RA2A1:
    1) DC Power supply +ve pin ----> Any on board VCC pin
    2) DC Power supply -ve pin ----> Any on board VSS(GND) pin
    3) External JLink connector ---> J10 JTAG point (Near Debug USB)

    For EK-RA2A1, LVD1 generates an interrupt when input Voltage raises above 2.9V.
    LVD0 resets the MCU when input Voltage drops below 2.51V.

    EK-RA4M1:
    1) DC Power supply +ve pin ----> Any on board VCC pin
    2) DC Power supply -ve pin ----> Any on board VSS(GND) pin
    3) External JLink connector ---> J10 JTAG point (Near Debug USB)

    For EK-RA4M1, LVD1 generates an interrupt when input Voltage raises above 2.9V.
    LVD0 resets the MCU when input Voltage drops below 2.51V.

    EK-RA6M1:
    1) DC Power supply +ve pin ----> Any on board VCC pin
    2) DC Power supply -ve pin ----> Any on board VSS(GND) pin
    3) External JLink connector ---> J10 JTAG point (Near Debug USB)

    For EK-RA6M1, LVD1 generates an interrupt when input Voltage raises above 2.99V.
    LVD0 resets the MCU when input Voltage drops below 2.8V.

    EK-RA6M2:
    1) DC Power supply +ve pin ----> Any on board VCC pin
    2) DC Power supply -ve pin ----> Any on board VSS(GND) pin
    3) External JLink connector ---> J10 JTAG point (Near Debug USB)

    For EK-RA6M2, LVD1 generates an interrupt when input Voltage raises above 2.99V.
    LVD0 resets the MCU when input Voltage drops below 2.8V.

    EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA4M3 and EK-RA6M5:
    1) DC Power supply +ve pin ----> Any on board +3.3V pin
    2) DC Power supply -ve pin ----> Any on board GND pin
    3) External JLink connector ---> J13 JTAG point (Near Debug USB)

    For EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA4M3 and EK-RA6M5 boards, LVD1 generates an interrupt when input Voltage raises above 2.99V.
    LVD0 resets the MCU when input Voltage drops below 2.8V.

    EK-RA2E2, EK-RA2A2
    1) DC Power supply +ve pin ----> Any on board VCC pin
    2) DC Power supply -ve pin ----> Any on board VSS(GND) pin
    3) External JLink connector ---> J13 JTAG point (Near Debug USB)

    For EK-RA2E2, EK-RA2A2 LVD1 generates an interrupt when input Voltage raises above 2.9V.
    LVD0 resets the MCU when input Voltage drops below 2.51V.

    EK-RA8M1, EK-RA8D1:
    1) DC Power supply +ve pin ----> Any on board VCC pin
    2) DC Power supply -ve pin ----> Any on board VSS(GND) pin
    3) External JLink connector ---> J13 JTAG point (Near Debug USB)

    For EK-RA8M1, EK-RA8D1 LVD1 generates an interrupt when input Voltage raises above 3.08V.
    LVD0 resets the MCU when input Voltage drops below 2.85V.

    FPB-RA2E3:
    1) DC Power supply +ve pin ----> Any on board VCC pin
    2) DC Power supply -ve pin ----> Any on board VSS(GND) pin
    3) External JLink connector ---> CN4 JTAG point (Near Debug USB)

    For FPB-RA2E3 board LVD1 generates an interrupt when input Voltage raises above 2.9V.
    LVD0 resets the MCU when input Voltage drops below 2.51V.

    MCK-RA8T1:
    1) DC Power supply +ve pin ----> Any on board VCC pin
    2) DC Power supply -ve pin ----> Any on board VSS(GND) pin
    3) External JLink connector ---> CN9 JTAG point (Near Debug USB)

    For MCK-RA8T1, LVD1 generates an interrupt when input Voltage raises above 3.08V.
    LVD0 resets the MCU when input Voltage drops below 2.85V.

    FPB-RA0E1:
    1) DC Power supply +ve pin ----> Any on board VCC pin
    2) DC Power supply -ve pin ----> Any on board VSS(GND) pin
    3) External JLink connector ---> CN4 JTAG point (Near Debug USB)

    For FPB-RA0E1, LVD1 generates an interrupt when input Voltage raises above 2.91V.
    LVD0 resets the MCU when input Voltage drops below 2.62V.

Note:
1) To run LVD Example project, use external debugger for flashing the code, disconnect debugging mode, push on board 
   reset button and open JLinkRTTViewer to see the output by changing input voltage values.

2) For EK-RA2E2, FPB-RA2E3, EK-RA2A2 boards, use external debugger for flashing the code, disconnect debugging mode,
   push on board reset button. User need to decrease the input Voltage belows 1.7V (Power-on Reset Voltage - VPOR) then raises the input 
   Voltage to 3.3V and open JLinkRTTViewer to see the output by changing input voltage values.

3) For EK-RA8M1, EK-RA8D1, MCK-RA8T1 after change input voltage values from below low threshold (ex: 2.8V) to above high threshold (ex: 3.3V),
   re-connect RTT viewer to show log. 

4) Use external capacitors on PMODA or PMODB (pins 5, 6 or 11, 12) if input power supply is not accurate and 
   create unwanted spikes in voltage.

5) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x2000484c
   b. Keil: 	Not Available 
   c. IAR: 	Not Available
 
6) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).