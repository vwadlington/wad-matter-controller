# ESP32-S3 Matter Controller with LVGL Dashboard

A Matter Controller implementation for the **ESP32-S3-LCD-EV-Board** (SUB3). This project bridges a custom **LVGL** touchscreen interface with the **ESP-Matter SDK** to control smart home devices (Lights, Switches, etc.) over Wi-Fi.

## � Target Audience

This project is intended for:

*   **Embedded Developers** interested in IoT and smart home applications.
*   **Hobbyists and Makers** exploring the Matter protocol on Espressif hardware.
*   **Students and Researchers** working with LVGL and ESP-IDF.

## ✨ Key Features

*   **Intuitive Touchscreen Control:** A responsive LVGL-based dashboard to manage your smart home devices.
*   **Matter Controller:** Built on the ESP-Matter SDK to communicate with a wide range of Matter-compatible devices.
*   **BLE Commissioning:** Simple device pairing process using Bluetooth LE.
*   **Persistent Storage:** Remembers Wi-Fi credentials and commissioned devices across reboots.
*   **Extensible Architecture:** Easily add support for new device types and UI elements.

## � Project Architecture

This project is structured into three distinct layers to ensure UI responsiveness and network stability:

* **UI Layer (`components/ui_dashboard`):** Pure C/LVGL code. Handles the rendering of buttons, sliders, and status icons.
* **Matter Layer (`main/` & `esp_matter`):** Handles the Matter stack, Bluetooth LE commissioning, and secure communication with end-devices.
* **The Bridge:** Glue logic in `main/app_main.cpp` that schedules Matter commands onto the Matter thread when UI events (clicks/swipes) occur.

## 📂 Directory Structure

```text
.
├── components/
│   ├── ui_dashboard/          # UI Logic (LVGL screens and elements)
│   └── matter_bridge/         # C++ Wrappers for Matter Controller commands
├── main/
│   ├── app_main.cpp           # Entry point: System Init & Thread Orchestration
│   ├── app_priv.h             # Shared constants and Node IDs
│   └── CMakeLists.txt         # Project build rules
├── partitions.csv             # Custom 16MB Flash layout (Large NVS for Matter)
├── sdkconfig.defaults         # Pre-configured PSRAM, WiFi, and Matter settings
└── README.md
```

## 🛠 Setup & Installation (WSL2)

### Prerequisites

Before you begin, ensure you have the following installed:

*   **[ESP-IDF v5.5.2](https://docs.espressif.com/projects/esp-idf/en/v5.5.2/esp32s3/get-started/index.html)**: The core development framework.
*   **[ESP-Matter SDK](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html)**: For Matter-specific functionality.
*   **[usbipd-win](https://github.com/dorssel/usbipd-win)**: For USB passthrough from Windows to WSL2.

### 1. Environment Initialization

Run these commands in your WSL2 terminal to set up the environment:

```bash
. ~/.espressif/v5.5.2/esp-idf/export.sh
. ~/.espressif/esp-matter/export.sh
```

### 2. Hardware Configuration

Set the target to ESP32-S3 and add the Board Support Package (BSP) for the ESP32-S3-LCD-EV-Board:

```bash
idf.py set-target esp32s3
idf.py add-dependency "espressif/esp32_s3_lcd_ev_board^4.0.0"
```

### 3. Build & Flash

Attach your board via `usbipd` in PowerShell, then run the following commands to build, flash, and monitor the application:

```bash
idf.py build
idf.py -p /dev/ttyACM0 flash monitor
```
🚀 Usage

### Commissioning a Device

1.  **Start Commissioning:** On the LCD, tap the "Pair Device" button to put the controller into commissioning mode.
2.  **Find the QR Code:** A QR code will be printed in the terminal monitor. You can also find the manual pairing code in the logs.
3.  **Use Your Matter App:** Open your preferred Matter-compatible app (e.g., Google Home, Apple Home) and scan the QR code to commission the controller.

### Controlling a Device

Once commissioned, the LVGL dashboard will display controls for your connected devices. You can:

*   **Toggle Lights:** Tap the lightbulb icon to turn a light on or off.
*   **Adjust Brightness:** Use the slider to change the brightness of a light.
*   **Control Switches:** Tap the switch icon to toggle a smart switch.

Persistent Storage
Wi-Fi: Credentials are saved in the nvs partition and auto-reconnect on boot.

Fabric Data: Commissioned devices are stored permanently; you do not need to re-pair after a power cycle.

⚠️ Important Notes
PSRAM: This project requires Octal PSRAM enabled (configured in sdkconfig.defaults).

WSL2 Speed: Ensure this project folder is located in the Linux filesystem (/home/user/...) and not a mounted Windows drive (/mnt/c/...) for 10x faster compilation.

## 🤝 Contributing

Contributions are welcome! Please refer to the `CONTRIBUTING.md` file for guidelines on how to submit pull requests, report issues, and suggest enhancements.

## 📜 License

This project is licensed under the MIT License. See the `LICENSE` file for more details.

## 💬 Support

If you have any questions or encounter issues, please open an issue on the GitHub repository.