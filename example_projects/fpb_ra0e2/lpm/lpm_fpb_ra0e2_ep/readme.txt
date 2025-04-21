/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of the Low Power Mode module (LPM) on Renesas RA MCUs
	using Renesas FSP. The project also illustrates methods to reduce MCU power consumption and restore the
	pre-LPM states of peripheral modules. The EP will perform different low power mode transitions based on
	the user's selection of Low Power Mode, request, and cancel sources in the RA Configurator. The MCU will
	automatically enter each LPM mode and then wait for cancel sources to trigger manually by an external
	interrupt or automatically by a timer to exit LPM mode. Turn OFF the LED1 and display the LPM Mode Name
	before entering each LPM mode. Turn ON the LED1 and display the cancel source name after exiting each
	LPM mode. The EP log will print to the Host PC via the UART interface at a baud rate of 115200 bps.
	The LED2 will turn ON if an error occurs.

2. Hardware Requirements:
	1 x Renesas RA board.
	1 x USB cable for programing and debugging.
		- Type-C USB cable: MCK-RA6T2, MCK-RA4T1, MCK-RA6T3, MCK-RA8T1, FPB-RA0E1, FPB-RA8E1, EK-RA4L1
		- Mini USB cable: RSSK-RA6T1
		- Micro USB cable: the other boards
	For RA boards do not support J-Link On Board VCOM functionality: EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4M1,
	EK-RA4M2, EK-RA4M3, EK-RA4W1, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, FPB-RA4E1, FPB-RA6E1,
	MCK-RA6T2, RSSK-RA6T1
		1 x USB to UART Converter (e.g, Pmod USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/)
			- Jumper JP1 placement is LCL-VCC pins
			  (The attached system board is powered independently from the Pmod USBUART).
		1 x Micro USB cable for boards have to use Pmod USBUART.
	For RSSK-RA6T1, MCK-RA4T1, MCK-RA6T3, MCK-RA8T1:
		1 x Pmod BTN.
	
