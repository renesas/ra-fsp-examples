/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

1. Project Overview:
	This project demonstrates the basic functionalities of the Low Power Mode module (LPM) on Renesas RA MCUs using Renesas FSP.
	The project also illustrates methods to reduce MCU power consumption and restore the pre-LPM states of peripheral modules.
	The EP will perform different low power mode transitions based on the user's selection of Low Power Mode, request, and cancel
	sources in the RA Configurator. The MCU will automatically enter each LPM mode and then wait for cancel sources to trigger 
	manually by an external interrupt or automatically by a timer to exit LPM mode. Turn off the LED1 and display the LPM Mode
	Name before entering each LPM mode. Turn on the LED1 and display the cancel source name after exiting each LPM mode.The EP 
	log will print to the Host PC via the UART interface at a baud rate of 115200 bps. The LED2 will turn on if an error occurs.

2. Hardware Requirements:
	1 x Renesas RA board
	1 x Micro USB cable or 1 x Type C USB cable (For MCK-RA6T2, MCK-RA4T1, MCK-RA6T3, MCK-RA8T1, FPB-RA0E1, FPB-RA8E1) 
            or 1 x Mini USB cable (For RSSK-RA6T1) + Additional 1 x Micro USB cable for boards have to use Pmod USBUART. 
	1 x USB to UART Converter (eg. PMOD USBUART: https://digilent.com/shop/pmod-usbuart-usb-to-uart-interface/), if the RA Board 
            does not support J-Link VCOM functionality.
	    + RA Boards have to use Pmod USBUART: EK-RA2A1, EK-RA2E1, EK-RA2E2, EK-RA2L1, EK-RA4M1, EK-RA4M2, EK-RA4M3, EK-RA4W1,
	      EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, FPB-RA4E1, FPB-RA6E1, MCK-RA6T2, RSSK-RA6T1.
	    + Jumper JP1 placement is pins LCL-VCC.
	1 x Pmod BTN (Only RSSK-RA6T1, MCK-RA4T1, MCK-RA6T3, MCK-RA8T1 have to use)
	
3. Hardware and Software Setups:
	The pin configuration file for LPM mode in each example project is named with the suffix "LPM". User can change these configurations 
	by selecting the file and setting the ports on the FSP's pin configurator. Unused pins in LPM modes are set as input ports to 
	reduce the MCU's power consumption; check the "Handling of Unused Pins" section in the MCU's user manual for more details.

	Four or three different LPM module's instances named g_lpm_sleep, g_lpm_deep_sleep, g_lpm_sw_standby, g_lpm_sw_standby_with_snooze,
	g_lpm_deep_sw_standby is used in each LPM example project depending on the support of the MCU line.

	Note:
		If using RTOS, Systick timer must be stopped before entering the Sleep mode because any interrupt will cancel the sleep mode. 
		The timer must be re-started after waking up.

	Hardware Connections:
	For EK-RA2A1:
	  	1) RXD MISO P301 (J2-Pin2)   <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P302 (J2-Pin4)   <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (J2-Pin12)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                        
										  
	For EK-RA2E1:                             
	  	1) RXD MISO P110 (J2-Pin28)  <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P109 (J2-Pin27)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	 	3) GND 	   	 (J2-Pin40)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                          
										  
	For EK-RA2E2:                             
	  	1) RXD MISO P401 (J1-Pin3)   <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P400 (J1-Pin2)   <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (J1-Pin13)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                          
										  
	For EK-RA2L1:                             
	  	1) RXD MISO P301 (J1-Pin30)  <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P302 (J1-Pin29)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (J1-Pin40)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                        
										  
	For EK-RA4M1:                             
	  	1) RXD MISO P410 (J2-Pin2)   <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P411 (J2-Pin4)   <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (J2-Pin12)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                         
										  
	For EK-RA4M2:                             
	  	1) RXD MISO P601 (J3-Pin17)  <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P602 (J3-Pin16)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (J3-Pin27)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                         
										  
	For EK-RA4M3:                             
	  	1) RXD MISO P202 (J4-Pin14)  <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P203 (J4-Pin13)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (J4-Pin39)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                         
										  
	For EK-RA4W1:                             
	  	1) RXD MISO P206 (CN7-Pin7)  <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P205 (CN7-Pin8)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (CN7-Pin19) <-------> GND Pin of Pmod USBUART (J2-Pin5)                         
										  
	For EK-RA6M1:                             
	  	1) RXD MISO P100 (J2-Pin2)   <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P101 (J2-Pin4)   <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	         (J2-Pin12)  <-------> GND Pin of Pmod USBUART (J2-Pin5)
	  	4) IRQ8 P415     (J2-Pin5)   <-------> IRQ0-DS P206 	   	 (J4-Pin12) 
	     	Because user button S1 being used P415/IRQ8 cannot cancel Deep Software Standby mode.
	
	For EK-RA6M2:
	  	1) RXD MISO P410 (J2-Pin2)   <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P411 (J2-Pin20)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (J2-Pin12)  <-------> GND Pin of Pmod USBUART (J2-Pin5)
	  	4) IRQ0 P105     (J1-Pin37)  <-------> IRQ0-DS P206		 (J1-Pin3)
             	Because user button S1 being used P105/IRQ0 cannot cancel Deep Software Standby mode.
	
	For EK-RA6M3/ EK-RA6M3G:
	  	1) RXD MISO P410 (J3-Pin35)  <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P411 (J3-Pin36)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	         (J3-Pin39)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                        
										  
	For EK-RA6M4:                    
	  	1) RXD MISO P100 (J3-Pin38)  <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P101 (J3-Pin37)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	 	3) GND 	   	 (J3-Pin39)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                         
		
	For EK-RA6M5:                    
	  	1) RXD MISO P100 (J3-Pin39)  <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P101 (J3-Pin38)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (J3-Pin40)  <-------> GND Pin of Pmod USBUART (J2-Pin5) 
								  
	For FPB-RA4E1/FPB-RA6E1:                 
	  	1) RXD MISO P100 (J4-Pin25)  <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P101 (J4-Pin24)  <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND 	   	 (J3-Pin45)  <-------> GND Pin of Pmod USBUART (J2-Pin5)                        
									  
	For MCK-RA6T2:                            
	  	1) RXD MISO PB04 (CN4-Pin29) <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI PB05 (CN4-Pin27) <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND	   	 (CN4-Pin25) <-------> GND Pin of Pmod USBUART (J2-Pin5)
	  	4) Connect to inverter board with INV2 Port to use SW2 on inverter board as the user push-button S1
    									  
	For RSSK-RA6T1:                            
	  	1) RXD MISO P206 (CN10-Pin3) <-------> TXD Pin of Pmod USBUART (J2-Pin3)
	  	2) TXD MOSI P205 (CN10-Pin2) <-------> RXD Pin of Pmod USBUART (J2-Pin2)
	  	3) GND    	 (CN10-Pin4) <-------> GND Pin of Pmod USBUART (J2-Pin5)
	  	4) IRQ06-DS P000 (CNB-Pin5)  <-------> BTN1 Pin of Pmod BTN    (J1-Pin2)
	  	5) VCC           (CNB-Pin18) <-------> VCC  Pin of Pmod BTN    (J1-Pin6)
	  	6) GND           (CNB-Pin20) <-------> GND  Pin of Pmod BTN    (J1-Pin5)
            	Use BTN1 of Pmod BTN as the user push-button S1

	For MCK-RA4T1/MCK-RA6T3:
    	  	1) Connect the Pmod BTN (J1:1-6) to the RA board via the PMOD1 Port (CN4:7-12)
             	   Use BTN1 of Pmod BTN as the user push-button S1 
	
	For FPB-RA2E3:
	  	1) P200	   	 (J3-Pin21)  <-------> IRQ0 P206	   	 (J3-Pin18)
             	   Because user button S1 (P200) does not have interrupt to cancel/end lpm mode selected.

	For MCK-RA8T1:
	  	1) Connect the Pmod BTN (J1:1-6) to the RA board via the PMOD(CN6:1)
	           Use BTN1 of Pmod BTN as the user push-button S1.

	For FPB-RA0E1:
	  	To use J-Link VCOM:
	  	1) Short the E4 trace.
	  	2) Replace R10 by 150ohm resistor.

