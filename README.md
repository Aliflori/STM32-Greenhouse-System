# 🌱 STM32 Smart Greenhouse Control System
![MCU](https://img.shields.io/badge/MCU-STM32-0082FC?style=for-the-badge&logo=stmicroelectronics&logoColor=white)
![Language](https://img.shields.io/badge/Language-C-A8B9CC?style=for-the-badge&logo=c&logoColor=white)
![Library](https://img.shields.io/badge/Library-STM32%20HAL-orange?style=for-the-badge)
![Simulation](https://img.shields.io/badge/Simulation-Proteus-1B5082?style=for-the-badge)

This repository contains a complete embedded system project for a **Smart Greenhouse Monitoring and Control System**. The system is built around an **STM32F1 series microcontroller** and is designed to automate the environmental control of a greenhouse, ensuring optimal conditions for plant growth while providing remote alerting capabilities. The entire project, including all hardware components and their interactions, was simulated using **Proteus Design Suite**.

This was developed as the final project for a Microprocessor Systems course.

## ✨ System Features

The system integrates a variety of sensors and actuators to create a fully autonomous environment.

####  мониторинг в реальном времени
* 🌡️ **Temperature Sensing**: Measures the ambient temperature using an LM35 sensor.
* 💡 **Light Sensing**: Measures the environmental light intensity using a Light Dependent Resistor (LDR).
* 💧 **Soil Moisture Sensing**: Monitors the soil's moisture level using a resistive sensor.
* 🚪 **Door Status Detection**: Detects if the greenhouse door is open or closed using a simple switch.

#### ⚙️ Automated Control
* **Fan Control**: A cooling fan is automatically activated via a relay if the temperature exceeds a predefined threshold (e.g., 30°C) and deactivated when it cools down (e.g., to 25°C).
* **Heater Control**: A heater is controlled using **PWM** to maintain the temperature above a minimum threshold (e.g., 20°C). The PWM duty cycle increases as the temperature drops.
* **Water Pump Control**: An irrigation pump is activated via a relay when the soil moisture drops below a set level (e.g., 35%).
* **Smart Curtain Control**: A smart curtain is controlled via **PWM** to regulate light exposure, opening or closing based on the ambient light level.

#### 📊 Data Logging & Display
* **LCD Display**: A **20x4 Alphanumeric LCD** provides a real-time display of all sensor readings (temperature, light %, moisture %), door status, and the current time.
* **Data Logging**: Every second, a timestamped record of all sensor data is stored on an external **25LC512 EEPROM** via the SPI bus.
* **Timekeeping**: An integrated **Real-Time Clock (RTC)** provides accurate timestamps for data logging and time-based events.

#### 📲 Remote Alerting System
* **GSM Module**: A **SIM800 GSM module** sends SMS alerts to a predefined phone number under critical conditions:
    * Temperature, light, or moisture levels entering a "danger zone."
    * The greenhouse door remaining open for more than 5 seconds.

## 🛠️ Hardware & Software Architecture

* **Microcontroller**: **STM32F103C6T6**.
* **Programming**: The firmware is written in **C** using the **STM32 HAL (Hardware Abstraction Layer)** library. The initial setup was configured using **STM32CubeMX**.
* **Simulation**: The entire circuit, including the MCU and all peripherals, was simulated in **Proteus**.

The system integrates multiple peripherals using various communication protocols:
* **ADC with DMA**: The internal **ADC** is used with **DMA (Direct Memory Access)** to continuously read data from the analog sensors (Temperature, Light, Moisture) without needing CPU intervention.
* **GPIO**: Used for simple digital on/off control of the fan and pump relays.
* **PWM**: Used to provide variable control for the heater and smart curtain actuators.
* **SPI**: Used for communication with the external **25LC512 EEPROM** for data logging.
* **UART**: Used to communicate with the **SIM800 GSM module** via AT commands.
* **RTC**: The onboard Real-Time Clock is used for all timekeeping functions.

## 🚀 How to Simulate
1.  The project was designed and simulated in **Proteus 8** (or a compatible version).
2.  The C firmware needs to be compiled using an ARM-C toolchain (e.g., Keil MDK, STM32CubeIDE) to generate a `.hex` file.
3.  Open the Proteus project file (`.pdsprj`).
4.  Load the compiled `.hex` file into the STM32F103C6 component within the Proteus schematic.
5.  Run the simulation. You can interact with the system using the virtual switches, potentiometers, and the Virtual Terminal (for simulating GSM communication).

## ✍️ Author
* **Ali Naghiloo**
