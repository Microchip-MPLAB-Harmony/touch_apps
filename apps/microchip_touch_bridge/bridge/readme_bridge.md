---
grand_parent: Touch Applications
parent: Microchip Touch Bridge
title: Touch Bridge Application
has_children: false
has_toc: false
nav_order: 5
---

![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip Touch Bridge Application
The default application has bootloader and bridge firmware combined. Since a valid application is present, the control does not stay in bootloader. By modifying the switch position(Refer to [Mode Selection](../readme.md#Mode-Selection)), user can force to stay in bootloader mode.

[Bootloader Readme](readme_bootloader.md)

## Application Overview
The figure below represents important blocks of the Bridge application.
![Application Overview](images/appOverivew.png)

### pcComLayer
This layer has two circular buffers: one to transmit data to the PC and another one to receive data from the PC.
The circular buffers are accessed by a set of APIs which can be used to write or read from both Transmit buffers. pcComm layer is not used in CAPBridge mode.

pcComm layer is accessed by:
* USB/BLE layer to:
  * Write data to RX buffer
  * Read data to TX buffer
* Touch process (or Bypass mode) to:
  * Write data to TX buffer
  * Read data to RX buffer

TX and RX terms are related to MCU.
![PC Comm Layer](images/pcCommLayer.png)

### Process Layer
This layer changes dynamically depending on the mode. All the processing related to data is implemented in this layer.
For example, in UART Bypass mode, this layer simply copies data from UART to pcComm layer and vice-versa.
In CAPBridge mode, this layer processes the commands received from CAP GUI and sends corresponding I2C commands to CAP devices.
If the bridge board is connected to a touch turnkey device (exemplary AT42QT2120), the memory map of the turnkey device is kept in this layer. It also contains the actual driver for the target device. In addition to that, this layer also contains the driver that talks to MPLAB Data Visualizer Touch Plug-in. This layer helps in receiving the data from the
turnkey device and repack the data prior to transmitting it to MPLAP Data Visualizer. Similarly, if the data or settings are modified by user in Touch Plug-in (MPLAB DV GUI) they are sent to the target device through this layer.
The image below shows some examples of Process Layer. The possible combinations are not limited.
![Process Layer](images/processLayer.png)

### USB
The entire application related to the USB is implemented in [usbApp.c](../bridge/firmware/src/usbApp.c) file.
* In CDC mode:
  * Copies data from the pcCOMM layer to USB peripheral and the following steps are performed:
    * Checking for new data to be transmitted
    * If new data is available in pcCOMM, that data is sent to PC.
    * Copies data from USB peripheral to pcCOMM buffer and the following steps are performed:
    * Checking for new data from PC
    * If new data is received from PC, that data is copied to the pcCOMM buffer
* In CAPBridge mode:
  * USB directly interacts with I2C drivers to read/write data to the CAP devices. The address, read/write instructions from CAP GUI are directly routed to I2C drivers. This part is handled in [capBridge.c](../bridge/firmware/src/capBridge.c) files.

Other than data transaction, the handling of USB connect/disconnect, COMPORT open/close etc are also handled in this layer.

### Bluetooth
The entire application related to Bluetooth is implemented in [btApp.c](../bridge/firmware/src/btApp.c) file.
* Sends data from the pcCOMM layer to the RN4678 module
* Checks for new data to be transmitted
* If new data is available in pcCOMM, then it sends the data to the PC.
* Copies data from the RN4678 module to the pcCOMM buffer
* Checks for new data from the PC
* If new data is received from the PC, it copies the data to pcCOMM buffer Other than data transaction, the handling of COMPORT open/close etc. are also handled in this layer.

### UART, I2C, SPI
The blocks UART, I2C, SPI provide wrapper functionality to the Harmony 3 drives for easy communication with other layers.

### LED
There are four LEDs present in the board:
* Blue
* Red
* Green
* Yellow
The firmware provides standard APIs to perform blink operation on Red, Green and Yellow LEDs. Blue LED is directly controlled by Bluetooth.

### Time
Provides periodic interrupt for time reference and delays needed by firmware. This proves useful if debug data from Touch turnkey parts needs to be read periodically.

## Firmware Components

### USB
USB can enumerate in two interfaces either CDC or CAPBridge. The descriptors are stored in two different files and depend on the 4-way switch (Refer to article [Introduction to Microchip Touch Bridge](https://microchipdeveloper.com/touch:introduction-to-microchip-touch-bridge)).
File usb_device_init_data_cdc.c contains the descriptors for CDC Interface and file usb_device_init_data_capBridge contains the descriptor details for CAP Bridge interface.
Standard Harmony 3 USB CDC drivers are used for CDC configuration and USB numeration. For CapBridge configuration, vendor class is used and vendor/product IDs are hard coded to match with the [CAP1xxx Touch Tuning GUI](https://www.microchip.com/en-us/products/touch-and-gesture/touch-development-tools).
The code related to USB CDC and CAPBridge is present in [usbApp.c](../bridge/firmware/src/usbApp.c) file.

### Bluetooth
Standard UART interface at 115200 baud rate is used to talk to RN4678 Bluetooth module. Flow control is not used in default firmware.

### Touch device interfaces
Standard driver function from Harmony 3 is used for interfaces like I2C, SPI, UART, GPIO on the header side.

