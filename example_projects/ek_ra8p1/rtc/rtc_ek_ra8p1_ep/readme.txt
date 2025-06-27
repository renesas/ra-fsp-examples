/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
     	The example project demonstrates the typical use of the RTC HAL module APIs. The project initializes the RTC
	module and allows the user to set RTC calendar time and calendar alarm. The user can enable periodic interrupt
	and can also view the current RTC calendar time. On enabling periodic interrupt, on-board LED toggles every 1
	second and on occurrence of calendar alarm, LED is turned ON.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x Type-C USB cable for programming and debugging.
	
Note:
1) The user is expected to use J-Link RTT Viewer with input will be sent when pressing Enter:
   Input -> Sending... -> Send on Enter

2) The user is expected to set RTC calendar Date and Time (option 1) before other runnings.

3) The user is expected to enter data not exceeding 15 bytes in size.

4) Operation is not guaranteed for any user input value other than integer, char (e.g., float, special char)
   or incorrect format through RTT Viewer.

5) The user is expected to enter Time values to set calendar date and time (Option 1) as shown below.
	Sample input: 23:10:2019 16:14:55 (Format: DD:MM:YYYY HH:MM:SS)

6) The user is expected to enter Second values to set calendar alarm date and time (Option 2) as shown below.
	Sample input: 35 (Format: SS)

7) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000430
   b. Keil:	0x22000850
   c. IAR: 0x2200094c
 
8) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release, keil\Listings, iar\Debug\List).
