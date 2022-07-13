---
grand_parent: Touch Applications
parent: T10 Xplained Pro Mutual Capacitance Examples
title: PIC32CM LE Curiosity Pro
has_toc: false
---
[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

#  PIC32CM LE Curiosity Pro Sensor Example using T10 Xplained Pro Mutual 

Path of the application within the repository is **apps/t10_mutualcap_xpro_board/pic32cm_le00_cpro/**

To build the application, refer to the following table and open the project using its IDE.

## Touch Application

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| t10_mutualcap_xpro_board/pic32cm_LE00_cpro/pic32cm_LE00_cpro.X    | MPLABX Project for [PIC32CM LE Curiosity Pro Evaluation Kit](https://www.microchip.com/en-us/development-tool/EV80P12A)|
|||

## Setting up the hardware
1. Connect [T10 Xplained Pro Extension Kit](https://www.microchip.com/en-us/development-tool/AC47H23A) to [PIC32CM LE Curiosity Pro Evaluation Kit](https://www.microchip.com/en-us/development-tool/EV80P12A) on extension header EXT1 & EXT2
2. Connect the Debug USB port on the board to the computer using a micro USB cable


## Running the Application

1. Open the application project *pic32cm_LE00_cpro.X* in the IDE
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