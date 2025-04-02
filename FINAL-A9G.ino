#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// OLED Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// GPS Setup
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);  // UART1 (RX=16, TX=17)

// Function to print messages on OLED
void displayMessage(const char* message, int y = 10) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, y);
    display.println(message);
    display.display();
}

// Send AT command to A9G
void sendCommand(const char* cmd) {
    gpsSerial.println(cmd);
    delay(500);  // Small delay to allow processing
}

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(115200, SERIAL_8N1, 16, 17);

    // OLED Initialization
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED init failed");
        while (1);
    }

    displayMessage("Initializing GPS...");

    // Auto-enable GPS on boot
    Serial.println("Checking A9G response...");
    sendCommand("AT");

    delay(1000);  // Wait for A9G response
    if (gpsSerial.available()) {
        Serial.println("A9G Detected!");
        displayMessage("A9G Connected!");
    } else {
        Serial.println("A9G NOT responding!");
        displayMessage("A9G ERROR! Check Wiring!");
        return;  // Stop setup if A9G is unresponsive
    }

    sendCommand("AT+GPS=1");    // Enable GPS
    sendCommand("AT+GPSRD=1");  // Start sending GPS data

    Serial.println("GPS Enabled! Waiting for Fix...");
    displayMessage("GPS Enabled! Waiting...");
}

void loop() {
    while (gpsSerial.available()) {
        char c = gpsSerial.read();
        Serial.write(c);  // Print raw GPS data for debugging
        gps.encode(c);

        if (gps.location.isUpdated()) {
            float latitude = gps.location.lat();
            float longitude = gps.location.lng();
            int satellites = gps.satellites.value();
            float altitude = gps.altitude.meters();

            // Print to Serial Monitor
            Serial.print("Lat: ");
            Serial.print(latitude, 6);
            Serial.print(" | Lon: ");
            Serial.print(longitude, 6);
            Serial.print(" | Sats: ");
            Serial.print(satellites);
            Serial.print(" | Alt: ");
            Serial.print(altitude, 2);
            Serial.println(" m");

            // Display GPS Data on OLED
            display.clearDisplay();
            display.setTextSize(1);
            display.setCursor(0, 0);
            display.print("Satellites: ");
            display.println(satellites);
            display.print("Lat: ");
            display.println(latitude, 6);
            display.print("Lon: ");
            display.println(longitude, 6);
            display.print("Alt: ");
            display.print(altitude, 2);
            display.println(" m");
            display.display();
        }
    
