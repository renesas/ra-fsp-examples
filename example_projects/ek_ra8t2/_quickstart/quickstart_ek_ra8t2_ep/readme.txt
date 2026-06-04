/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	The example project demonstrates some of the capabilities of the EK and FSP by using multiple peripherals.
	The project initializes the OSPI, UART various timers and GPIO modules along with various supporting stacks
	to create an interactive user demo.

	There is a serial console which can be activated by connecting USB cable to DEBUG1 (J10),
	connection settings are Speed 115200, Data 8-bit, Parity None, Stop 1-bit, Flow Control None.
	Once application is running the user can control the board by button presses on the board and see regularly
	updated board configuration on the kit information screen using the serial console, please read the following
	r20qs0097eg Evaluation Kit for RA8T2 Microcontroller Group EK-RA8T2 v1 Quick Start Guide.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.5.0
	e2 studio: Version 2026-04.2
	Terminal Console Application: Tera Term or a similar application
	USB Serial Drivers (included in Windows 11)
	Windows® 11 operating system
  
3. Hardware settings for the project:
	Link to access Quick Start Guide - https://www.renesas.com/en/document/qsg/ek-ra8t2-v1-quick-start-guide

	Configuration switch (SW4) settings
	    SW4-1 SW4-2 SW4-3 SW4-4 SW4-5 SW4-6 SW4-7 SW4-8
	     OFF   OFF   OFF   OFF   OFF   OFF   OFF   OFF
	Configuration switch (SW5) settings
	    SW5-1 SW5-2 SW5-3 SW5-4 SW5-5 SW5-6 SW5-7 SW5-8
	     OFF   OFF   OFF   OFF   OFF   OFF   OFF   OFF
	Configuration switch (SW6) settings
	    SW6-1 SW6-2 SW6-3 SW6-4 SW6-5 SW6-6 SW6-7 SW6-8 SW6-9 SW6-10
	     OFF   OFF   OFF   OFF   ON    OFF   ON    ON    ON    ON
			 
4. Configuration Settings:
	Please explore the configuration settings of each module in the RA configurator.
 