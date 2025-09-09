/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This example project demonstrates audio streaming using the SSI module with a double-buffer mechanism.
	A sine wave is generated and continuously streamed via a loopback connection between the SSITX and SSIRX pins.
	The sine wave data is stored in two buffers, alternating between them to ensure continuous and uninterrupted
	streaming. The transmitted and received audio data are compared to verify the successful completion of the SSI
	transfer. The results will be displayed on the RTT Viewer. The sample data can be observed using waveform
	rendering in the memory viewer of e2studio.

2. Software Requirements:
	Renesas Flexible Software Package (FSP): Version 6.1.0
	e2 studio: Version 2025-07
	SEGGER J-Link RTT Viewer: Version 8.58
	LLVM Embedded Toolchain for ARM: Version 18.1.3

3. Hardware Requirements:
	Supported RA boards: EK-RA4E2, EK-RA4L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA6E2, EK-RA6M1, EK-RA6M2, EK-RA6M3,
			     EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8D1, EK-RA8E2, EK-RA8M1, FPB-RA6E1, FPB-RA8E1, 
			     EK-RA2L2, EK-RA8P1
	1 x Renesas RA Board.
	1 x Type-C USB cable.
	1 x Host PC.
	1 x Jumper wire (For EK-RA4E2, FPB-RA6E1: 2 x Jumper wires).

4. Hardware Connections:
	For EK-RA4E2:
		Connect SSITXD P301 (J2:24) <--> SSIRXD P302 (J2:23).
		Connect GPT1 P105 (J2:34)   <--> AUDIO_CLK P100 (J2:39).

	For EK-RA4L1:
		Connect SSITXD P405 (J2:42) <--> SSIRXD P406 (J2:38).

	For EK-RA4M1:
		Connect SSITXD P405 (J2:31) <--> SSIRXD P406 (J2:15).

	For EK-RA4M2:
		Connect SSITXD P405 (J1:7) <--> SSIRXD P406 (J1:10).

	For EK-RA6E2:
		Connect SSITXD P301 (J2:24) <--> SSIRXD P302 (J2:23).

	For EK-RA6M1:
		Connect SSITXD P405 (J1:16) <--> SSIRXD P406 (J1:18).

	For EK-RA6M2:
		Connect SSITXD P405 (J1:6) <--> SSIRXD P406 (J1:8).

	For EK-RA6M3, EK-RA6M3G:
		Connect SSITXD P405 (J3:13) <--> SSIRXD P406 (J3:16).
		Cut E14 to get signal output for SSITXD pin and SSIRXD pin.

	For EK-RA4M3, EK-RA6M4:
		Connect SSITXD P405 (J1:7) <--> SSIRXD P406 (J1:8).

	For EK-RA6M5:
		Connect SSITXD P405 (J1:8) <--> SSIRXD P406 (J3:9).
		Cut E22 to get signal output for SSITXD pin and SSIRXD pin.

	For EK-RA8D1:
		Connect SSITXD P405 (J51:8) <--> SSIRXD P406 (J51:5).
		Turn OFF SW1-3 and SW1-5 to get signal output for SSITXD pin and SSIRXD pin.

	For EK-RA8E2:
		Connect SSITXD P114 (J14:10) <--> SSIRXD P115 (J14:8).

	For EK-RA8M1:
		Connect SSITXD P405 (J56:29) <--> SSIRXD P406 (J56:27).
		Install jumper J61 to get signal output for SSITXD pin and SSIRXD pin.

	For FPB-RA6E1:
		Connect SSITXD P405 (J3:6) <--> SSIRXD P406 (J3:7).
		Connect GPT2 P103 (J4:22)  <--> AUDIO_CLK P402 (J3:3).

	For FPB-RA8E1:
		Connect SSITXD P405 (J4:14) <--> SSIRXD P406 (J4:13).

	For EK-RA2L2:
		Connect SSITXD P409 (J2:7) <--> SSIRXD P408 (J2:8).

	For EK-RA8P1:
		Connect SSITXD P115 (J37:22) <--> SSIRXD P114 (J37:21).

Note:
1) On EK-RA6M3, EK_RA6M3G boards, P403 (which is also connected to SSISCK) is connected to LED1. As a result,
   it will glow while running the SSI application.

2) Segger RTT block address may be needed to download and observe EP operation using a hex file with RTT Viewer.
   RTT Block address for hex file committed in repository are as follows:
   a. e2studio: 0x22000470
   b. Keil:     Not Available
   c. IAR:      Not Available

3) If an EP is modified, compiled, and downloaded please find the block address (for the variable in RAM called _SEGGER_RTT)
   in .map file generated in the project folder (e2studio\Debug or e2studio\Release).
