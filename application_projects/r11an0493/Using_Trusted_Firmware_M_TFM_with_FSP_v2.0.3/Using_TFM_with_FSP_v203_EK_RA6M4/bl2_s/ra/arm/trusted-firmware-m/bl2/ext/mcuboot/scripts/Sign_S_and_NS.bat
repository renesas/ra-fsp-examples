rem update to the path of the tfm secure and non-secure .elf files. The .bin are output location for the batch file to write to.
set elffile_s="C:\Work\RA\FSP_2_0_3_TOE_Demo\tfm_s\Debug\tfm_s.elf"
set binfile_s="C:\Work\RA\FSP_2_0_3_TOE_Demo\tfm_s\Debug\tfm_s.bin"
set elffile_ns="C:\Work\RA\FSP_2_0_3_TOE_Demo\tfm_ns\Debug\tfm_ns.elf"
set binfile_ns="C:\Work\RA\FSP_2_0_3_TOE_Demo\tfm_ns\Debug\tfm_ns.bin"
rem set the signing key to be used. Note that the counterpart public key must be embedded into the bootloader.
rem For RSA2048, use the demo key at ra/arm/trusted-firmware-m/bl2/ext/mcuboot/root-rsa-2048.pem
set key="C:\Work\RA\FSP_2_0_3_TOE_Demo\bl2_s\ra\arm\trusted-firmware-m\bl2\ext\mcuboot\root-rsa-3072.pem"
rem flash_layout_s is set to an empty file since the script only requires it to perform setting unused areas to 0 which we are skipping for now.
set flash_layout_s="C:\Work\RA\FSP_2_0_3_TOE_Demo\bl2_s\ra\arm\trusted-firmware-m\bl2\ext\mcuboot\scripts\flash_layout_s.c"
rem set the version numbers for the primary and secondary images.
set version=1.0.1+0
set version1=1.0.1+1
set dependencies="(1,1.0.0+0)"
rem update the python_path variable to the python path on your workstation.
set python_path="C:\Users\b3800274\AppData\Local\Programs\Python\Python38-32"
rem update the gcc_path variable to the toolchain path on your workstation.
set gcc_path="c:\Program Files (x86)\GNU Tools ARM Embedded\9 2019-q4-major\bin"
rem convert the input elf files to bin files
%gcc_path%\arm-none-eabi-objcopy.exe -O binary %elffile_s% %binfile_s%
%gcc_path%\arm-none-eabi-objcopy.exe -O binary %elffile_ns% %binfile_ns%
rem truncate the output bin files so that they fit within the partitions. trunc.exe or equivalent can be used.
rem trunc.exe %binfile_s% 227328
rem trunc.exe %binfile_ns% 227328
py -3 file_truncate.py -i %binfile_s% -l 227328
py -3 file_truncate.py -i %binfile_ns% -l 227328
rem sign the secure image and an update for it with a postfix of _imageand _image1 respectively using the bin file created earlier. Here we are using the same bin file for both the image and the update with different version numbers.
py -3 imgtool.py sign --layout %flash_layout_s% -k %key% --public-key-format hash --align 1 -v %version% -d %dependencies% -s 42 -H 0x400 %binfile_s% %binfile_s:~0,-5%_image.bin
py -3 imgtool.py sign --layout %flash_layout_s% -k %key% --public-key-format hash --align 1 -v %version1% -d %dependencies% -s 42 -H 0x400 %binfile_s% %binfile_s:~0,-5%_image1.bin
rem sign the non-secure image and an update for it with a postfix of _image and _image1 respectively.
py -3 imgtool.py sign --layout %flash_layout_s% -k %key% --public-key-format hash --align 1 -v %version% -d %dependencies% -s 42 -H 0x400 %binfile_ns% %binfile_ns:~0,-5%_image.bin
py -3 imgtool.py sign --layout %flash_layout_s% -k %key% --public-key-format hash --align 1 -v %version1% -d %dependencies% -s 42 -H 0x400 %binfile_ns% %binfile_ns:~0,-5%_image1.bin