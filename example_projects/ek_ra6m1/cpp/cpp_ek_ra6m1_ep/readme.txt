/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates the typical use of the ADC HAL module APIs.
        The project initializes the ADC in single scan or continuous scan mode based on user selection in RA configurator.
        Once initialized,user can initiate the ADC scan and also stop the scan (in the case of continuous scan mode)
        using JLinkRTTViewer by sending commands. Result and ADC status is displayed on the JLinkRTTViewer.


2. Hardware settings for the project:
        Hardware connections:

        EK-RA2A1:
        1)J2 Connector Pin no:36(AVSS0)--------> Pin no: 34(VREFLO) with Jumper or wire
        Note: No Hardware connections required for other RA boards.

3. Configuration Settings:
        The example project, by default, runs in continuous scan mode on the boards, with the exception as mentioned in the *Note.
 	In order to switch the scan mode, change the 'Mode' field of ADC Driver in the RA configurator to Single Scan.

*Note: EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1 and EK-RA4M1 support only Single Scan mode.