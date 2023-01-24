---
grand_parent: Touch Applications
parent: T9 Xplained Pro Touch Examples
title: SAML10 Xplained Pro
has_toc: false
---

[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

# SAML10 Xplained Sensor Example using T9 Xplained Pro Self

Path of the application within the repository is **apps/t9_selfcap_xpro_board/sam_l10_xpro/**

To build the application, refer to the following table and open the project using its IDE.

## Touch Application

| Project Name                                      | Description                                                                                                         |
| ------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| t9_selfcap_xpro_board/sam_l10_xpro/sam_l10_xpro.X | MPLABX Project for [SAM L10 Xplained Pro Evaluation Kit](https://www.microchip.com/en-us/development-tool/DM320204) |
|                                                   |                                                                                                                     |

## Setting up the hardware

1. Connect [T9 Xplained Pro Extension Kit](https://www.microchip.com/en-us/development-tool/AC89D55A) to [SAM L10 Xplained Pro Evaluation Kit](https://www.microchip.com/en-us/development-tool/DM320204) on extension header EXT1
2. Connect the Debug USB port on the board to the computer using a micro USB cable

## Running the Application

1. Open the application project _sam_l10_xpro.X_ in the IDE
2. Build the project and program the target board

## Monitoring Touch Data in MPLAB DV Touch plugin

**Prerequisite**
MPLAB Data Visualizer and Touch Pluign. Procedure for Installation is available in [Introduction to Touch Plugin](https://microchipdeveloper.com/touch:introduction-to-touch-plugin).

1. Configure COM Port
   - Select the correct COM port in **Connection** tab
   - Update the **Baud Rate** to 115200 in **COM Settings**
   - Start **Streaming** for COM data
2. COM Port on Touch
   - Select Touch in **Connection** tab
   - For data source, from drop-down menu, select configured COM port as source
   - Start **Streaming** on Touch

Detailed procedure can be found in [Introduction to Touch Plugin](https://microchipdeveloper.com/touch:introduction-to-touch-plugin) page.
