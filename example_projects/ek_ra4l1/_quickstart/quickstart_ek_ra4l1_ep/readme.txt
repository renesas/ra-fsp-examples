/***********************************************************************************************************************
* Copyright [2025] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
        The project initializes the QSPI , LPM , SLCD and GPIO modules to create an interactive user demo.

        Once programmed the user can control the board by button presses on the board and see regularly updated board 
        configuration on the kit information screen.
        On the attached Segment LCD panel the user can interact with the application, please read the following document
        d020244_11_s01_v01_r20qs0081eg0100 Evaluation Kit for RA4L1 Microcontroller Group EK-RA4L1 Quick Start Guide

2. Hardware settings for the project:
        Please configure the Jumper settings as described below:
        Configuration switch (SW1) settings 
            SW4-1 SW4-2 SW4-3 SW4-4 SW4-5 SW4-6 SW4-7 SW4-8
             OFF   OFF   OFF   OFF   OFF   OFF   OFF   OFF
        
3. Configuration Settings:
        Please explore the configuration settings of each module in the RA configurator.

 