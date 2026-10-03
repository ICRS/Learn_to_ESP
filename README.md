# ESP32 Super Mini OLED Starter Workshop

Welcome! This repository contains everything you need to get your **ESP32-C3 Super Mini with built-in OLED** up and running in minutes. 

No prior electronics or programming experience required!

---

## Step 1: Download & Install Arduino IDE

1. Download and install **Arduino IDE 2.x** from the official website: [arduino.cc/en/software](https://www.arduino.cc/en/software).
2. Open the Arduino IDE on your computer.

---

## Step 2: Add ESP32 Board Support to Arduino

The Arduino IDE needs to know how to speak to ESP32 microcontrollers:

1. In Arduino IDE, open your preferences:
   - **macOS:** `Cmd + ,` (or go to `Arduino IDE > Settings...`)
   - **Windows / Linux:** `Ctrl + ,` (or go to `File > Preferences`)
2. Find the field labeled **Additional boards manager URLs**.
3. Paste the following URL into the box:
   ```text
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
   *(If there is already another URL in that field, separate them with a comma `,`).*
4. Click **OK**.
5. On the left sidebar of the Arduino IDE, click the **Boards Manager** icon (looks like a circuit board).
6. Search for `esp32` and install the package by **Espressif Systems**.

---

## Step 3: Install the Required Library

The on-board screen uses an open-source graphics library called **U8g2**:

1. On the left sidebar, click the **Library Manager** icon (looks like a stack of books).
2. In the search bar, type:
   ```text
   U8g2
   ```
3. Locate **U8g2 by oliver** and click **Install**.

---

## Step 4: Connect & Select Board Settings

1. Plug your ESP32 Super Mini into your computer using a **USB-C Data Cable**.  
   *(Note: Some cheap phone-charging cables only carry power and no data lines. If your computer does not detect a serial device, try a different USB-C cable).*
2. In the Arduino IDE top menu, select:
   - **Tools > Board > esp32 > ESP32C3 Dev Module**
   - **Tools > USB CDC On Boot > Enabled** *(Crucial for the ESP32-C3 Super Mini!)*
   - **Tools > Port >** Select the port for your board (`COM...` on Windows, `/dev/cu.usb...` on macOS).

>  **Bootloader Mode / Flash Tip:**  
> If an upload ever hangs or fails with `Failed to connect to ESP32-C3`:  
> 1. Hold down the onboard **BOOT** (or **BOO**) button.  
> 2. Plug the USB-C cable in (or press and release the **RESET** button while holding BOOT).  
> 3. Release the **BOOT** button. This forces the chip into download mode so you can hit upload again.

---

## The 3 Sample Projects

Open any `.ino` file in the Arduino IDE and click the **Upload arrow (➜)** in the top toolbar:

| Project | Difficulty | Description |
| :--- | :--- | :--- |
| **`01_digital_dice`** | Simplest | **Start with this one to try out the process of uploading firmware!** Press the onboard button to trigger an animated, randomized 6-sided dice roll. |
| **`02_slot_machine`** | Next simplest | A 3-reel casino slot game with staggered reel stops and jackpot animations. |
| **`03_wifi_pager`** | Using Wifi | Sets up a standalone Wi-Fi hotspot and web portal to send messages straight to the OLED screen from your phone. |

---

## About The ESP32-C3 Super Mini

You are working with a teeny tiny 32-bit single-core RISC-V microcontroller equipped with 2.4 GHz Wi-Fi and Bluetooth LE.

### Pre-Wired Onboard Hardware
- **Monochrome OLED (72x40 SSD1306):**
  - Clock (`SCL`) $\to$ **GPIO 6**
  - Data (`SDA`) $\to$ **GPIO 5**
- **User Button:** Labeled `BOOT` or `BOO` on the PCB $\to$ **GPIO 9** (Active LOW, pulled HIGH internally).

### Pinout Diagram
```text
           +-----------------+
           |     [USB-C]     |
        V5 | [ ]         [ ] | GPIO 10
        GD | [ ]         [ ] | GPIO 9
        V3 | [ ]  ESP32  [ ] | GPIO 8
        RX | [ ]   -C3   [ ] | GPIO 7
        TX | [ ]         [ ] | GPIO 6
    GPIO 2 | [ ] [OLED]  [ ] | GPIO 5
    GPIO 1 | [ ]         [ ] | GPIO 4
    GPIO 0 | [ ]         [ ] | GPIO 3
           +-----------------+
```

### Ideas for A Project
You can do so much more with a microcontroller when you add peripherals! Think of ways you can turn this fun toy into a practical daily life tool (add a buzzer to make a portable alarm clock, or some sensors to turn it into a wearable (could make a tiny smart watch!), the coolest pair of earrings ever, with a humidity sensor you can monitor your plants' health, etc!)
You can also:
- **Digital Outputs:** Add external LEDs, buzzers, or relays using `pinMode(pin, OUTPUT)` and `digitalWrite(pin, HIGH)`.
- **Bluetooth:** Use the included `BLEDevice.h` library to communicate wirelessly with mobile apps or build custom controllers.

*This ReadMe and the accompanying programs were made with the help of Gemini (the generative AI tool)*
