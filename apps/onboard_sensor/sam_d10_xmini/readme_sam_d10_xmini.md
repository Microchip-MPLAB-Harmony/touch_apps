---
grand_parent: Touch Applications
parent: Onboard Sensor Examples
title: SAMD10 Xplained Mini
has_toc: false
---

![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)

# SAMD10 Xplained Mini Onboard Sensor Example

Path of the application within the repository is **apps/onboard_sensor/firmware/sam_d10_xmini/**

To build the application, refer to the following table and open the project using its IDE.

## Hardware Setup

1. Project uses [SAM D10 Xplained Mini Evaluation Kit](https://www.microchip.com/developmenttools/ProductDetails/ATSAMD10-Xmini)
2. Connect the Debug USB port on the board to the computer using a micro USB cable

## Running The Application

1. Open the application project _sam_d10_xmini.X_ in the IDE
2. Build the project and program the target board
3. Open [Data Visualizer](https://microchipdeveloper.com/mplabx:datavisualizer) and configure the serial port control settings as follows:
   - Baud : 38400
   - Data : 8 Bits
   - Parity : None
   - Stop : 1 Bit
   - Flow Control : None
   - Config Path :
4. Select the Port from the dropdown menu and click "Connect"
5. Configuration files are stored as part of the project source files. Browse the ../touch/datastreamer/ project folder and click on Select Folder.
