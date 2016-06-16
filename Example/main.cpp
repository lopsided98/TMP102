#include <Arduino.h>
#include <TMP102.h>


void setup() {
    Serial.begin(9600);
    Wire.begin();
    TMP102 temp(Wire, TMP102_A0_VCC_ADDR);
    temp.setConversionRate(TMP102::RATE_8_HZ);
    temp.setFaultQueueLength(4);
    temp.setAlertMode(TMP102::COMPARATOR);
    temp.setAlertPolarity(true);
    temp.setConversionMode(TMP102::ONESHOT);
    temp.setAlertHighTemperature(30);
    temp.setAlertLowTemperature(22);
    for(;;) {
        Serial.print("Temperature: ");
        Serial.println(temp.getTemperature());

        if (temp.hasAlert()) {
            Serial.println("Alert!");
        }
        delay(125);
    }
}

void loop() {
}
