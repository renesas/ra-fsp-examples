/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates some of the capabilities of the EK and FSP by using multiple peripherals.
        The project initializes the USB, ADC, SDRAM, OSPI, CEU, MIPI and GPIO modules to create an interactive user demo.

        Once programmed the user can control the board by button presses on the board and see regularly updated board
        configuration on the kit information screen.
        On the attached LCD pannel the user can interact with the application, please read the following document
        d018944_11_s01_r20qs0065eg0100 Evaluation Kit for RA8D1 Microcontroller Group EK-RA8D1 Quick Start Guide

2. Software Requirements:
        Renesas Flexible Software Package (FSP): Version 5.9.0
        e2 studio: Version 2025-04
        GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
        Terminal Console Application: Tera Term or a similar application
        USB Serial Drivers (included in Windows 10) 
        Windows® 10 operating system

3. Hardware settings for the project:
        Please configure the Jumper settings as described in the Quick Start Guide.
        Link to access Quick Start Guide -  https://www.renesas.com/us/en/document/qsg/ek-ra8d1-quick-start-guide

        Configuration switch (SW1) settings (Connectivity DEMO)
            SW1-1 SW1-2 SW1-3 SW1-4 SW1-5 SW1-6 SW1-7 SW1-8
             OFF   OFF   OFF   OFF   ON    OFF   ON    OFF

        Configuration switch (SW1) settings (AI Camera DEMO)
            SW1-1 SW1-2 SW1-3 SW1-4 SW1-5 SW1-6 SW1-7 SW1-8
             OFF   OFF   ON    OFF   OFF   OFF   ON    OFF

4. Configuration Settings:
        Please explore the configuration settings of each module in the RA configurator.

5. Download options in e2studio
        The Graphics data is loaded into OSPI during project download, this can be disabled after the first download.
		To disable, in e2studio open desired configuration 'Debug Configurations -> Startup'
		In the 'Load images and symbols' dialog disable 'graphics.srec'




