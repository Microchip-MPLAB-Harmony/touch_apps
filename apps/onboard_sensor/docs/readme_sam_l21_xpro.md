---
grand_parent: Touch Applications
parent: Onboard Single Touch Button Examples
title: SAML21 Xplained Pro
has_toc: false
---
![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

#  SAML21 Xplained Pro Onboard Sensor Example

Path of the application within the repository is **apps/onboard_sensor/firmware/**

To build the application, refer to the following table and open the project using its IDE.

## Hardware Setup

1. Project uses [SAM L21 Xplained Pro Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/ATSAML21-XPRO-B)
   
2. Connect the Debug USB port on the board to the computer using a micro USB cable

## Running The Application

1. Open the application project */firmware/sam_l21_xpro.X* in the IDE
2. Build the project and program the target board
3. Open [Data Visualizer](https://microchipdeveloper.com/mplabx:datavisualizer) and configure the serial port control settings as follows:
    - Baud : 115200
    - Data : 8 Bits
    - Parity : None
    - Stop : 1 Bit
    - Flow Control : None
    - Config Path : 
4.    Select the Port from the dropdown menu and click "Connect"
5.    Configuration files are stored as part of the project source files. Browse the ../touch/datastreamer/ project folder and click on Select Folder.
