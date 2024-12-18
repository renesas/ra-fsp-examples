/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of USBX Peripheral Audio device driver on Renesas RA MCUs based on
	Renesas FSP using Azure RTOS. RA MCU will be connected as USBX peripheral audio to the host PC. An audio WAV file will 
	be played on the host PC using audio player(example Audacity). The WAV audio data will be transferred from host PC to the 
	MCU using the isochronous OUT, through USBX PAUD module. Once the audio data is received on the MCU, the received audio 
	data will be stored on the MCU SRAM. When the host PC starts recording audio using the audio recorder(example Audacity), 
	the MCU will send the audio data stored in the MCU SRAM to the host PC using the isochronous IN, through USBX PAUD module. 
	The user will be able to play, listen and verify the recorded audio file data. USBX status and any errors will be printed 
	on the JLinkRTTViewer which will be handled in separate thread.
 
2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 5.7.0
	e2 studio: Version 2024-10
	SEGGER J-Link RTT Viewer: Version 8.10f
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7
	Audacity v3.1.3(Audio Player/Recorder)
	     
3. Hardware Requirements:
	1) RA board 
	2) 2x Micro USB device cable or 2x Type C USB device cable (For MCK-RA8T1)
	3) A PC with at least 2 USB port(1 for debug and 1 for emulated USB Audio port)

4. To run this example project, user needs to compile and download the example project to the evaluation kit and then follow below instructions to observe the operation of the system.

   Supported Boards: EK-RA4M2, EK-RA4M3, EK RA6M1, EK RA6M2, EK RA6M3, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1 and MCK-RA8T1.

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

5. If user needs to change the USB speed, they can follow below configuration changes.

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

6. Hardware settings for the project:

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

    EK-RA8D1 (High Speed)
        *Note: User must turn-off SW1-6 to use USBHS.
        1. Jumper j7: Connect pins 2-3
        2. Connect Jumper j17 pins
        3. Connect the micro USB end of the micro USB device cable to micro-AB USB High Speed 
        port (J31) of the board.  Connect the other end of this cable to USB port of the 
        host PC.

    MCK-RA8T1 (Full Speed)
        1. Jumper JP9: Connect pins 2-3
        2. Connect Jumper JP10 pins
        3. Connect the micro USB end of the micro USB device cable to micro-AB USB Full Speed 
        port (CN14) of the board.  Connect the other end of this cable to USB port of the 
        host PC.

Note:
1) Need to update src code as below if user changes USB Speed:
   For High Speed: Uncomment the macro APL_AUDIO_20 in usbx_paud_ep.h file
   For Full Speed: Comment the macro APL_AUDIO_20 in usbx_paud_ep.h file

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT-Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x200013bc
   b. Keil: 	Not Available 
   c. IAR:  	Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).