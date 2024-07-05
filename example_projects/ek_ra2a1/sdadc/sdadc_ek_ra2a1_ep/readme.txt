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
       This Example project demonstrates the basic functionalities of SDADC running on Renesas RA MCUs using RA board and external variable power
       supply.SDADC channel 0 is connected as single ended input mode and channel 2 is connected as differential ended input mode to power supply.
       Data read from both the channels is displayed on JLink RTT Viewer.

2. Hardware settings for the project:

       Hardware  : External power supply with more than one output source(variable voltages).

       Hardware connections:
   
       RA2A1-EK Board                            Power supply

       Header J3 Pin 8(P100)  ----------------  +v supply(variable between 0.2 to 1.8V).

       Differential Ended(+ve end voltage should be greater than -ve end.Differential input max difference is 800mv)
       Header J1 Pin 9(P104)  ----------------  +v supply(variable between 0.2 to 1.8V)
       Header J1 Pin 7(P105)  ----------------  +v supply(variable between 0.2 to 1.8V).
       Header J1 Pin 17(VSS)  ----------------  Gnd 
   

Note: In absence of power supply with multiple output sources, the voltage can be supplied with single output and voltage divider circuit as explained below.
​
      For external power supply with a single output, use resistor(R1 =10 ohms) and (R2 =100 ohms) in series(as a voltage divider) across one end of the output. This
      setup provides different input voltages to differential ended pins of SDADC. 
​
Example:Output from power supply = 1.6 V. Using resistor(R1 =10ohms) and (R2 =100 ohms) in series(as voltage divider) gives output of 1.45V(approx). Now connect
        1.6V to Pin 9 and 1.45V to Pin 7.