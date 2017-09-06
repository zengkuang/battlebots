# RM2018 CLion Toolchain 
This project is built for cross-platform built on Mac and Windows on STM32. Target board is chosen for RM2017 dev board. 

## Hardware
1. RM2017 dev board
![image of RM2017 board](http://product1.djicdn.com/uploads/photos/760/medium_2c39c864-899e-42da-a75c-38668c16b793.jpg)
http://store.dji.com/cn/product/rm-main-controller-development-board

2. JLink SWD / JLink v8

## Windows
Using Keil uVision 5, open MDK-ARM/RMv3testDemo.uvprojx

## Mac
You need to set up the toolchain yourself.
* CLion
* gcc-arm-embedded
  for gcc-arm-embedded toolchain, you may followed the pixhawk installation instruction at 
  https://dev.px4.io/en/setup/dev_env_mac.html
  ```
  brew tap PX4/px4
  brew install px4-dev
  ```
  After set up, import the toolchain and build with `RM2018_CLion_Toolchain.elf`. If succeed, this will pop
  ```
  /Users/beck/Developer/RM2018_CLion_Toolchain/cmake-build-debug/RM2018_CLion_Toolchain.elf
  
  Process finished with exit code 127
  ```
  
* JLink Debugger - Ozone
  https://www.segger.com/downloads/jlink/
  
  A Jlink debugger file is already created for import at ~/RM2018_CLion_Toolchain/JLinkDebugger, but if you prefer setup yourself, here is the instructions:
  
  In the *New Project Wizard*, select *Target Device Settings* to STM32F427II.
  
  <img src="https://github.com/robomasterhkust/RM2018_CLion_Toolchain/blob/master/Hardware-Datasheet/Screen%20Shot%202017-09-05%20at%2011.20.15%20AM.png" width="308" height="240"><img src="https://github.com/robomasterhkust/RM2018_CLion_Toolchain/blob/master/Hardware-Datasheet/Screen%20Shot%202017-09-05%20at%2011.20.31%20AM.png" width="308" height="240">
  
* In the Program file Choose the elf file for better debugging `~/cmake-build-debug/RM2018_CLion_Toolchain.elf` 
  
  <img src="https://github.com/robomasterhkust/RM2018_CLion_Toolchain/blob/master/Hardware-Datasheet/Screen%20Shot%202017-09-06%20at%202.17.37%20PM.png" width="308" height="240">
  
  
* Details of the steps to initialize JLink Debugger are on the blog https://blog.jetbrains.com/clion/2016/06/clion-for-embedded-development/

The CMake.txt and cross-compile cmake file are inside the folder.
