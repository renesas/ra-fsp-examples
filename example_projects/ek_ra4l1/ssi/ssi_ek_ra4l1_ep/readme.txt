/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:

This example project demonstrates audio streaming using the SSI module with a double-buffer mechanism. A sine wave 
is generated and continuously streamed via a loopback connection between the SSITX and SSIRX pins. The sine wave data 
is stored in two buffers, alternating between them to ensure continuous and uninterrupted streaming. The transmitted 
and received audio data are compared to verify the successful completion of the SSI transfer. The results will be displayed 
on the RTT viewer. The sample data can be observed using waveform rendering in the memory viewer of e2studio.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.2.0
	e2 studio: Version 2025-10
	SEGGER J-Link RTT Viewer: Version 8.74
	GCC ARM Embedded Toolchain: Version 13.2.1.arm-13-7

3. Hardware Requirements:
	1) 1 x Renesas RA board (EK-RA4E2, EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2, 
	EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8E2, EK-RA8M1, FPB-RA6E1, FPB-RA8E1). 
	2) 1 x Micro USB cable or Type-C USB cable (for EK-RA4L1, EK-RA8E2, FPB-RA8E1).
	3) 1 x Host PC.
	4) 1 x Jumper wire.

4. Hardware Connections:
	For EK-RA4E2:
		Connect P301 (J2:24) (SSITXD pin)	<--> P302 (J2:23) (SSIRXD pin)
				P105 (J2:34) (GPT1)	<--> P100 (J2:39) (AUDIO_CLK)

	For EK-RA4L1:
		Connect P405 (J2:42) (SSITXD pin)	<--> P406 (J2:38) (SSIRXD pin)

	For EK-RA4M1:
		Connect P405 (J2:31) (SSITXD pin)	<--> P406 (J2:15) (SSIRXD pin)
		
	For EK-RA4M2:
		Connect P405 (J1:7) (SSITXD pin)	<--> P406 (J1:10) (SSIRXD pin)

	For EK-RA6E2:
		Connect P301 (J2:24) (SSITXD pin)	<--> P302 (J2:23) (SSIRXD pin)

	For EK-RA6M1:
		Connect P405 (J1:16) (SSITXD pin)	<--> P406 (J1:18) (SSIRXD pin)

	For EK-RA6M2:
		Connect P405 (J1:6) (SSITXD pin)	<--> P406 (J1:8) (SSIRXD pin)

	For EK-RA6M3, EK-RA6M3G:
		Connect P405 (J3:13) (SSITXD pin)	<--> P406 (J3:16) (SSIRXD pin)
		Cut E14 to get signal output for SSITXD pin and SSIRXD pin.

	For EK-RA4M3, EK-RA6M4:
		Connect P405 (J1:7) (SSITXD pin)	<--> P406 (J1:8) (SSIRXD pin)

	For EK-RA6M5:
		Connect P405 (J1:8) (SSITXD pin)	<--> P406 (J3:9) (SSIRXD pin)
		Cut E22 to get signal output for SSITXD pin and SSIRXD pin.

	For EK-RA8D1:
		Connect P405 (J51:8) (SSITXD pin)	<--> P406 (J51:5) (SSIRXD pin)
		Turn OFF SW1-3 and SW1-5 to get signal output for SSITXD pin and SSIRXD pin.

	For EK-RA8E2:
		Connect P114 (J14:10) (SSITXD pin)	<--> P115 (J14:8) (SSIRXD pin)

	For EK-RA8M1:
		Connect P405 (J56:29) (SSITXD pin)	<--> P406 (J56:27) (SSIRXD pin)
		Install jumper J61 to get signal output for SSITXD pin and SSIRXD pin.

	For FPB-RA6E1:
		Connect P405 (J3:6) (SSITXD pin)	<--> P406 (J3:7) (SSIRXD pin)
				P103 (J4:22) (GPT2)	<--> P402 (J3:3) (AUDIO_CLK)

	For FPB-RA8E1:
		Connect P405 (J4:14) (SSITXD pin)	<--> P406 (J4:13) (SSIRXD pin)

Note:
1) On EK-RA6M3, EK_RA6M3G boards, P403 (which is also connected to SSISCK) is connected to LED1. As a result, it will glow while running 
   the SSI application.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x20000c84
   b. Keil:     Not Available 
   c. IAR:      Not Available
 
3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT) 
   in .map file generated in the build configuration folder (Debug/Release).