3.Hardware Connections:
	For EK-RA2A1:
	  	1) RXD MISO P301 (J2:2)   <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P302 (J2:4)   <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (J2:12)  <-------> GND Pin of Pmod USBUART (J2:5)                        
										  
	For EK-RA2E1:                             
	  	1) RXD MISO P110 (J2:28)  <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P109 (J2:27)  <-------> RXD Pin of Pmod USBUART (J2:2)
	 	3) GND 	   	 (J2:40)  <-------> GND Pin of Pmod USBUART (J2:5)                          
										  
	For EK-RA2E2:                             
	  	1) RXD MISO P401 (J1:3)   <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P400 (J1:2)   <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (J1:13)  <-------> GND Pin of Pmod USBUART (J2:5)                          
										  
	For EK-RA2L1:                             
	  	1) RXD MISO P301 (J1:30)  <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P302 (J1:29)  <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (J1:40)  <-------> GND Pin of Pmod USBUART (J2:5)                        
										  
	For EK-RA4M1:                             
	  	1) RXD MISO P410 (J2:2)   <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P411 (J2:4)   <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (J2:12)  <-------> GND Pin of Pmod USBUART (J2:5)                         
										  
	For EK-RA4M2:                             
	  	1) RXD MISO P601 (J3:17)  <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P602 (J3:16)  <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (J3:27)  <-------> GND Pin of Pmod USBUART (J2:5)                         
										  
	For EK-RA4M3:                             
	  	1) RXD MISO P202 (J4:14)  <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P203 (J4:13)  <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (J4:39)  <-------> GND Pin of Pmod USBUART (J2:5)                         
										  
	For EK-RA4W1:                             
	  	1) RXD MISO P206 (CN7:7)  <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P205 (CN7:8)  <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (CN7:19) <-------> GND Pin of Pmod USBUART (J2:5)                         
										  
	For EK-RA6M1:                             
	  	1) RXD MISO P100 (J2:2)   <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P101 (J2:4)   <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	         (J2:12)  <-------> GND Pin of Pmod USBUART (J2:5)
	  	4) IRQ8 P415     (J2:5)   <-------> IRQ0-DS P206 	    (J4:12) 
	     	Because user button S1 being used P415/IRQ8 cannot cancel Deep Software Standby mode.
	
	For EK-RA6M2:
	  	1) RXD MISO P410 (J2:2)   <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P411 (J2:20)  <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (J2:12)  <-------> GND Pin of Pmod USBUART (J2:5)
	  	4) IRQ0 P105     (J1:37)  <-------> IRQ0-DS P206	    (J1:3)
             	Because user button S1 being used P105/IRQ0 cannot cancel Deep Software Standby mode.
	
	For EK-RA6M3, EK-RA6M3G:
	  	1) RXD MISO P410 (J3:35)  <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P411 (J3:36)  <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	         (J3:39)  <-------> GND Pin of Pmod USBUART (J2:5)                        
										  
	For EK-RA6M4:                    
	  	1) RXD MISO P100 (J3:38)  <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P101 (J3:37)  <-------> RXD Pin of Pmod USBUART (J2:2)
	 	3) GND 	   	 (J3:39)  <-------> GND Pin of Pmod USBUART (J2:5)                         
		
	For EK-RA6M5:                    
	  	1) RXD MISO P100 (J3:39)  <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P101 (J3:38)  <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (J3:40)  <-------> GND Pin of Pmod USBUART (J2:5) 
								  
	For FPB-RA4E1, FPB-RA6E1:                 
	  	1) RXD MISO P100 (J4:25)  <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P101 (J4:24)  <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND 	   	 (J3:45)  <-------> GND Pin of Pmod USBUART (J2:5)                        
									  
	For MCK-RA6T2:                            
	  	1) RXD MISO PB04 (CN4:29) <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI PB05 (CN4:27) <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND	   	 (CN4:25) <-------> GND Pin of Pmod USBUART (J2:5)
	  	4) Connect to inverter board with INV2 Port to use SW2 on inverter board as the user push-button S1
    									  
	For RSSK-RA6T1:                            
	  	1) RXD MISO P206 (CN10:3) <-------> TXD Pin of Pmod USBUART (J2:3)
	  	2) TXD MOSI P205 (CN10:2) <-------> RXD Pin of Pmod USBUART (J2:2)
	  	3) GND    	 (CN10:4) <-------> GND Pin of Pmod USBUART (J2:5)
	  	4) IRQ06-DS P000 (CNB:5)  <-------> BTN1 Pin of Pmod BTN    (J1:2)
	  	5) VCC           (CNB:18) <-------> VCC  Pin of Pmod BTN    (J1:6)
	  	6) GND           (CNB:20) <-------> GND  Pin of Pmod BTN    (J1:5)
            	Use BTN1 of Pmod BTN as the user push-button S1

	For MCK-RA4T1, MCK-RA6T3:
    	  	1) Connect the Pmod BTN (J1:1-6) to the RA board via the PMOD1 Port (CN4:7-12)
             	   Use BTN1 of Pmod BTN as the user push-button S1 
	
	For FPB-RA2E3:
	  	1) P200	   	 (J3:21)  <-------> IRQ0 P206	   	 (J3:18)
             	   Because user button S1 (P200) does not have interrupt to cancel/end lpm mode selected.

	For MCK-RA8T1:
	  	1) Connect the Pmod BTN (J1:1-6) to the RA board via the PMOD(CN6:1)
	           Use BTN1 of Pmod BTN as the user push-button S1.

	For FPB-RA0E1:
	  	To use J-Link OB VCOM for Serial Terminal (UART):
	  	1) Short the E4 trace.
	  	2) Replace R10 by 150ohm resistor.

	For FPB-RA0E2:
	  	To use J-Link OB VCOM for Serial Terminal (UART), the user must close E46, E47.

4. Configuration Settings:
	The pin configuration file for LPM mode in each example project is named with the suffix "LPM". 
	The user can change these configurations by selecting the file and setting the ports on the FSP's pin configurator.
	Unused pins in LPM modes are set as input ports to reduce the MCU's power consumption;
	check the "Handling of Unused Pins" section in the MCU's user manual for more details.

	Four or three different LPM module's instances named g_lpm_sleep, g_lpm_deep_sleep, g_lpm_sw_standby,
	g_lpm_sw_standby_with_snooze, g_lpm_deep_sw_standby is used in each LPM example project depending on
	the support of the MCU line.

	Note: If using RTOS, the SysTick timer must be stopped before entering the Sleep mode because any interrupt
	      will cancel the Sleep mode. The timer must be re-started after waking up.

