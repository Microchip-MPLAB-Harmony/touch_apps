---
grand_parent: Touch Applications
parent: Onboard Sensor Examples
title: PIC32CMJH CURIOSITY Pro
has_toc: false
---

![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)

# PIC32CM JH01 CURIOSITY Pro Onboard Sensor Example

## Downloading and building the application

Path of the application within the repository is **apps/onboard_sensor/pic32cm_jh_cpro/**

To build the application, refer to the following table and open the project using its IDE.

## Hardware Setup

1. Project uses PIC32CM JH01 Curiosity Pro Evaluation Kit
2. Connect the Debug USB port on the board to the computer using a micro USB cable

## Running The Application

1. Open the application project _pic32cmjh_curiosity_pro.X_ in the IDE
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