4. The hardware modifications required for measuring the MCU's current.
	For EK-RA2A1, EK-RA4M1:
	  	1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
 
	For EK-RA6M1:
	  	1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	  	2) Connect a jumper wire from J2-Pin5(P415) to J4-Pin12(P206/IRQ0-DS). 
	     	   Since the user push-button S1 used P415/IRQ8, it won't cancel Deep Software Standby mode.
	    	   This step will enable P206/IRQ0-DS as the canceling source for all of LPM modes.
		Note:
		To get more accurate MCU's current (Icc) on EK-RA6M1, you should cut traces between E10, E50 and E52 
		to isolate the Icc from the Analog power supply current AIcc.

	For EK-RA6M2:
	  	1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of E6.
	  	2) Connect a jumper wire from J1-Pin37(P105) to J1-Pin3(P206/IRQ0-DS). 
	     	   Since the user push-button S1 used P105/IRQ0, it won't cancel Deep Software Standby mode.
	           This step will enable P206/IRQ0-DS as the canceling source for all of LPM modes.

	For EK-RA4W1:
	  	1) Cut the trace connected between pin1 (+3V3) and pin3 (+3V3_MCU) of SS20.
   
	        To measure MCU's current (Icc) on EK-RA2A1, EK-RA4M1, EK-RA6M1, EK-RA6M2 kits directly, 
	        using a Digital Multi-meter connecting between pin1 and pin3 of E6 and on EK-RA4W1 measure
	        current between pin1 and pin2 of J7; This is the recommended method.

	        You can also measure and calculate the Icc by capturing the voltage drops across a resistor from 
	        5 ohm to 20 ohm connected between pin1 and pin3 of E6.

	For EK-RA2L1, EK-RA2E1, EK-RA2E2, EK-RA4M2, EK-RA4M3, EK-RA6M3, EK-RA6M3G, EK-RA6M4, EK-RA6M5, EK-RA8M1, EK-RA8D1:
	  	1) Remove R2.(Not applicable to EK-RA2L1, EK-RA2E1, EK-RA2E2)
	  	2) Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	  	3) To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the 
	           kit connectors.

	For FPB-RA6E1, FPB-RA4E1:
	  	1) Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	  	2) To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins (CN1) 
	           on the kit connectors.

	For RSSK-RA6T1:
	  	Not applicable

	For MCK-RA6T2:
	  	Not applicable

	For FPB-RA2E3:
	  	1) Measure the voltage drops across R3 and calculate MCU's current (Icc); replace R3 by a bigger resistor if needed.
	  	2) To measure MCU's current (Icc) directly, remove R3 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the 
	           kit connectors.

	For MCK-RA8T1:
	  	Not applicable

	For FPB-RA0E1:
	  	Not applicable

        For FPB-RA8E1:
	  	1) Measure the voltage drops across R2 and calculate MCU's current (Icc); replace R2 by a bigger resistor if needed.
	  	2) To measure MCU's current (Icc) directly, remove R2 resistor and connect Multi-meter between the +3V3 and +3V3_MCU pins on the 
	           kit connectors.

5. The steps to run LPM Example Projects are as follows:
	1) Download the example project to the RA board using the USB Debugport. J-Link may be disconnected when MCU enter LPM mode. Recommend to stop
	   debugging after downloading EP.
	2) Connect the PMOD USBUART module to the RA board according to the hardware connection instructions, and then connect the Pmod USBUART to the 
           host PC via a micro-USB cable. Skip if the RA board supports J-Link VCOM functionality.
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
	5) Press the user push-button or wait 10 seconds to enter and cancel LPM mode. It will turn off LED1 before entering each LPM mode 
	   and turn on LED1 after exiting each LPM mode.
	6) The LPM mode name will display on the terminal application before entering each LPM mode, and the canceling source name will display 
	   on the terminal application after exiting each LPM mode.
	
	Transition sequences: 
	1. Normal → Sleep → Normal.									(Support all MCU lines)
	2. Normal → Deep Sleep → Normal.								(Support for RA8 MCU lines)
	3. Normal → SW Standby → Normal.								(Support all MCU lines)
	4. Normal → SW Standby → Snooze → SW Standby → Snooze → Normal.					(Support some MCU lines)
	5. Normal → Deep SW Standby → Normal.								(Support some MCU lines)