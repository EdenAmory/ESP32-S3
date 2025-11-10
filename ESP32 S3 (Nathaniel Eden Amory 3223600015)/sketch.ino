#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

#define LED_PIN        2
#define BUZZER_PIN     46
#define BUTTON_A       35
#define BUTTON_B       19
#define POT_INPUT      5
#define ENC_CLK        41
#define ENC_DT         40
#define OLED_SDA       38
#define OLED_SCL       39
#define SERVO_PIN      18

#define STP1 4
#define STP2 7
#define STP3 8
#define STP4 9

// OLED driver
Adafruit_SSD1306 screen(128, 64, &Wire, -1);

// Servo
Servo myservo;

// Stepper
AccelStepper motor(AccelStepper::FULL4WIRE, STP1, STP3, STP2, STP4);

// Encoder
volatile int encValue = 0;
int prevClk = 0;


// ======================= LED TASK ==========================
void taskLED(void *pv) {
  pinMode(LED_PIN, OUTPUT);
  while (1) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    Serial.printf("[LED] Core %d\n", xPortGetCoreID());
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// ======================= BUZZER TASK ==========================
void taskBuzzer(void *pv) {
  pinMode(BUZZER_PIN, OUTPUT);
  while (1) {
    digitalWrite(BUZZER_PIN, HIGH);
    vTaskDelay(200);
    digitalWrite(BUZZER_PIN, LOW);
    vTaskDelay(200);
    Serial.printf("[BUZZER] Core %d\n", xPortGetCoreID());
  }
}

// ======================= BUTTON TASK ==========================
void taskButton(void *pv) {
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  while (1) {
    Serial.printf("[BUTTON] A=%d B=%d Core %d\n",
                  digitalRead(BUTTON_A),
                  digitalRead(BUTTON_B),
                  xPortGetCoreID());
    vTaskDelay(200);
  }
}

// ======================= POT TASK ==========================
void taskPot(void *pv) {
  while (1) {
    int val = analogRead(POT_INPUT);
    Serial.printf("[POT] %d Core %d\n", val, xPortGetCoreID());
    vTaskDelay(200);
  }
}

// ======================= OLED TASK ==========================
void taskOLED(void *pv) {
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!screen.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("[OLED] Failed");
    vTaskDelete(NULL);
  }

  while (1) {
    screen.clearDisplay();
    screen.setTextSize(1);
    screen.setTextColor(SSD1306_WHITE);
    screen.setCursor(0, 0);
    screen.printf("OLED Running\nCore %d", xPortGetCoreID());
    screen.display();

    vTaskDelay(500);
  }
}

// ======================= ENCODER TASK ==========================
void taskEncoder(void *pv) {
  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT, INPUT);

  prevClk = digitalRead(ENC_CLK);

  while (1) {
    int clk = digitalRead(ENC_CLK);
    if (clk != prevClk) {
      if (digitalRead(ENC_DT) == clk) encValue++; else encValue--;
      Serial.printf("[ENC] %d Core %d\n", encValue, xPortGetCoreID());
    }
    prevClk = clk;
    vTaskDelay(5);
  }
}

// ======================= SERVO TASK ==========================
void taskServo(void *pv) {
  myservo.attach(SERVO_PIN);

  while (1) {
    for (int p = 20; p <= 160; p += 4) {
      myservo.write(p);
      vTaskDelay(15);
    }
    for (int p = 160; p >= 20; p -= 4) {
      myservo.write(p);
      vTaskDelay(15);
    }

    Serial.printf("[SERVO] Core %d\n", xPortGetCoreID());
  }
}

// ======================= STEPPER TASK ==========================
void taskStepper(void *pv) {
  motor.setMaxSpeed(500);
  motor.setAcceleration(200);

  while (1) {
    motor.moveTo(300);
    while (motor.distanceToGo()) motor.run();
    motor.moveTo(-150);
    while (motor.distanceToGo()) motor.run();

    Serial.printf("[STEPPER] Core %d\n", xPortGetCoreID());
  }
}

// ======================= SETUP ==========================
void setup() {
  Serial.begin(115200);
  delay(300);

  // LED
  xTaskCreatePinnedToCore(taskLED, "taskLED", 4000, NULL, 1, NULL, 1);

  // BUZZER
  //xTaskCreatePinnedToCore(taskBuzzer, "taskBuzzer", 4000, NULL, 1, NULL, 1);

  // BUTTON
  //xTaskCreatePinnedToCore(taskButton, "taskButton", 4000, NULL, 1, NULL, 0);

  // POT
  //xTaskCreatePinnedToCore(taskPot, "taskPot", 4000, NULL, 1, NULL, 1);

  // OLED
  //xTaskCreatePinnedToCore(taskOLED, "taskOLED", 6000, NULL, 1, NULL, 1);

  // ENCODER
  //xTaskCreatePinnedToCore(taskEncoder, "taskEncoder", 4000, NULL, 1, NULL, 1);

  // SERVO
  //xTaskCreatePinnedToCore(taskServo, "taskServo", 5000, NULL, 1, NULL, 1);

  // STEPPER
  //xTaskCreatePinnedToCore(taskStepper, "taskStepper", 7000, NULL, 1, NULL, 1);
}

// ======================== LOOP ==========================
void loop() {}