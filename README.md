# 8-Bit Hardware Simon Says Game

An embedded systems project built on an Arduino microcontroller utilizing a **74HC595 shift register** to drive an 8-bit binary memory game. This project bridges low-level bitwise manipulation in C++ with physical hardware design, featuring real-time state management, hardware debouncing, and dedicated status feedback indicators.

---

## Key Features

* **Shift Register Data Streaming:** Utilizes a 74HC595 serial-in/parallel-out shift register to drive 8 game LEDs from minimal microcontroller pins, handling MSB-to-LSB bit shifting.
* **Standalone Status Feedback:** Integrates a dedicated 3-color status cluster (Yellow for input confirmation, Green for win, Red for loss) to decouple game data from system states and eliminate user ambiguity.
* **Robust Input Handling:** Configured with internal pull-up logic and software state-machine timing to prevent erratic button behavior and electromagnetic noise.
* **Interactive Idle Standby Mode:** Features a latching standby state that floods the register with all `1`s upon game completion, creating a visual "scoreboard/ready" wall of light until the player triggers a restart.

---

## Tech Stack & Hardware Components

* **Microcontroller:** Arduino (Uno / Nano compatible)
* **Shift Register:** 74HC595 8-bit shift register
* **Language:** C++ (Arduino IDE)
* **Hardware Peripherals:** 
  * 1x Push button
  * 8x LEDs (Game display array, can be any color)
  * 3x Status LEDs (Yellow, Green, Red)
  * 11x Current-limiting resistors (220Ω)
  * 1x Button Pull-Down Resistor (10kΩ)

---

## Circuit Schematic & Pinout Overview

| Component Pin | Arduino / Shift Register Pin | Function |
| :--- | :--- | :--- |
| **74HC595 SER (Pin 14)** | Arduino Pin 10 | Serial Data Input |
| **74HC595 SRCLK (Pin 11)**| Arduino Pin 8 | Shift Register Clock |
| **74HC595 RCLK (Pin 12)** | Arduino Pin 9 | Storage Register Clock (Latch) |
| **74HC595 SERCLR (Pin 10)**  | 5V (HIGH) | Master Reset (Active Low) |
| **74HC595 OE (Pin 13)**  | GND (LOW) | Output Enable (Active Low) |
| **Input Button**         | Arduino Pin 12 | Debounced Player Input (`INPUT_PULLUP`) |
| **Yellow Status LED**    | Arduino Pin 5 | Input Confirmation Indicator |
| **Green Status LED**     | Arduino Pin 6 | Win State Indicator |
| **Red Status LED**       | Arduino Pin 4 | Loss State Indicator |

---

## Core Software Architecture

The firmware relies on non-blocking state management to track player input sequences across rounds:
1. **Target Generation:** Generates a random target byte (`0–255`) and shifts it out bit-by-bit to the register.
2.**State-Machine Button Parsing & Timing:** Implements a dynamic, non-blocking button state handler that tracks press duration. It distinguishes between quick clicks (`0` via `bitClear`) and extended long-press holds (`1` via `bitSet`), streaming inputs sequentially from bit index 7 down to 0.
3. **Direct Byte Comparison:** Compares `playerByte == dataIN` in a single processor cycle to determine game outcomes instantly.
4. **State Transition & Standby:** Triggers multi-color status LED feedback (Yellow for confirmation, Green/Red for outcome) and latches all register outputs `HIGH` to enter a persistent idle state awaiting a hardware reset (game reset, so the player can choose to play again).
---

## 📸 Demo / Gallery
*(Optional: Drop a short GIF or photo of your breadboard / enclosure here showing the LEDs lighting up and the buttons being pressed!)*
