# HVAC Thermostat and Dual-Zone Controller
<img width="180" height="300" alt="Screenshot 2025-10-07 at 8 17 20 PM" src="https://github.com/user-attachments/assets/a4271154-e335-42ad-8088-0682db2936e8" />

## Overview
This project implements a **dual-zone HVAC control system** using an Arduino to regulate temperature independently across two zones. Each zone continuously monitors its temperature using analog sensors, compares it against a user-defined setpoint, and controls airflow through **servo-driven vents**.

The system supports **heating and cooling modes**, provides clear **LED-based state indication**, and is governed by a **Finite State Machine (FSM)** architecture. User interaction is enabled through switches and push buttons to power the system on/off, select operating modes, and adjust the target temperature.

The design demonstrates a modular, reliable, and extensible embedded control solution suitable for smart home or building automation systems.

---

## Features
- Dual-zone independent temperature control  
- Heating and cooling modes  
- Servo-controlled air vents  
- LED indicators for system state  
- Push-button and switch-based user input  
- FSM-based control logic  
- Real-time serial monitoring  
- Fully simulated and tested using **Wokwi**

---

## System Design

### Development Environment
- **Platform:** Arduino  
- **Simulation Tool:** Wokwi Online Arduino Simulator  
- **Control Strategy:** Finite State Machine (FSM)

The system is designed with a clear separation between **model, view, and controller**, enabling easier debugging, testing, and future expansion.

---

## Application Architecture

### A. System Architecture
The system follows a structured control model:

- **Model:**  
  Stores system data including:
  - `zone1Temp`, `zone2Temp`  
  - `setTemp`  
  - `zone1Open`, `zone2Open`  

- **View:**  
  Visual feedback via:
  - Servo positions (vent open/close)
  - LEDs for power, heating, cooling, and error states  

- **Controller:**  
  Handles:
  - FSM state transitions  
  - User input processing (switches, push buttons)  
  - Actuation of servos and LEDs  

This separation of concerns improves maintainability and allows straightforward system scaling (e.g., more zones or sensors).

---

## Finite State Machine (FSM) Design

### 1. System Control State Machine
The **System Control FSM** governs overall HVAC behavior and transitions between the following states:

- **OFF**
  - System inactive  
  - All LEDs off  
  - All vents closed  

- **HEAT**
  - Red LED active  
  - Vents open for zones below setpoint  

- **COOL**
  - Blue LED active  
  - Vents open for zones above setpoint  

- **ERROR**
  - All LEDs flash to alert the user  
  - All vents forced closed for safety  
  - Requires manual reset (power cycle)  

State transitions occur based on:
- Power switch input  
- Mode selector switch  
- Sensor or system fault detection  

---

### 2. Zone Control State Machine (Per Zone)
Each zone (Zone 1 and Zone 2) operates an **independent FSM**, ensuring true dual-zone control.

#### Zone States
- **ZONE_CLOSED**
  - Vent closed  
  - Servo set to 1 ms pulse width  

- **ZONE_OPEN**
  - Vent open  
  - Servo set to 2 ms pulse width  

#### Transition Logic
- **Heating Mode:**  
  - Vent opens when temperature < setpoint  

- **Cooling Mode:**  
  - Vent opens when temperature > setpoint  

Once the temperature stabilizes at the desired value, the vent closes automatically. This design minimizes energy waste and prevents zone interference.

---

## Results
The system successfully met all design objectives:

- Independent and accurate temperature regulation for both zones  
- Correct FSM transitions across OFF, HEAT, COOL, and ERROR states  
- Reliable servo actuation based on real-time sensor input  
- Clear LED status indication  
- Continuous serial output reporting:
  - Zone temperatures  
  - Active system mode  
  - Vent states  

The simulation confirmed stable real-time performance and robust control logic.

---
