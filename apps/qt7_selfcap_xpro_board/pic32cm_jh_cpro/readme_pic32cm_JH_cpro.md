---
grand_parent: Touch Applications
parent: QT7 Xplained Pro Touch Examples
title: PIC32CM JH Curiosity Pro
has_toc: false
---
![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

#  PIC32CM JH Curiosity Pro Example using QT7 Xplained Pro

Path of the application within the repository is **apps/qt7_selfcap_xpro_board/pic32cm_jh_cpro/**

To build the application, refer to the following table and open the project using its IDE.

## Hardware Setup

1. Connect [QT7 Xplained Pro Extension Kit](https://www.microchip.com/DevelopmentTools/ProductDetails/ATQT7-XPRO) to [PIC32CM JH Curiosity Pro Evaluation](https://www.microchip.com/en-us/development-tool/EV80P12A_TODO) on extension header EXT1
   
2. Connect the Debug USB port on the board to the computer using a micro USB cable

## Running The Application

1. Open the application project */firmware/pic32cmjh_curiosity_pro.X* in the IDE
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
