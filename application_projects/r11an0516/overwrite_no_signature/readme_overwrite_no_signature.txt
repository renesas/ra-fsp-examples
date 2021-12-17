Steps to run the example project in this folder:

1. Import projects to a workspace
2. Open the configuration.xml file from project ra_mcuboot_ra2e1
3. Click Generate Project Content
4. Overwrite bootutil.h and image.h (located in \overwrite_no_signature\ra_mcuboot_ra2e1\ra\mcu-tools\MCUboot\boot\bootutil\include\bootutil) with the two files bootutil.h and image.h included in this folder.
5. Compile the project ra_mcuboot_ra2e1
6. Open the configuration.xml file from project blinky
7. Click Generate Project Content
8. Compile the blinky project
9. Open the configuration.xml file from project blinky_new
10. Click Generate Project Content
11. Compile the blinky_new project
12. Debug the application from project blinky
13. Pause the debugging session 
14. Download the blinky.bin.signed to address 0x2000
15. Resume the program execution, all leds should be clinking
16. Pause the execution
17. Download the blinky_new.bin.signed to address 0x4000
18. Resume the program execution, the blue led should be blinking
