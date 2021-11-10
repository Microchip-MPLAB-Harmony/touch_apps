---
grand_parent: Touch Applications
parent: T10 Xplained Pro Mutual Capacitance Examples
title: SAML10 Xplained Pro
has_toc: false
---
[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

#  SAML10 Xplained Sensor Example using T10 Xplained Pro Mutual 

Path of the application within the repository is **apps/t10_mutualcap_xpro_board/firmware/**

To build the application, refer to the following table and open the project using its IDE.

## Touch Application

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| t10_mutualcap_xpro_board/firmware/sam_l10_xpro.X    | MPLABX Project for [SAM L10 Xplained Pro Evaluation Kit](https://www.microchip.com/en-us/development-tool/DM320204)|
|||

## Setting up the hardware
1. Connect [T10 Xplained Pro Extension Kit](https://www.microchip.com/en-us/development-tool/AC47H23A) to [SAM L10 Xplained Pro Evaluation Kit](https://www.microchip.com/en-us/development-tool/DM320204) on extension header EXT3
2. Connect the Debug USB port on the board to the computer using a micro USB cable


## Running the Application

1. Open the application project */firmware/sam_l10_xpro.X* in the IDE
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
