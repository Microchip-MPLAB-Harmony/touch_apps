---
grand_parent: Touch Applications
parent: Onboard Sensor Examples
title: SAME54 Xplained Pro
has_toc: false
---

![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)

# SAME54 Xplained Pro Onboard Sensor Example

Path of the application within the repository is **apps/onboard_sensor/sam_e54_xpro/**

To build the application, refer to the following table and open the project using its IDE.

## Hardware Setup

1. Project uses [SAM E54 Xplained Pro Evaluation Kit](https://www.microchip.com/developmenttools/productdetails/atsame54-xpro)
2. Connect the Debug USB port on the board to the computer using a micro USB cable

## Running The Application

1. Open the application project _sam_e54_xpro.X_ in the IDE
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
