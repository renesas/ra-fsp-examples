/***********************************************************************************************************************
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
* **********************************************************************************************************************/

1. Project Overview:
      The example project demonstrates the basic functionalities of ADC periodic using GPT module on Renesas RA MCUs  
      based on Renesas FSP. When ADC periodic data Sampling is performed GPT triggers an ADC group scan at a periodic interval. 
      When each group scan completes, the data transfer controller (DTC) triggers a data transfer, copying the scan result
      to a user buffer which is configured as ping-pong between the two buffers to allow for continuous processing of the acquired data.
      The sample data can be observed using waveform rendering in the memory viewer in e2studio. 

Hardware and Software Requirements:
      Hardware : 
	1) RA board
	2) Micro USB device cable - 1Nos.
	3) A PC with at least 1 USB port(for debug port)
	4) Breadboard 
	5) Signal generator
      
Hardware Connections:
      The connection from the signal generator to breadboard :
      Connect input signal of (i.e 800Hz) to the horizontal line of the breadboard.

      Board Supporting 32 bit GPT Timer with Enhanced features:
      EK-RA6M3/G
      ADC UNIT 0 : AN00-P000, AN01-P001, AN02-P002, AN03-P008
      ADC UNIT 1 : AN00-P004, AN01-P005, AN02-P006, AN03-P010
      
      EK-RA6M1
      ADC UNIT 0 : AN00-P000, AN01--P001, AN02--P002, AN03--P008 
      ADC UNIT 1 : AN00-P004, AN01--P005, AN02--P006, AN05--P014

      EK-RA6M2
      ADC UNIT 0 : AN00-P000, AN01--P001, AN02--P002, AN03--P008 
      ADC UNIT 1 : AN00-P004, AN01--P005, AN02--P006, AN05--P014
      
      Board Supporting 32 bit GPT timer without Enhanced features:
      EK-RA2E1, EK-RA2L1, EK-RA4M1, EK-RA4M2,FPB-RA6E1
      ADC UNIT 0 : AN00-P000, AN01-P001, AN02-P002, AN03-P003
      EK-RA2E2:
      ADC UNIT 0 : AN09-P014, AN10-P015, AN19-P103, AN20-P102

      
      EK-RA2A1 
      ADC Unit 0 : AN00-P500, AN01-P501, AN02-P502, AN03-P015 
      Short J2 Connector Pin no:36(AVSS0)-----> Pin no: 34(VREFLO) 
      Short J2 Connector Pin no:38(AVCC0)-----> Pin no: 32(VREFHO) 
      
      EK-RA4M3 
      ADC Unit 0 : AN00-P000, AN01-P001, AN02-P002, AN03-P003
      ADC Unit 1 : AN19-P503, AN20--P504, AN21-P505, AN22-P506
      
      EK-RA6M4 
      ADC Unit 0 : AN00-P000, AN01-P001, AN02-P002, AN03-P003
      ADC Unit 1 : AN16-P500, AN17--P501, AN18--P502, AN19--P503

      EK-RA6M5 
      ADC Unit 0 : AN04-P004, AN05--P005, AN06--P006, AN07--P007 
      ADC Unit 1 : AN00-P000, AN01--P001, AN02--P002, AN16--P500
       
      Connect the above pins to horizontal holes of the breadboard so that all pins are shorted to receive a signal generator Input signal of (800Hz) and 
      connect Ground connect to GND. 
      Note: All the channels are connected to the same input signal for testing, the user can configure channels to other input frequencies as per requirement.

Note : 
1.This example project is tested with 800Hz input sinewave signal frequency with amplitude between 0-3.3V
2.When changing the sampling frequency, the user will also need to change:
   The time period in General -> Time period property tab under GPT module.
   ADC Compare A and B Match raw count values, in General, -> Extra features-> ADC trigger property tab under GPT module for MCUs supporting enhanced gpt timer.
  Below is the calculation to calculate the above values.
  a.To calculate the Time period of gpt timer use 
    Time period = 1/input frequency (where input frequency is 2 * Input frequency of signal generator)).
    This calculated Time Period can be set as the GPT Time period in the properties tab.
  b.To calculate Raw period count for ADC trigger use 
    Raw period count =  (Time period in seconds * (PCLKD / (2^ pre-scaler)))
    (where Prescaler value is 0 by default so 2^0 = 1)
    Note: PCLKD frequency will vary as per the MCU variant.
  c.ADC Trigger value for ADC Compare match value for A and B are set to 50% and 99% duty cycle(Raw period count)
    So ADC Compare match value for A = (0.5 * Raw period count) and ADC Compare match value for B = (0.99 * Raw period count).
    Note : ADC Compare A and B Match raw count values are supported for MCU's supporting enhanced gpt timer.
           For MCU's without enhanced gpt timer support, the user only needs to set GPT Time period as ADC group are set to trigger ADC group A at 50% of the cycle and ADC group B 
           at full cycle (GTP overflow interrupt).
3.To add waveform rendering for each channel go to memory view add respective user buffer address for each channel under memory monitor, then click on 
  New Rendering -> Waveform -> Waveform properties
  Select data size as 16 bit, user-specified minimum and maximum values to 0 and 4095 respectively, Channel as Mono, buffer size as 512 for each of the channels 
  as Ping-Pong buffer is used of size 256 each.
     
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000224
   b. Keil: Not Available 
   c. IAR: Not Available
 
2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
