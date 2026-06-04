/***********************************************************************************************************************
* Copyright [2026] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
*
* This software is supplied by Renesas Electronics America Inc. and may only be used with products of Renesas 
* Electronics Corp. and its affiliates (“Renesas”). No other uses are authorized.  This software is protected under all
* applicable laws, including copyright laws.
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
        The example project demonstrates some of the capabilities of the EK and FSP by using multiple peripherals.
        The project initializes the QSPI , MIPI CSI , GLCD and GPIO modules along with various supporting stacks to create 
        an interactive user demo.

        There is a serial console which can be activated by connecting USB cable to DEBUG1 (J10), 
        connection settings are Speed 115200, Data 8-bit, Parity None, Stop 1-bit, Flow Control None.
        Once application is running the user can control the board by button presses on the board and see regularly updated 
        board configuration on the kit information screen using the serila console. 
        Attaching the LCD panel (supplied) a user can interact with the application using the screen.
        Attach OV5640 camera module in addition to the panel to allow 'Display' demo on application menu or option 4 on console
        to show live image, please read the following document for full description of the application:
        r20qs0077eg Evaluation Kit for RA8D2 Microcontroller Group EK-RA8D2 v1 Quick Start Guide.

2. Software Requirements:
        Renesas Flexible Software Package (FSP): Version 6.5.0 
        e2 studio: Version 2026-04.2
        Terminal Console Application: Tera Term or a similar application
        USB Serial Drivers (included in Windows 11) 
        Windows® 11 operating system
  
3. Hardware settings for the project:
        Configuration switch (SW4) settings
            SW4-1 SW4-2 SW4-3 SW4-4 SW4-5 SW4-6 SW4-7 SW4-8
             OFF   OFF   OFF   OFF   OFF   OFF   OFF   OFF

4. Configuration Settings:
        Please explore the configuration settings of each module in the RA configurator.
 