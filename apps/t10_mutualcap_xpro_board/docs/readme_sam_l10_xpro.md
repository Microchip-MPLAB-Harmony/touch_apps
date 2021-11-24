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

## Monitoring Touch Data in MPLAB DV Touch plugin
**Prerequisite**
MPLAB Data Visualizer and Touch Pluign. Procedure for Installation is available in [Introduction to Touch Plugin](https://microchipdeveloper.com/touch:introduction-to-touch-plugin).

1. Configure COM Port
    -    Select the correct COM port in **Connection** tab
    -    Update the **Baud Rate** to 115200 in **COM Settings**
    -    Start **Streaming** for COM data
2. COM Port on Touch
    - Select Touch in **Connection** tab
    - For data source, from drop-down menu, select configured COM port as source
    - Start **Streaming** on Touch

Detailed procedure can be found in [Introduction to Touch Plugin](https://microchipdeveloper.com/touch:introduction-to-touch-plugin) page.
