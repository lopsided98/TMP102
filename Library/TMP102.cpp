#include "TMP102.h"

#define TEMP_REGISTER 0x0
#define CONFIG_REGISTER 0x1
#define LOW_REGISTER 0x2
#define HIGH_REGISTER 0x3

#define CONFIG_EM  _BV(4)
#define CONFIG_AL  _BV(5)
#define CONFIG_CR  (0b11 << 6)
#define CONFIG_SD  _BV(8)
#define CONFIG_TM  _BV(9)
#define CONFIG_POL _BV(10)
#define CONFIG_FQ  (0b11 << 11)
#define CONFIG_OS  _BV(15)

TMP102::TMP102(TwoWire& i2c, const uint8_t address): i2c(i2c),
                                                     config(0b0110000010100000),
                                                     address(address) {
    config |= CONFIG_EM;
    writeConfig();
}

int16_t TMP102::getRawTemperature() {
    // Handle oneshot mode
    if (conversionMode == ONESHOT) {
        // Start a conversion
        config |= CONFIG_OS;
        writeConfig();
        // Conversions are supposed to take 26 ms
        delay(26);
        // Check for conversion ready signal every 500 us
        while (true) {
            readConfig();
            if (config & CONFIG_OS) {
                break;
            }
            delayMicroseconds(500);
        }
    }
    int16_t tempRaw = static_cast<int16_t>(readRegister(TEMP_REGISTER));
    tempRaw = (tempRaw >> 3) & 0xFFFF;
    if(tempRaw & _BV(12)) {
        tempRaw |= 0xF000;
    }
    return tempRaw;
}

float TMP102::getTemperature() {
    return ((float) getRawTemperature()) / 16.0f;
}

void TMP102::setConversionRate(TMP102::ConversionRate rate) {
    config = (config & ~CONFIG_CR) | (static_cast<uint8_t> (rate) << 6);
    writeConfig();
}

TMP102::ConversionRate TMP102::getConversionRate() {
    return static_cast<TMP102::ConversionRate>((config & CONFIG_CR) >> 6);
}

bool TMP102::setFaultQueueLength(uint8_t length) {
    uint16_t configMask;
    switch(length) {
    case 1:
        configMask = 0;
        break;
    case 2:
        configMask = 1;
        break;
    case 4:
        configMask = 2;
        break;
    case 6:
        configMask = 3;
        break;
    default:
        return false;
    }

    config = (config & ~CONFIG_FQ) | (configMask << 11);
    writeConfig();
    return true;
}

uint8_t TMP102::getFaultQueueLength() {
    uint8_t length = (config & CONFIG_FQ) >> 11;
    if (length == 0) {
        length = 1;
    } else {
        length *= 2;
    }
    return length;
}

void TMP102::setAlertMode(AlertMode mode) {
    config = (config & ~CONFIG_TM) | (static_cast<uint16_t>(mode) << 9);
}

TMP102::AlertMode TMP102::getAlertMode() {
    return static_cast<TMP102::AlertMode>((config & CONFIG_TM) != 0);
}

void TMP102::setAlertPolarity(bool polarity) {
    config = (config & ~CONFIG_POL) | (polarity << 10);
}

bool TMP102::getAlertPolarity() {
    return config & CONFIG_POL;
}

void TMP102::setAlertHighTemperature(float temp) {
    writeRegister(HIGH_REGISTER, floatToTempReg(temp));
}

void TMP102::setAlertLowTemperature(float temp) {
    writeRegister(LOW_REGISTER, floatToTempReg(temp));
}

void TMP102::setShutdown(bool shutdown) {
    config = (config & ~CONFIG_SD) | (shutdown << 8);
    if(!shutdown) {
        conversionMode = CONTINUOUS;
    }
    writeConfig();
}

bool TMP102::isShutdown() {
    return config & CONFIG_SD;
}

void TMP102::setConversionMode(ConversionMode mode) {
    setShutdown(mode == ONESHOT);
    conversionMode = mode;
}

TMP102::ConversionMode TMP102::getConversionMode() {
    return conversionMode;
}

bool TMP102::hasAlert() {
    if (getAlertPolarity()) {
        return getAlertPin();
    } else {
        return !getAlertPin();
    }
}

bool TMP102::getAlertPin() {
    readConfig();
    return config & CONFIG_AL;
}

void TMP102::writeRegister(uint8_t reg, uint16_t value) {
    i2c.beginTransmission(address);
    pointer = reg;
    i2c.write(reg);
    i2c.write((value >> 8) & 0xFF);
    i2c.write(value & 0xFF);
    i2c.endTransmission(true);
}

uint16_t TMP102::readRegister(uint8_t reg) {
    if (pointer != reg) {
        pointer = reg;
        i2c.beginTransmission(address);
        i2c.write(reg);
        i2c.endTransmission(false);
    }
    i2c.requestFrom(address, 2, true);

    return (i2c.read() << 8) | i2c.read();
}

void TMP102::readConfig() {
    config = readRegister(CONFIG_REGISTER);
}

void TMP102::writeConfig() {
    writeRegister(CONFIG_REGISTER, config);
}

uint16_t TMP102::floatToTempReg(float temperature) {
    int16_t data = static_cast<int16_t>(temperature * 16);
    return static_cast<uint16_t>(data << 3);
}
