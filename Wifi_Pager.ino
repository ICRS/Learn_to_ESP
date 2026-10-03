/*
 * ============================================================================
 * PROJECT: 03 - Wi-Fi Mini OLED Pager
 * BOARD:   ESP32-C3 Super Mini (with built-in 72x40 SSD1306 OLED)
 * ============================================================================
 * 
 * WHAT THIS PROGRAM DOES:
 * - Turns your ESP32 into a standalone Wi-Fi Access Point ("ESP32-Badge").
 * - Hosts an internal web page at http://192.168.4.1.
 * - Anyone who connects to the Wi-Fi on their phone/laptop can submit a name
 *   and message, which instantly prints live on the OLED badge screen!
 * - Includes low-power & thermal throttles to keep the tiny board cool.
 * 
 * HARDWARE CONNECTIONS (Pre-wired on board):
 * - Screen I2C Clock (SCL) -> GPIO 6
 * - Screen I2C Data (SDA)  -> GPIO 5
 * 
 * HOW TO FLASH:
 * 1. Select Board: "ESP32C3 Dev Module"
 * 2. Select your Port.
 * 3. Make sure "USB CDC On Boot" is set to "Enabled".
 * 4. Click the "Upload" arrow button in the Arduino IDE.
 * ============================================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <U8g2lib.h>
#include <Wire.h>

// 72x40 SSD1306 Display on GPIO 6 (SCL) and GPIO 5 (SDA)
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 6, /* data=*/ 5);

// Wi-Fi Access Point Name (No password required)
const char* ssid = "ESP32-Badge";
WebServer server(80);

String currentMessage = "Ready!";
String senderName = "System";

// Re-renders the OLED layout with header bar and message
void updateDisplay() {
  u8g2.clearBuffer();

  // Top header bar (black background with inverted white text)
  u8g2.drawBox(0, 0, 72, 9);
  u8g2.setDrawColor(0); // 0 = black on white background
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.setCursor(2, 7);
  u8g2.print("FROM: ");
  u8g2.print(senderName.substring(0, 7)); // Truncate name to 7 chars to fit

  // Message area (normal white text on black background)
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_6x10_tf);
  
  // Wrap text across two lines if longer than 11 characters
  if (currentMessage.length() <= 11) {
    u8g2.setCursor(4, 25);
    u8g2.print(currentMessage);
  } else {
    u8g2.setCursor(2, 21);
    u8g2.print(currentMessage.substring(0, 11));
    u8g2.setCursor(2, 35);
    u8g2.print(currentMessage.substring(11, 22));
  }

  u8g2.sendBuffer();
}

// Serve the mobile-friendly web page
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:sans-serif;text-align:center;padding:20px;background:#121212;color:#eee;}";
  html += "input{width:90%;max-width:300px;padding:12px;margin:8px 0;border-radius:6px;border:none;font-size:16px;}";
  html += "button{width:95%;max-width:320px;padding:12px;background:#00c853;color:white;font-weight:bold;border:none;border-radius:6px;font-size:18px;}";
  html += "</style></head><body>";
  html += "<h2>Mini OLED Pager</h2>";
  html += "<form action='/send' method='POST'>";
  html += "<input type='text' name='user' placeholder='Your Name' maxlength='8'><br>";
  html += "<input type='text' name='msg' placeholder='Message (max 22 chars)' maxlength='22' required><br><br>";
  html += "<button type='submit'>Send to Screen</button>";
  html += "</form></body></html>";

  server.send(200, "text/html", html);
}

// Receive form submission from phone/browser
void handleSend() {
  if (server.hasArg("msg")) {
    currentMessage = server.arg("msg");
    senderName = server.hasArg("user") && server.arg("user") != "" ? server.arg("user") : "Anon";
    updateDisplay();
  }
  // Redirect browser back to home page
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  // 1. Thermal management: lower CPU frequency to 80MHz to avoid overheating
  setCpuFrequencyMhz(80);

  u8g2.begin();
  u8g2.setContrast(255);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(4, 20, "Starting AP...");
  u8g2.sendBuffer();

  // Create the Wi-Fi hotspot
  WiFi.softAP(ssid);

  // 2. Reduce Wi-Fi transmission power to keep chip cool in small spaces
  WiFi.setTxPower(WIFI_POWER_8_5dBm);

  // Set up web server endpoints
  server.on("/", HTTP_GET, handleRoot);
  server.on("/send", HTTP_POST, handleSend);
  server.begin();

  // Display connection instructions on OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(0, 11, "WiFi:ESP32-Badge");
  u8g2.drawStr(0, 23, "IP:192.168.4.1");
  u8g2.drawStr(0, 36, "Open in browser!");
  u8g2.sendBuffer();
}

void loop() {
  server.handleClient(); // Process incoming HTTP requests
  
  // 3. Yield CPU time so the FreeRTOS idle task can manage background tasks
  delay(10);
}
