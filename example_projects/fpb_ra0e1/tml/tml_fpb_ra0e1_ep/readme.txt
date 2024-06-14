/***********************************************************************************************************************
* Copyright [2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
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
	The example project showcases how the TML driver operates on Renesas RA MCUs. The demonstration includes showcasing the TML timer with 
	counter operation, capture operation in normal mode, and counter operation in low power mode. In TML counter operation (normal mode), 
	three TML instances (which are configured as 8-bit, 16-bit, and 32-bit counter modes) operate for the inputted duration by the user. 
	In TML 16-bit capture operation, user can trigger an ELC software event to get the raw counts value of 16-bit timer. In TML counter
	operation (low power modes), the 32-bit timer is configured as counter mode with period 10 seconds. Once the 32-bit timer has expired,
	the RA board is woken up from LPM to normal mode. The EP information and error messages will be printed to the Host PC.

	NOTE1: To display information, users can select between the SEGGER J-Link RTT Viewer and the Serial Terminal (UART) with J-Link OB VCOM. 
	       By default, EP information is printed to the host PC via the Serial Terminal.
	       To utilize the SEGGER J-Link RTT Viewer instead of the Serial Terminal, please follow the instructions in tml_notes.md file.

2. Hardware Requirements:

	Supported RA Boards: FPB-RA0E1

	1 x Renesas RA board.
	1 x USB Type C cable.
	1 x Host PC.

3. Hardware Connections:

    FPB-RA0E1:     
	---------
	1. Connect RA board to Host PC by USB Type C cable.
  	2. For using the Serial Terminal (UART):     
       		The soldering bridge E4 must be closed.
		Populate R10 with a resistor ranging from 0 to 150 ohms.


NOTE2:
    For using the Serial Terminal (UART):
      1) To echo back what was typed in Tera Term, the user needs to enable it through [Setup] -> [Terminal...] -> Check [Local echo].
      2) The serial terminal setting parameters are as follows:
      	Select the COM port provided by the J-Link on-board.
      	Baud rate: 115200 bps
      	Data length: 8-bits  
      	Parity: none
      	Stop bit: 1-bit
      	Flow control: none
