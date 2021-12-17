Steps to run the example project in this folder:

1. Import projects to a workspace
2. Open the configuration.xml file from project ra_mcuboot_ra2e1_overwrite_with_signature
3. Click Generate Project Content
4. Overwrite bootutil.h and image.h (located in \overwrite_with_signature\ra_mcuboot_ra2e1_overwrite_with_signature\ra\mcu-tools\MCUboot\boot\bootutil\include\bootutil) with the two files bootutil.h and image.h included in this folder.
5. Compile the project ra_mcuboot_ra2e1_overwrite_with_signature
6. Open the configuration.xml file from project blinky_with_signature
7. Click Generate Project Content
8. Compile the blinky_with_signature project
9. Open the configuration.xml file from project blinky_with_signature_new
10. Click Generate Project Content
11. Compile the blinky_with_signature_new
12. Debug the application from project blinky_with_signature
13. Pause the debugging session 
14. Download the blinky_with_signature.bin.signed to address 0x3800
15. Resume the program execution, all leds should be clinking
16. Pause the execution
17. Download the blinky_with_signature_new.bin.signed to address 0x5800
18. Resume the program execution, the blue led should be blinking
