Steps to run the example project in this folder:

1. Import projects to a workspace
2. Open the configuration.xml file from project ra_mcuboot_ra2e1_swap_with_signature
3. Click Generate Project Content
4. Overwrite bootutil.h and image.h (located in \swap_with_signature\ra_mcuboot_ra2e1_swap_with_signature\ra\mcu-tools\MCUboot\boot\bootutil\include\bootutil) with the two files bootutil.h and image.h included in this folder.
5. Compile the project ra_mcuboot_ra2e1_swap_with_signature
6. Open the configuration.xml file from project blinky_swap_with_signature
7. Click Generate Project Content
8. Compile the blinky_swap_with_signature project
9. Open the configuration.xml file from project blinky_swap_with_signature_new
10. Open the Properties view of the MCUboot Image Utilties stack, update the path to mcuboot_config.h, mcuboot_logging.h and sysflash.h.
11. Click Generate Project Content
12. Compile the blinky_swap_with_signature_new project
13. Debug the application from project blinky_swap_with_signature
14. Pause the debugging session 
15. Download the blinky_swap_with_signature.bin.signed to address 0x4000
16. Resume the program execution, all leds should be clinking
17. Pause the execution
18. Download the blinky_swap_with_signature_new.bin.signed to address 0x6800
19. Resume the program execution, the blue led should be blinking
20. Reset the program execution from e2studio 
21. Run the application, the blue led should be blinking