5. The hardware modifications required for measuring the MCU's current.
	For EK-RA2A1, EK-RA4M1:
	  	1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
 
	For EK-RA6M1:
	  	1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	  	2) Connect a jumper wire from J2:5(P415) to J4:12(P206/IRQ0-DS). 
	     	   Since the user push-button S1 used P415/IRQ8, it won't cancel Deep Software Standby mode.
	    	   This step will enable P206/IRQ0-DS as the canceling source for all of LPM modes.
		Note:
			To get more accurate MCU's current (Icc) on EK-RA6M1, you should cut traces between E10, E50 and E52 
			to isolate the Icc from the Analog power supply current AIcc.

	For EK-RA6M2:
	  	1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	  	2) Connect a jumper wire from J1:37(P105) to J1:3(P206/IRQ0-DS). 
	     	   Since the user push-button S1 used P105/IRQ0, it won't cancel Deep Software Standby mode.
	           This step will enable P206/IRQ0-DS as the canceling source for all of LPM modes.

	For EK-RA4W1:
	  	1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of SS20.
   
	        To measure MCU's current (Icc) on EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2 kits directly, 
	        using a Digital Multi-meter connecting between pin1 and pin3 of E6 and on EK-RA4W1 measure
	        current between pin1 and pin2 of J7; This is the recommended method.

	        You can also measure and calculate the Icc by capturing the voltage drops across a resistor from 
	        5 ohm to 20 ohm connected between pin1 and pin3 of E6.

	For EK-RA2L1, EK-RA2E1, EK-RA2E2, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA4E2, EK-RA6E2, EK-RA6M5, EK-RA8M1, 
	    EK-RA8D1, EK-RA4L1:
	  	1) Remove R2 (Not applicable to EK-RA2L1, EK-RA2E1, EK-RA2E2, EK-RA4E2, EK-RA6E2).
	  	2) Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	  	3) To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU
		   pins on the kit connectors.

	For FPB-RA6E1, FPB-RA4E1:
	  	1) Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	  	2) To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU 
	           pins (CN1) on the kit connectors.

	For RSSK-RA6T1, MCK-RA6T2, MCK-RA8T1, FPB-RA0E1, FPB-RA0E2:
	  	Not applicable

	For FPB-RA2E3:
	  	1) Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	  	2) To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU
	           pins on the kit connectors.

        For FPB-RA8E1:
	  	1) Measure the voltage drops across R2 and calculate MCU's current (Icc); replace R2 by a bigger resistor if needed.
	  	2) To measure MCU's current (Icc) directly, remove R2 resistor and connect Multi-meter between the +3V3 and +3V3_MCU 
	           pins on the kit connectors.

6. Verifying Operation:
	1) Download the example project to the RA board using the USB debug port. J-Link may be disconnected when MCU enter LPM mode.
	   It is recommended to stop debugging after downloading the EP.
	2) Connect the Pmod USBUART module to the RA board according to the hardware connection instructions, and then connect the
           Pmod USBUART to the host PC via a micro-USB cable. Skip if the RA board supports J-Link VCOM functionality.
	3) Open a terminal application on the host PC and connect to the COM Port provided by the J-Link onboard or Pmod USB UART.
	   The configuration parameters of the serial port on the terminal application are as follows:
			Baud rate: 115200 bps
			Data length: 8-bits  
			Parity: none
			Stop bit: 1-bit
			Flow control: none
	4) Power-cycle the kits.
	   Power-cycle the kit is required to avoid influence of debugging into cancel LPM mode. When the influence happens,
	   the log "LPM is canceled, but source has not been detected" will be printed.
	5) Press the user push-button S1 or wait 10 seconds to enter and cancel LPM mode. It will turn OFF LED1 before entering
	   each LPM mode and turn ON LED1 after exiting each LPM mode.
	6) The LPM mode name will display on the terminal application before entering each LPM mode, and the canceling source name
	   will display on the terminal application after exiting each LPM mode.
	
	Transition sequences: 
	1. Normal → Sleep → Normal.						(Supported by all MCU lines)
	2. Normal → Deep Sleep → Normal.					(Supported by RA8 MCU lines)
	3. Normal → SW Standby → Normal.					(Supported by all MCU lines)
	4. Normal → SW Standby → Snooze → SW Standby → Snooze → Normal.		(Supported by all MCU lines except RA8 MCU lines)
	5. Normal → Deep SW Standby → Normal.					(Supported by RA4E2, RA4M2, RA4M3, RA4E1, RA4T1,
										 RA6 MCU lines, and RA8 MCU lines)
