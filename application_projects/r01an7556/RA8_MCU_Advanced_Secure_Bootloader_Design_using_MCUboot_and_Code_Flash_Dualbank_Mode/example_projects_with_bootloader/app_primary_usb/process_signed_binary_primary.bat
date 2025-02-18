srec_cat Debug\app_primary_usb.bin.signed -binary -offset 0x2010000 -execution-start-address 0x2010000 -o app_primary_usb_signed_offset.srec

srec_cat ..\ra_mcuboot_ra8m1_dualbank\ra_mcuboot_ra8m1_dualbank_offset.srec ..\ra_mcuboot_ra8m1_dualbank\Debug\ra_mcuboot_ra8m1_dualbank.srec app_primary_usb_signed_offset.srec -o combined.srec
