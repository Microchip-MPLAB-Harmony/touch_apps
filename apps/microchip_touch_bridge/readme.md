---
parent: Touch Applications
title: Microchip Touch Bridge
has_children: true
has_toc: false
nav_order: 5
technology: touch
---

![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip Touch Bridge
The default application has bootloader and bridge firmware combined. Since a valid application is present, the control does not stay in bootloader. By modifying the switch position, user can force to stay in bootloader mode.

| Applications Details |
| --- |
|[Bootloader](docs/readme_bootloader.md) |
|[Bridge application](docs/readme_bridge.md) |

## Mode Selection
The Application mode can be selected by using the switch settings (Mode Selector switch). Change the switch settings as needed and power up the board.

In Test Mode, Red, Green and Yellow glows continously and data transaction does not work. Currently the Bridge firmware support only UART Bypass, CAPBridge and Bootloader modes. Support for turnkey deviecs will be added in future updates.

| SW4 | SW3 | SW2 | SW1 | Mode | USB COM | USB MSD | Bluetooth | USB CAPBridge |
| :---: | :---: | :---: | :---: | ----------- | :---: | :---: | :---: | :---: |
| 0 | 0 | 0 | 0 | UART Bypass 9600 | Yes | No | No | No|
| 0 | 0 | 0 | 1 | UART Bypass 19200 | Yes | No | No| No|
| 0 | 0 | 1 | 0 | UART Bypass 38400 | Yes | No | No| No|
| 0 | 0 | 1 | 1 | UART Bypass 115200 | Yes | No | No| No|
| 0 | 1 | 0 | 0 | UART Bypass 9600 | No |  No| Yes | No|
| 0 | 1 | 0 | 1 | UART Bypass 19200 | No |  No| Yes | No|
| 0 | 1 | 1 | 0 | UART Bypass 38400 | No |  No| Yes | No|
| 0 | 1 | 1 | 1 | UART Bypass 115200 | No |  No| Yes | No|
| 1 | 0 | 0 | 0 | Reserved | No | No | No| No|
| 1 | 0 | 0 | 1 | Reserved | No | No | No| No|
| 1 | 0 | 1 | 0 | Reserved | No | No | No| No|
| 1 | 0 | 1 | 1 | Reserved | No | No | No| No|
| 1 | 1 | 0 | 0 | Reserved | No | No | No| No|
| 1 | 1 | 0 | 1 | Test Mode | No | No | No| No|
| 1 | 1 | 1 | 0 | CAP Bridge | No | No | No| Yes|
| 1 | 1 | 1 | 1 | Bootloader | No | Yes | No| No|

More information can be found in [Introduction to Microchip Touch Bridge](https://microchipdeveloper.com/touch:introduction-to-microchip-touch-bridge) article.
