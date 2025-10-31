/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
        The example project demonstrates some of the capabilities of the EK and FSP by using multiple peripherals.
        The project initializes the QSPI, MIPI CSI, GLCD and GPIO modules along with various supporting stacks to create 
        an interactive user demo.

        There is a serial console which can be activated by connecting USB cable to DEBUG1 (J10), 
        connection settings are Speed 115200, Data 8-bit, Parity None, Stop 1-bit, Flow Control None.
        Once application is running the user can control the board by button presses on the board and see regularly updated 
        board configuration on the kit information screen using the serial console. 
        Attaching the LCD panel (supplied) a user can interact with the application using the screen.
        Attach OV5640 camera module in addition to the panel to allow 'Display' demo on application menu or option 4 on console
        to show live image, please read the following document for full description of the application:
        r20qs0051eg Evaluation Kit for RA8P1 Microcontroller Group EK-RA8P1 v1 Quick Start Guide.

2. Software Requirements:
        Renesas Flexible Software Package (FSP): Version 6.2.0
        e2 studio: Version 2025-10
        Terminal Console Application: Tera Term or a similar application
        USB Serial Drivers (included in Windows 10) 
        Windows® 10 operating system
        
3. Hardware settings for the project:
        Please configure the Jumper settings as described in the Quick Start Guide.
        Link to access Quick Start Guide - https://www.renesas.com/en/document/qsg/ek-ra8p1-v1-quick-start-guide

        Configuration switch (SW4) settings
            SW4-1 SW4-2 SW4-3 SW4-4 SW4-5 SW4-6 SW4-7 SW4-8
             OFF   OFF   OFF   OFF   OFF   OFF   OFF   OFF

4. Configuration Settings:
        Please explore the configuration settings of each module in the RA configurator.
