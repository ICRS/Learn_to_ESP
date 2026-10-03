/*
 * ============================================================================
 * PROJECT: 01 - Digital Dice
 * BOARD:   ESP32-C3 Super Mini (with built-in 72x40 SSD1306 OLED)
 * ============================================================================
 * 
 * WHAT THIS PROGRAM DOES:
 * - Displays a classic six-sided dice face on your tiny OLED screen.
 * - When you press the onboard button labeled "BOOT" (or "BOO"), it triggers
 *   a rolling animation that gradually slows down and lands on a random number (1-6).
 * 
 * HARDWARE CONNECTIONS (Pre-wired on board):
 * - Screen I2C Clock (SCL) -> GPIO 6
 * - Screen I2C Data (SDA)  -> GPIO 5
 * - Onboard BOOT Button    -> GPIO 9
 * 
 * HOW TO FLASH:
 * 1. Select Board: "ESP32C3 Dev Module"
 * 2. Select your Port (COM port on Windows, /dev/cu.usb... on Mac).
 * 3. Make sure "USB CDC On Boot" is set to "Enabled" in Tools.
 * 4. Click the "Upload" arrow button in the Arduino IDE.
 * ============================================================================
 */

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// Initialize the 72x40 OLED display using hardware I2C
// SCL is pinned to GPIO 6, SDA is pinned to GPIO 5
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 6, /* data=*/ 5);

// The onboard button is wired to GPIO 9 (active LOW: reads 0 when pressed)
const int BUTTON_PIN = 9;

int diceValue = 1;

// Draws a standard rounded dice box and the correct dot pattern (pips)
void drawDice(int value) {
  u8g2.clearBuffer(); // Clear the internal graphics memory

  // Center a 32x32 dice box on the 72x40 display
  const int bx = 20; // X offset: (72 - 32) / 2 = 20
  const int by = 4;  // Y offset: (40 - 32) / 2 = 4
  u8g2.drawRFrame(bx, by, 32, 32, 4); // Draw rounded rectangle outline

  // Column and row pixel positions for the dice dots (pips)
  int xL = bx + 8;   // Left column
  int xC = bx + 16;  // Center column
  int xR = bx + 24;  // Right column

  int yT = by + 8;   // Top row
  int yC = by + 16;  // Center row
  int yB = by + 24;  // Bottom row

  int r = 2; // Radius of each dot

  // Draw center dot for odd numbers (1, 3, 5)
  if (value == 1 || value == 3 || value == 5) {
    u8g2.drawDisc(xC, yC, r);
  }

  // Draw top-left and bottom-right dots (2, 3, 4, 5, 6)
  if (value >= 2) {
    u8g2.drawDisc(xL, yT, r);
    u8g2.drawDisc(xR, yB, r);
  }

  // Draw top-right and bottom-left dots (4, 5, 6)
  if (value >= 4) {
    u8g2.drawDisc(xR, yT, r);
    u8g2.drawDisc(xL, yB, r);
  }

  // Draw middle-left and middle-right dots (6 only)
  if (value == 6) {
    u8g2.drawDisc(xL, yC, r);
    u8g2.drawDisc(xR, yC, r);
  }

  u8g2.sendBuffer(); // Push graphics from memory to physical screen
}

// Simulates the dice tumbling and slowing down before stopping
void rollAnimation() {
  for (int i = 0; i < 15; i++) {
    int tempVal = (esp_random() % 6) + 1; // Pick a random value 1-6
    drawDice(tempVal);
    delay(30 + (i * 12)); // Add delay each cycle to simulate friction/deceleration
  }
}

void setup() {
  // Use internal pull-up resistor: button pin stays HIGH until pressed to GND
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Turn on and configure the display
  u8g2.begin();
  u8g2.setContrast(255); // Max brightness

  // Welcome splash screen
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(12, 16, "DIGITAL DICE");
  u8g2.drawStr(10, 30, "PRESS 'BOO'");
  u8g2.sendBuffer();
}

void loop() {
  // Check if button is pushed down
  if (digitalRead(BUTTON_PIN) == LOW) {
    rollAnimation();

    // Pick final true random roll using ESP32 hardware RNG
    diceValue = (esp_random() % 6) + 1;
    drawDice(diceValue);

    // Debounce: pause until user lifts finger off button
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
    }
    delay(100); // Short settle time to avoid double-triggers
  }
}
