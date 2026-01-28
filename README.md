# Final Project Overview

## Project Overview
https://github.com/cu-ecen-aeld/final-project-karimsharabash/wiki/Smart-Watering-System

## Schedule Page

https://github.com/users/karimsharabash/projects/1/views/1

## Sprints Page

https://github.com/cu-ecen-aeld/final-project-karimsharabash/wiki/Sprints

## Quick Start

### Build

To build the project, use the `build.sh` script and provide the build directory path.

```bash
./build.sh build-rpi
```
**Note:**
* The `conf` directory is backed up during the first build.
* After creating the build directory for the first time, copy the `conf` directory back if required.
---

### Flashing the SD Card

To flash the generated image to an SD card, use the `flash.sh` script and provide:

* The build directory
* The SD card device

```bash
./flash.sh build-rpi/ /dev/mmcblk0
```

⚠️ **Warning:**
Make sure the SD card device is correct. Using the wrong device may result in data loss.
---

### Communication

You can communicate with the system using:

* UART
* Screen and keyboard

Use whichever method is convenient.
---

### ADC Readings

To read ADC values on the Raspberry Pi, use:

```bash
adc /dev/spidev0.x <channel>
```

Replace `<channel>` with the required ADC channel number.

---

### Pump Control

To control the pump:

```bash
pump on
pump off
```
---

### Automation

To automate the full process, run:

```bash
sws_controller
```
---
