---
grand_parent: Touch Applications
parent: QT1 Xplained Pro Self Capacitance Examples
title: SAMC21 Xplained Pro
has_toc: false
---
[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

#  SAMC21 Xplained Sensor Example using QT1 Xplained Pro Self 

Path of the application within the repository is **apps/qt1_selfcap_xpro_board/firmware**

To build the application, refer to the following table and open the project using its IDE.

## Touch Application

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| qt1_selfcap_xpro_board/firmware/sam_c21_xpro.X    | MPLABX Project for [SAM C21 Xplained Pro Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNo/ATSAMC21-XPRO)|
|||

## Setting up the hardware
1. Connect the QT1 Xplained pro Self capacitance to the Xplained Board
2. Connect the Debug USB port on the board to the computer using a micro USB cable


## Running the Application

1. Open the application project */firmware/sam_c21_xpro.X* in the IDE
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
