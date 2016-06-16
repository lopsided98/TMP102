#include <Arduino.h>
#include <TMP102.h>


void setup() {
    Serial.begin(9600);
    Wire.begin();
    TMP102 temp(Wire, TMP102_A0_VCC_ADDR);
    temp.setConversionRate(TMP102::RATE_8_HZ);
    temp.setFaultQueueLength(1);
    temp.setAlertMode(TMP102::INTERRUPT);
    temp.setAlertPolarity(false);
    temp.setConversionMode(TMP102::ONESHOT);
    Serial.print("Conversion Rate: ");
    Serial.println(temp.getConversionRate());
    Serial.print("Fault Queue Length: ");
    Serial.println(temp.getFaultQueueLength());
    Serial.print("Alert Mode: ");
    Serial.println(temp.getAlertMode());
    Serial.print("Alert Polarity: ");
    Serial.println(temp.getAlertPolarity());
    for(int i = 0;; i++) {
        Serial.print("Temperature: ");
        Serial.println(temp.getTemperature());
    }
}

void loop() {
}
