# Automatic Roller Blinds

## Overview
This project automates roller blinds using an **ESP8266**, a **NEMA 17 stepper motor**, and a **TMC2208 stepper driver**. It is controlled via **MQTT**, allowing integration with **Home Assistant** or other smart home systems. The system remembers the last position even after a power outage.

## Features
- Wi-Fi connectivity for remote control
- MQTT-based communication
- Stepper motor control with TMC2208
- Hall sensor for position detection
- EEPROM storage for remembering maximum steps
- Power supply via **12V** with a **buck converter** to **3.3V** for ESP8266

## Components
- **ESP8266**
- **NEMA 17 Stepper Motor**
- **TMC2208 Stepper Driver**
- **Hall Sensor** (for position detection)
- **12V Power Supply**
- **Buck Converter** (12V to 3.3V)

## Wiring Diagram
| Component       | ESP8266 Pin |
|----------------|------------|
| Stepper STEP   | GPIO4      |
| Stepper DIR    | GPIO5      |
| Stepper ENABLE | GPIO0      |
| Hall Sensor    | A0         |
| MS1            | GPIO14     |
| MS2            | GPIO12     |

## Installation
### 1. Install Required Libraries
Ensure you have the following libraries installed in the Arduino IDE:
- **ESP8266WiFi** (for Wi-Fi connectivity)
- **PubSubClient** (for MQTT communication)
- **EEPROM** (to store max steps)

### 2. Configure Wi-Fi and MQTT
Update the following details in the `.ino` file:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const char* mqtt_user = "YOUR_MQTT_USERNAME";
const char* mqtt_pass = "YOUR_MQTT_PASSWORD";
```

### 3. Upload the Code
Compile and upload the `AutomaticRollerBlinds.ino` file to your ESP8266.

### 4. MQTT Topics
| Topic             | Command Description       |
|------------------|-------------------------|
| `roller/home`    | Move to home position   |
| `roller/up`      | Roll up the blinds      |
| `roller/down`    | Roll down the blinds    |
| `roller/stop`    | Stop movement           |
| `roller/roll_max` | Move to max position   |
| `roller/set_max` | Save current position as max |

## Future Improvements
- Use **FRAM storage** instead of EEPROM to prevent write limits.
- Integrate with **Home Assistant** for automation.
- Add **position feedback** to Home Assistant.

## License
This project is open-source under the **MIT License**.

