<div align="center">
 
 # Smart Power Case for FatShark, Skyzone, DJI fpv goggles
 
 [![Build Status](https://travis-ci.com/cvetaevvitaliy/Smart-Power-Case.svg?branch=master)](https://travis-ci.com/github/cvetaevvitaliy/Smart-Power-Case)
 [![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/cvetaevvitaliy/Smart-Power-Case/graphs/contributors)
 
</div>

Smart Power Case for FatShark goggles <br>
Powered by a single Li-ion, stable output 8.2V or 12.6V (switching from menu)<br>
Support QC 2.0/3.0, FastCharge charges 5V, 9V, 12V - max input 18W (BQ25895) <br>
Shows operating time, remaining battery capacity, remaining charging time (BQ27441)<br>
<br>

## How to build 

###### Depends
You need install `gcc-arm-none-eabi` compiler and CMake  <br>
for Ubuntu <br>
 `sudo apt-get install gcc-arm-none-eabi cmake` <br>
for MacOS <br>
 `brew install gcc-arm-none-eabi cmake`
 
###### How to build project for STM32F103TBU
```
    git clone https://github.com/cvetaevvitaliy/Smart-Power-Case
    cd Smart-Power-Case
    mkdir build
    cd build
    cmake ..
    make -j8
```
default build for MCU STM32F103TBU (128kb flash ), if you need change type MCU - STM32F103T8U (64kb flash), please use option `-DTYPE_MCU=F103TBU` <br>
P.S. if you run `cmake ..` and want to use  `-DTYPE_MCU=F103T8U` You have two steps: <br>
First: you need delete build dir <br>
Second: remove `CMakeCache.txt` use `rm CMakeCache.txt` in build directory <br>

###### How to build project for STM32F103T8U
```$xslt
    git clone https://github.com/cvetaevvitaliy/Smart-Power-Case
    cd Smart-Power-Case
    mkdir build
    cd build
    cmake -DTYPE_MCU=F103T8U ..
    make -j8
```

Or use Clion IDE and load CMakeLists.txt file <br>


After a successful build, such files will be created<br>
:beer:   `firmware-x.x.x.bin` update for flash GUI flasher<br>
:beer:   `FullFW.bin` for flash via SWD flasher ST-link or Jlink<br>
:beers:   `FullFW.bin` has bootloader, firmware and default eeprom data<br>
<br>

#### Flash memory map for STM32F103TBU (128kb)
| 0x8000000 | 0x8000800 | 0x801FC00 |                   
|:----------------:|:---------:|:-----------------:|
| bootloader | firmware | eeprom |
<br>

#### Flash memory map for STM32F103T8U (64kb)

| 0x8000000 | 0x8000800 | 0x800FC00 |
|:----------------:|:---------:|:-----------------:|
| bootloader | firmware | eeprom |

<br>
:white_check_mark: Since the data is not saved often, it made no sense to use an external eeprom


another options: `-DDEBUG=ON` for debug firmware via USB virtual comport, speed 115200, you can use `printf` and send out to console

*update project, added  support fw update from bootloader, use HID bootloader (https://github.com/cvetaevvitaliy/STM32_HID_Bootloader)<br>
*update project, added cross-platform command line flasher (https://github.com/cvetaevvitaliy/Flasher_USB_HID)<br> <br>
*update project, added cross-platform GUI flasher (https://github.com/cvetaevvitaliy/Smart-Power-Case-GUIFlasher)
<img width="224" alt="Linux" src="https://user-images.githubusercontent.com/26421560/84379887-33094380-abef-11ea-9d6c-4b74dc4eadb8.png">
<img width="224" alt="MacOS" src="https://user-images.githubusercontent.com/26421560/84379892-343a7080-abef-11ea-957d-4b48acbeb280.png">
<img width="224" alt="Win" src="https://user-images.githubusercontent.com/26421560/84379972-53d19900-abef-11ea-9d15-2edbee6894fe.png">

YouTube link <br>https://www.youtube.com/watch?v=Y0oo7hjzqtc <br>
https://www.youtube.com/watch?v=4TmXY8NL-vo


<br><br> Download [Schematic.pdf](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/schematic.pdf)
<br>
Download [Gerber X2.zip](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/GerberX2_V2_2.zip)

Download STL for 3D printing <br>
MID https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/STL_File/mid.STL<br>
TOP https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/STL_File/top.STL<br>

![BOT](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/IMG_5686.jpeg)

![TOP](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/2020-05-30_23-37-36.png)

![BOT](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/2020-05-30_23-38-08.png)

![Altium](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/2020-05-30_23-38-28.png)


![BOT](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/IMG_5678.jpeg)

![BOT](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/IMG_5679.jpeg)

![BOT](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/IMG_5680.jpeg)

![BOT](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/IMG_5682.jpeg)

![SolidWorks](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/2020-05-31_0-51-28.png)
![SolidWorks](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/2020-05-31_0-50-00.png)
![SolidWorks](https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/2020-05-31_0-50-51.png)

See 3D model in the browser <br>
MID https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/STL_File/mid.STL<br>
TOP https://github.com/cvetaevvitaliy/Smart-Power-Case/blob/master/HW/STL_File/top.STL<br>
