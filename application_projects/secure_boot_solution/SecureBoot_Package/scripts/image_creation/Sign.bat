@echo off
rem update the path of the application project binary file in your PC.

set infile=../blinky_red/Debug/blinky_red_led.bin

set key=Signing_Key/signingkey.pem
set flash_layout=../flash_layout/flash_layout.h
set version=1.0.1+0
set dependencies="(1,1.0.0+0)"

rem update the python_path variable to the python path in your workstation.
set python_path="C:\python_3_7_4"

rem Uncomment (remove rem) from the line below to use the path set in python_path
rem set path=%path%;%python_path%

python.exe imgtool.py sign --layout %flash_layout% -k %key% --public-key-format hash --align 1 -v %version% -d %dependencies% -s 42 -H 0x400 %infile% %infile:~0,-4%_image.bin
pause
