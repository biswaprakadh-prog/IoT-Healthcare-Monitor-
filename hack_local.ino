#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>

// Pin Definitions
#define DHTPIN 4
#define DHTTYPE DHT11
#define POT_PIN 34
#define SDA_PIN 21
#define SCL_PIN 22
#define BUZZER_PIN 5
#define BUTTON_PIN 13

// Thresholds
const float FALL_THRESHOLD = 6.0;    // Low G (Free fall)
const float IMPACT_THRESHOLD = 30.0; // High G (Hit ground)

DHT dht(DHTPIN, DHTTYPE);
Adafruit_MPU6050 mpu;
bool alarmActive = false;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Wire.begin(SDA_PIN, SCL_PIN);
  dht.begin();
  if (!mpu.begin()) {
    while (1) { Serial.println("MPU6050 Error!"); delay(1000); }
  }
}

void loop() {
  // 1. Read Sensors
  sensors_event_t a, g, temp_mpu;
  mpu.getEvent(&a, &g, &temp_mpu);
  float temp = dht.readTemperature();

  // 2. Calculate Fall Magnitude (Resultant Vector)
  float totalAcc = sqrt(pow(a.acceleration.x, 2) + 
                        pow(a.acceleration.y, 2) + 
                        pow(a.acceleration.z, 2));

  // 3. Simulate BP & SpO2 using Potentiometer
  int potValue = analogRead(POT_PIN);
  int simSpO2 = map(potValue, 0, 4095, 80, 100);
  int simBP = map(potValue, 0, 4095, 90, 180);

  // 4. Emergency Logic
  if (totalAcc < FALL_THRESHOLD || totalAcc > IMPACT_THRESHOLD || simSpO2 < 92 || temp > 38.0) {
    alarmActive = true;
  }

  // Reset Button
  if (digitalRead(BUTTON_PIN) == LOW) {
    alarmActive = false;
    digitalWrite(BUZZER_PIN, LOW);
  }

  Serial.print("Acc_G:"); Serial.print(totalAcc); Serial.print(",");
  Serial.print("Temp_C:"); Serial.print(temp); Serial.print(",");
  Serial.print("SpO2_pct:"); Serial.print(simSpO2); Serial.print(",");
  Serial.print("BP_mmHg:"); Serial.println(simBP);

  if (alarmActive) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
    delay(50);
  } else {
    delay(100); 
  }
}