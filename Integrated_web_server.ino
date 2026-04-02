#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>

// --- Configuration ---
const char* ap_ssid = "TechNova_Healthcare";
const char* ap_pass = "12345678";

// Pin Definitions
#define DHTPIN 4
#define DHTTYPE DHT11
#define POT_PIN 34
#define SDA_PIN 21
#define SCL_PIN 22
#define BUZZER_PIN 5
#define BUTTON_PIN 13

// Object Initializations
DHT dht(DHTPIN, DHTTYPE);
Adafruit_MPU6050 mpu;
WebServer server(80);

// Global State
bool alarmActive = false;

// HTML Webpage Generation
void handleRoot() {
  // Read Sensors
  float temp = dht.readTemperature();
  int potValue = analogRead(POT_PIN);
  int simSpO2 = map(potValue, 0, 4095, 80, 100);
  int simBP = map(potValue, 0, 4095, 90, 180);
  
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);
  float totalAcc = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
  
  // Logic for Alarm
  if (simSpO2 < 92 || totalAcc > 25.0 || totalAcc < 5.0) {
    alarmActive = true;
  }

  String status = alarmActive ? "CRITICAL ALERT" : "SYSTEM NORMAL";
  String bgColor = alarmActive ? "#ff4d4d" : "#2ecc71";

  // HTML Content
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><meta http-equiv='refresh' content='2'>";
  html += "<style>body{font-family:sans-serif; text-align:center; background:#f0f2f5; margin:0;}";
  html += ".header{background:" + bgColor + "; color:white; padding:20px; font-size:24px;}";
  html += ".card{background:white; margin:20px; padding:20px; border-radius:15px; box-shadow:0 4px 6px rgba(0,0,0,0.1);}";
  html += ".val{font-size:32px; font-weight:bold; color:#2c3e50;} .unit{font-size:16px; color:#7f8c8d;}</style></head><body>";
  
  html += "<div class='header'><h1>" + status + "</h1></div>";
  
  html += "<div class='card'><div>Temperature</div><div class='val'>" + String(temp, 1) + "<span class='unit'> &deg;C</span></div></div>";
  html += "<div class='card'><div>Blood Oxygen (SpO2)</div><div class='val'>" + String(simSpO2) + "<span class='unit'> %</span></div></div>";
  html += "<div class='card'><div>Blood Pressure</div><div class='val'>" + String(simBP) + "<span class='unit'> mmHg</span></div></div>";
  html += "<div class='card'><div>Motion Force</div><div class='val'>" + String(totalAcc, 2) + "<span class='unit'> G</span></div></div>";
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize Sensors
  Wire.begin(SDA_PIN, SCL_PIN);
  dht.begin();
  if (!mpu.begin()) {
    Serial.println("MPU6050 failed!");
  }

  // Set ESP32 as an Access Point
  WiFi.softAP(ap_ssid, ap_pass);
  
  Serial.println("\n--- TechNova Server Started ---");
  Serial.print("Connect to Wi-Fi: "); Serial.println(ap_ssid);
  Serial.print("Open this IP in Browser: "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  // 1. Read Sensors for reporting
  float temp = dht.readTemperature();
  int potValue = analogRead(POT_PIN);
  int simSpO2 = map(potValue, 0, 4095, 80, 100);
  int simBP = map(potValue, 0, 4095, 90, 180);
  
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);
  float totalAcc = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));

  // 2. SERIAL PLOTTER & MONITOR OUTPUT
  // This format allows the Serial Plotter to draw multiple lines
  Serial.print("Temperature:"); Serial.print(temp); Serial.print(",");
  Serial.print("SpO2:");        Serial.print(simSpO2); Serial.print(",");
  Serial.print("BloodPressure:"); Serial.print(simBP); Serial.print(",");
  Serial.print("G-Force:");     Serial.println(totalAcc);

  // 3. Logic for Alarm (SpO2 drop or Fall)
  if (simSpO2 < 92 || totalAcc > 25.0 || totalAcc < 5.0) {
    alarmActive = true;
  }

  // 4. Reset Alarm via physical button
  if (digitalRead(BUTTON_PIN) == LOW) {
    alarmActive = false;
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println(">> ALARM RESET BY USER <<");
  }

  // 5. Buzzer Alert Logic
  if (alarmActive) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
  }
  
  delay(200); // Small delay to make the Serial Plotter readable
}