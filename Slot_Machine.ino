/*
 * ============================================================================
 * PROJECT: 02 - Mini Slot Machine
 * BOARD:   ESP32-C3 Super Mini (with built-in 72x40 SSD1306 OLED)
 * ============================================================================
 * 
 * WHAT THIS PROGRAM DOES:
 * - Simulates a 3-reel casino slot machine with animated spinning.
 * - Press the "BOOT" button to spin the reels.
 * - Each reel stops one by one with authentic staggered deceleration.
 * - Detects matches: Jackpots (3-of-a-kind) trigger a flashing victory screen!
 * 
 * HARDWARE CONNECTIONS (Pre-wired on board):
 * - Screen I2C Clock (SCL) -> GPIO 6
 * - Screen I2C Data (SDA)  -> GPIO 5
 * - Onboard BOOT Button    -> GPIO 9
 * 
 * HOW TO FLASH:
 * 1. Select Board: "ESP32C3 Dev Module"
 * 2. Select your Port.
 * 3. Make sure "USB CDC On Boot" is set to "Enabled".
 * 4. Click the "Upload" arrow button in the Arduino IDE.
 * ============================================================================
 */

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// Initialize 72x40 SSD1306: SCL = 6, SDA = 5
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 6, /* data=*/ 5);

const int BUTTON_PIN = 9; // Onboard BOOT button

int reels[3] = {7, 7, 7}; // Default starting numbers

// Helper: draws the 3 reel boxes, numbers, and status text on screen
void drawReels(int r1, int r2, int r3, const char* footerMsg = "") {
  u8g2.clearBuffer();

  // Draw 3 boxes side-by-side (20px wide, 24px high)
  // Horizontal start positions: Reel 1 = 3, Reel 2 = 26, Reel 3 = 49
  int xPos[3] = {3, 26, 49};
  int vals[3] = {r1, r2, r3};

  u8g2.setFont(u8g2_font_helvB14_tr); // Large bold font for reel digits

  for (int i = 0; i < 3; i++) {
    // Draw reel border
    u8g2.drawFrame(xPos[i], 2, 20, 24);
    
    // Center the number inside the 20x24 box
    u8g2.setCursor(xPos[i] + 5, 20);
    u8g2.print(vals[i]);
  }

  // Draw bottom status message
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(6, 36, footerMsg);

  u8g2.sendBuffer();
}

// Staggered reel spinning animation:
// Reel 1 locks first, then Reel 2, then Reel 3
void spinAnimation() {
  int tempReels[3] = {reels[0], reels[1], reels[2]};
  
  for (int step = 0; step < 28; step++) {
    // Reel 1 spins until step 14
    if (step < 14) tempReels[0] = (esp_random() % 9) + 1;
    // Reel 2 spins until step 20
    if (step < 20) tempReels[1] = (esp_random() % 9) + 1;
    // Reel 3 spins until the very end
    tempReels[2] = (esp_random() % 9) + 1;

    drawReels(tempReels[0], tempReels[1], tempReels[2], "SPINNING...");
    delay(40 + (step * 4)); // Gradually slow down
  }

  // Save the final settled numbers
  reels[0] = tempReels[0];
  reels[1] = tempReels[1];
  reels[2] = tempReels[2];
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  u8g2.begin();
  u8g2.setContrast(255);

  // Ready screen with lucky sevens
  drawReels(7, 7, 7, "PRESS BOO!");
}

void loop() {
  // Button press detected
  if (digitalRead(BUTTON_PIN) == LOW) {
    spinAnimation();

    // Check game outcome:
    if (reels[0] == reels[1] && reels[1] == reels[2]) {
      // 3 of a kind: JACKPOT flashing animation!
      for (int i = 0; i < 5; i++) {
        drawReels(reels[0], reels[1], reels[2], "*** JACKPOT! ***");
        delay(150);
        u8g2.clearBuffer();
        u8g2.sendBuffer();
        delay(100);
      }
      drawReels(reels[0], reels[1], reels[2], "YOU WON! :)");
    } else if (reels[0] == reels[1] || reels[1] == reels[2] || reels[0] == reels[2]) {
      // 2 of a kind
      drawReels(reels[0], reels[1], reels[2], "SO CLOSE! 2 MATCH");
    } else {
      // No match
      drawReels(reels[0], reels[1], reels[2], "TRY AGAIN!");
    }

    // Debounce: wait for button release
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
    }
    delay(150);
  }
}
