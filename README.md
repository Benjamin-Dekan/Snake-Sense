# Snake Sense 🐍

Snake Sense is an embedded systems project that modernizes the classic arcade game "Snake" by implementing it on the BeagleBone Black with a spin. Instead of using traditional keyboard inputs, this project uses an LSM9DS0 Accelerometer to enable intuitive movement. 

## Features
* **Gesture Control:** Uses a 3-axis accelerometer (LSM9DS0) to detect hand tilt in real-time.
* **Userspace I2C Application:** Implements Linux I2C communication using the `i2c-dev` library to interface with the sensor directly from userspace, bypassing kernel module complexity.
* **Smart Directioning:** Features a threshold-based control algorithm (magnitude > 8500) check to distinguish between resting tremors and intentional movement.
* **Qt GUI:** Renders game graphics at 50Hz to a 4.3" LCD Cape using the Qt Framework.

## Hardware
* **BeagleBone Black**
* **4.3" LCD Display Cape** 
* **LSM9DS0 9-Axis IMU**
* **Jumper Wires** (Male-to-Female)

## Software
* **OS:** EC535-stock-net
* **Framework:** Qt 5.15.2

## Wiring
| LSM9DS0 Pin | BeagleBone Pin | Description |
| :--- | :--- | :--- |
| **VCC** | P9:04 | 3.3V Power |
| **GND** | P9:02 | Ground |
| **SCL** | P9:19 | I2C Clock|
| **SDA** | P9:20 | I2C Data |

## Controls
  * **Tilt Forward:** Move UP
  * **Tilt Backward:** Move DOWN
  * **Tilt Left:** Move LEFT
  * **Tilt Right:** Move RIGHT
  * **Spacebar:** Restart Game (if a keyboard is attached)

## Project Structure
* **`snakesense.h`**: Header file defining the game class, sensor vars, and I2C regs (`0x20`, `0x28`).
* **`snakesense.cpp`**: Main file containing the game loop, userspace I2C connection logic, and Qt painting events.
* **`main.cpp`**: Application entry point.
* **`snakesense.pro`**: Qmake config file defining the project's sources, headers, and dependencies.

## Contributors
**Team Pythons** 
* **Benjamin Dekan** (bdekan@bu.edu) - Hardware Integration & I2C Application
* **Chenyi Jiang** (chenyij@bu.edu) - Game Logic & Qt Implementation
