/***********************************************************************************************************************
* 
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
* 
**********************************************************************************************************************/

1. Project Overview:

	This example project demonstrates configuration of the DAC, AGT, and DTC to generate a continuous sinusoid signal. The
	user will define the samples per period and frequency in Hz of the desired output wave in JLink RTT Viewer. The 
	samples per period is used to compute and store 1 period of the sinusoid in a look up table (LUT), and the frequency is 
	used to set the	AGT period to the appropriate sampling rate. The AGT generates an interrupt which triggers the DTC to 
	transfer the next value of the LUT to the DADR0 register.  
	
	The output sinusoid wave will oscillate between the minimum and maximum DAC values at the user-specified samples per period
	and frequency.
	
2. Hardware Requirements:
	
	  i. Renesas RA board (EK-RA2A1, EK-RA2L1, EK-RA4M2, or EK-RA6M4)
	 ii. 1x micro-USB cable
	iii. PC with USB port and JLink RTT Viewer software
	
3. Hardware Connections:

	Connect micro-USB cable between J10 on RA board and USB port on PC. 

	The following pin connections are required to capture the output signal from the DAC:
	* RA2L1, RA4M2, RA6M4:
		P014 is DA0 ouput
	* RA2A1:
		P500 is DA12_0 ouptut
	
	
NOTE: 
	JLink RTT Viewer only supports integer values for user input.

	
	
