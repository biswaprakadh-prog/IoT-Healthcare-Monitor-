# IoT Healthcare Monitor 

An ESP32-powered wearable system designed for real-time health monitoring and emergency detection.

## 🚀 Features
* **Fall Detection:** Real-time impact monitoring using the **MPU6050** accelerometer.
* **Vital Signs:** Body temperature tracking via **DHT11** and simulated SpO2/BP monitoring.
* **Emergency Alert:** Audible buzzer alarm with a physical **Panic/Reset button**.
* **Dual-Mode Visualization:** Local Web Dashboard (Browser) and Real-time Graphing (Serial Plotter).

## 🛠️ Technical Design
The system architecture follows a three-layer approach:
1. **Perception:** MPU6050 (Motion), DHT11 (Temp), Potentiometer (ADC Simulation).
2. **Processing:** ESP32 handles threshold logic and hosts a Local Web Server.
3. **Visualization:** Access Point (AP) mode allows monitoring without an internet connection.

### 📉 Fall Detection Logic
We calculate the total acceleration magnitude ($A_{total}$) using:
$$A_{total} = \sqrt{A_x^2 + A_y^2 + A_z^2}$$
* **Free-Fall:** Triggered if $A_{total} < 0.5G$.
* **Impact:** Triggered if $A_{total} > 2.5G$.

## 💻 How to Use
1. **Connect to Wi-Fi:** Search for `TechNova_Healthcare` on your phone/PC.
2. **Password:** `12345678`
3. **Open Dashboard:** Type `192.168.4.1` in your web browser.
4. **Serial Plotter:** Open Arduino IDE -> Tools -> Serial Plotter (115200 baud) to see live graphs.

## 📦 Hardware Setup
* **Microcontroller:** ESP32
* **Sensors:** MPU6050 (SDA: 21, SCL: 22), DHT11 (Pin 4)
* **Inputs:** Potentiometer (Pin 34), Reset Button (Pin 13)
* **Output:** Active Buzzer (Pin 5)
*
