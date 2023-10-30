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

This project demonstrates the basic functionalities of USBX Peripheral Audio device driver on Renesas RA MCUs based on Renesas FSP using Azure RTOS.
RA MCU will be connected as USBX peripheral audio to the host PC. An audio WAV file will be played on the host PC using audio player(example Audacity). The WAV audio data will be transferred from host PC to the MCU using the isochronous OUT, through USBX PAUD module. Once the audio data is received on the MCU, the received audio data will be stored on the MCU SRAM. When the host PC starts recording audio using the audio recorder(example Audacity), the MCU will send the audio data stored in the MCU SRAM to the host PC using the isochronous IN, through USBX PAUD module. The user will be able to play, listen and verify the recorded audio file data. USBX status and any errors will be printed on the JLinkRTTViewer which will be handled in separate thread.
 
2. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below instructions to observe the operation of the system.

   Supported Boards: EK-RA4M2, EK-RA4M3, EK RA6M1, EK RA6M2, EK RA6M3, EK-RA6M4, EK-RA6M5 and EK-RA8M1.

	1) Connect the USBX PAUD device to host PC via two USB Cables: One for USB Debug port and one for USB HS or FS port.

   	2) On the host PC, open Windows Device Manager. 
	   Expand Sound, video and game controllers 
			Verify "USB Audio Demonstration" appears.
	   Expand Audio inputs and outputs
			Verify "Headphones (USB Audio Demonstration)" and "Microphone (USB Audio Demonstration)" appears.

	3) Open the RTT viewer to view the application status messages.	

	4) Play an audio file (eg. .wav, .mp3) using an audio player (example Audacity) on host PC.

	5) MCU records the audio received in SRAM upon reception of the audio data from PC.

	6) After audio play is completed , start recording audio using audio recorder(example Audacity) on the host PC.

	7) MCU sends the received data back to PC upon request from PC.

	8) Export the recorded data to a desired format (eg. .wav, .mp3).

	9) Import the exported recorded audio into audio player(example Audacity).

	10) Listen to the recorded audio and verify. The playback and recorded audio should be similar. There can be differences if the recorded format differs or a partial record is acquired.
     
3. Hardware and Software Requirements:
   
   Hardware : 
	1) RA board 
	2) 2x Micro USB device cable
	3) A PC with at least 2 USB port(1 for debug and 1 for emulated USB Audio port)
    
   Software: 
	1) e2studio.
	2) JLink RTTViewer.
	3) FSP v3.5.0 or higher.
	4) Audacity v3.1.3(Audio Player/Recorder)

4. If user needs to change the USB speed, they can follow below configuration changes.

    	i.  Full-Speed :
            USB Speed                     :    Full Speed
            USB Module Number             :    USB_IP0 Port
	    DMA Source Address            :    FS Address
            DMA Destination Address       :    FS Address
	    DMAC Transfer Size            :    2 Bytes

        ii. High-Speed :
            USB Speed                     :    Hi Speed
            USB Module Number             :    USB_IP1 Port
	    DMA Source Address            :    HS Address
            DMA Destination Address       :    HS Address
	    DMAC Transfer Size            :    4 Bytes
	
5. Hardware settings for the project:
	
    Jumper Settings:
     	
	EK-RA6M1, EK-RA6M2 (Full Speed)
        1. Connect the micro USB end of the micro USB device cable to micro-AB USB (DEBUG USB) 
	   port (J11) of the respective board. 
	2. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   port (J9) of the board.  Connect the other end of this cable to USB port of the 
	   host PC.

	EK-RA6M4, EK-RA4M2, EK_RA4M3 (Full Speed)
        1. Jumper J12 placement is pins 2-3
        2. Connect Jumper J15 pins
        3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
	   port (J11) of the board.  Connect the other end of this cable to USB port of the 
	   host PC.

	EK-RA6M3 (High Speed)
	1. Jumper J7: Connect pins 2-3
        2. Connect Jumper J17 pins
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	   port (J6) of the board.  Connect the other end of this cable to USB port of the 
	   host PC.

	EK-RA6M5, EK-RA8M1 (High Speed)
	1. Jumper J7: Connect pins 2-3
        2. Connect Jumper J17 pins
	3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
	   port (J31) of the board.  Connect the other end of this cable to USB port of the 
	   host PC.

Note:
1) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22001400
   b. Keil: 	Not Available 
   c. IAR:  	Not Available

2) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).