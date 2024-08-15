/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        This example project demonstrates the typical use of the DC-DC Regulator while operating ADC module using the HAL API's provided by FSP.
        The project enables DC-DC regulator from BSP properties and calls R_BSP_PowerModeSet API at run time to change the MCU power 
        input from Linear Regulator (LDO) mode to Switching Regulator (DC-DC) mode with Voltage range selected as 2.7V - 3.6V.
        This example project can be easily modified to support varying the input voltage range at run time as well.
        This can be done by calling above mentioned API with required Voltage range as an input. The API will return the previous mode value.
        User can initiate the ADC scan using JLinkRTTViewer by sending commands. Result and ADC status is displayed on the JLinkRTTViewer.

2. Hardware settings for the project:
        Hardware modifications:

        RA2L1-EK:
        1) Short E15, E2 & E14 points near the RA2L1 MCU.
        Note: DC-DC regulator feature & the example project is only supported on EK-RA2L1 board.

        If user want to use any other input Voltage range or change the input voltage range at run time, then following external Hardwares are needed.
        1) External JLink Debugger.
        2) Input power supply.
            Note: The input power supply should provide min. of 2.4V and up to 5.5V. Please do not go beyond the mentioned 
            input Voltage range as it may damage the board.

        User can connect the External JLink Debugger to J13 slot and connect Jumper J9 to 1 - 2.
        To provide power input, User can connect input power supply on TP3 & GND.

3. Configuration Settings:
        User can navigate DCDC property via BSP -> Properties -> RA2L1 Family -> Power -> DC-DC Regulator & DC-DC Supply range.
        DC-DC Regulator property have following 3 options provided.
            a. If user selects "Enable at startup" option, DCDC regulator will be enabled by default at the time of BSP initialization by BSP with selected Voltage range.
            b. If user selects "Enable" option, user has to call BSP API(mentioned above) with input voltage range. This will switch the MCU power input from LDO mode to DC-DC mode
            c. If user selects "Disable" option, DC-DC mode will be disabled.
        DC-DC Supply Range property have following 4 options provided.
            a. 2.4V - 2.7V
            b. 2.7V - 3.6V
            c. 3.6V - 4.5V
            d. 4.5V - 5.5V

        Note:
        The example project by default uses enable DC-DC mode option with 2.7V - 3.6V range and on-board JLink debugger.
