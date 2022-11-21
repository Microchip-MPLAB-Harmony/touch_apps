---
grand_parent: Touch Applications
parent: QT7 Xplained Pro Touch Examples
title: PIC32CZ CA Curiosity Ultra
has_toc: false
---
![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

#  PIC32CZ CA Curiosity Ultra Example using QT7 Xplained Pro

Path of the application within the repository is **apps/qt7_selfcap_xpro_board/pic32czca_cultra/**

To build the application, refer to the following table and open the project using its IDE. The project configures only one button of QT7 with the shield capability.

## Hardware Setup

1. Connect [QT7 Xplained Pro Extension Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/ATQT7-XPRO) to PIC32CZ CA Curiosity Ultra Evaluation Kit on extension header EXT1 such that pin 1 of QT7 is connected to pin 3 of Curiosity board.
   
2. Connect the Debug USB port on the board to the computer using a micro USB cable

## Running The Application

1. Open the application project *pic32czca_curiosity_qt7.X* in the IDE
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
