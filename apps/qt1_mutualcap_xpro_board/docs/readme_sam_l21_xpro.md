---
grand_parent: Touch Applications
parent: QT1 Xplained Pro Mutual Capacitance Examples
title: SAML21 Xplained Pro
has_toc: false
---
[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

#  SAML21 Xplained Sensor Example using QT1 Xplained Pro Mutual 

Path of the application within the repository is **apps/qt1_mutualcap_xpro_board/firmware/**

To build the application, refer to the following table and open the project using its IDE.

## Touch Application

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| qt1_mutualcap_xpro_board/firmware/sam_l21_xpro.X    | MPLABX Project for [SAM L21 Xplained Pro Evaluation Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/ATSAML21-XPRO-B)|
|||

## Setting up the hardware
1. Connect the QT1 Xplained pro Mutual capacitance to the Xplained Board
2. Connect the Debug USB port on the board to the computer using a micro USB cable


## Running the Application

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
