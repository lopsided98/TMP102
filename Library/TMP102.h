#ifndef TMP102_H
#define TMP102_H

#include <Arduino.h>
#include <Wire.h>

#define TMP102_A0_GND_ADDR 0x48
#define TMP102_A0_VCC_ADDR 0x49
#define TMP102_A0_SDA_ADDR 0x4A
#define TMP102_A0_SCL_ADDR 0x4B

/**
 * @brief Device driver for TI TMP102 temperature sensor
 *
 * Full featured device driver for the TMP102 temperature sensor. It supports
 * allows easy access to nearly all the features of the sensor.
 *
 * Datasheet: http://www.ti.com/lit/ds/symlink/tmp102.pdf
 */
class TMP102 {
public:
    /**
     * @brief Constructor for TMP102 driver
     *
     * Creates a new driver for the TMP102. A @ref TwoWire object can be
     * specified by the caller to support the dual busses on the Due as well as
     * helping to support multiple devices on the same bus. The @ref TwoWire
     * object must be initialized using begin() before being passed to the
     * constructor.
     *
     * This constructor automatically puts the device into extended mode.
     *
     * @param i2c the I2C bus to use
     * @param address the address of the TMP102 device
     */
    TMP102(TwoWire& i2c = Wire, const uint8_t address = TMP102_A0_GND_ADDR);

    /**
     * @brief The possible temperature sampling rates
     *
     * Enum representing the possible temperature conversion (sampling) rates
     * supported by continuous mode on the TMP102.
     */
    enum ConversionRate {
        RATE_0_25_HZ = 0,
        RATE_1_HZ = 1,
        RATE_4_HZ = 2,
        RATE_8_HZ = 3
    };

    /**
     * @brief The two modes for the alert pin
     *
     * Enum representing the two modes of the alert pin. The meanings of the two
     * modes are described in section 7.5.4 of the datasheet.
     */
    enum AlertMode {
        COMPARATOR = 0,
        INTERRUPT = 1
    };

    /**
     * @brief The two possible conversion modes of the TMP102
     *
     * Enum representing the two conversion (sampling) modes of the TMP102.
     * Continous mode automatically samples at the rate specified, while oneshot
     * mode causes samples to only be taken when requested by the user.
     */
    enum ConversionMode {
        CONTINUOUS = 0,
        ONESHOT = 1
    };

    /**
     * @brief Gets the integer value of the temperature.
     *
     * Returns the raw value of the temperature register, without converting it
     * to a float. The only conversion performed is to convert the 13 bit
     * register into a 16 bit integer. This can be used to avoid the
     * performance overhead of working with floats.
     *
     * This function performs slightly differently depending on what mode the
     * device is using. In continous mode, this function grabs the latest value
     * from the temperature register. This means it is possible (but not
     * beneficial) to call this function faster than the sampling rate of the
     * device.
     *
     * In oneshot mode, this function triggers a new conversion, waits for it to
     * complete, and then returns the result. This means it takes much longer to
     * complete (>=26ms), but calling this function as fast as possible will
     * sample at the maximum rate supported by the device.
     *
     * @return the raw value of the temperature register (0.0625 deg celcius/LSB)
     */
    int16_t getRawTemperature();

    /**
     * @brief Gets the temperature in degrees celcius
     *
     * Gets the temperature from the device in standard units, but with the
     * added overhead of working with floats.
     *
     * This function performs slightly differently depending on what mode the
     * device is using. In continous mode, this function grabs the latest value
     * from the temperature register. This means it is possible (but not
     * beneficial) to call this function faster than the sampling rate of the
     * device.
     *
     * In oneshot mode, this function triggers a new conversion, waits for it to
     * complete, and then returns the result. This means it takes much longer to
     * complete (>=26ms), but calling this function as fast as possible will
     * sample at the maximum rate supported by the device.
     *
     * @return the latest temperatue measured by the sensor
     */
    float getTemperature();

    /**
     * @brief Sets the conversion rate of the device.
     * @param rate the @ref ConversionRate enum element
     */
    void setConversionRate(ConversionRate rate);

    /**
     * @brief Gets the conversion rate of the device
     * @return the conversion rate
     */
    ConversionRate getConversionRate();

    /**
     * @brief Set the length of the fault queue.
     *
     * The fault queue is used to filter noise in the temperature reading for
     * the alert function. To trigger an alert, the temperature must be outside
     * the limits for this many samples to trigger an alert. More information
     * is available in section 7.5.3.4 of the datasheet.
     *
     * @param length the fault queue length (must be 1, 2, 4, or 6)
     * @return false if the length is invalid
     */
    bool setFaultQueueLength(uint8_t length);

    /**
     * @brief Get the length of the fault queue
     *
     * See @ref setFaultQueueLength() for the description of the fault queue.
     *
     * @return the fault queue length
     */
    uint8_t getFaultQueueLength();

    /**
     * @brief Sets the alert mode
     *
     * Sets the alert mode (known as thermostat mode in the datasheet). The two
     * modes are described in section 7.5.4 of the datasheet.
     *
     * @param mode the new alert mode
     */
    void setAlertMode(AlertMode mode);

    /**
     * @brief Gets the current alert mode
     * @return the alert mode
     */
    AlertMode getAlertMode();

    /**
     * @brief Sets the polarity of the alert pin.
     *
     * The polarity of the alert pin controls whether it becomes high or low
     * when activated.
     *
     * @param polarity the polarity (true: active high, false: active low)
     */
    void setAlertPolarity(bool polarity);

    /**
     * @brief Gets the polarity of the alert pin
     * @return the polarity (true: active high, false: active low)
     */
    bool getAlertPolarity();

    /**
     * @brief Sets the high alert threshold
     *
     * @param temp the high threshold temperature in degrees celcius
     */
    void setAlertHighTemperature(float temp);

    /** * @brief Sets the low alert threshold
     *
     * @param temp the low threshold temperature in degrees celcius
     */
    void setAlertLowTemperature(float temp);

    /**
     * @brief Controls whether the device is in shutdown mode.
     *
     * Allows the temperature sensor to go into shutdown mode to save power.
     * According to the datasheet, the sensor consumes less than 0.5 μA in
     * shutdown mode. Shutdown mode disables continous conversion.
     *
     * @param shutdown if true, shutdown the device
     */
    void setShutdown(bool shutdown);

    /**
     * @brief Gets whether the device is in shutdown mode
     * @return true if the device is in shutdown mode
     */
    bool isShutdown();

    /**
     * @brief Sets the conversion mode of the sensor.
     *
     * In continuous mode, the sensor samples at a fixed rate. In oneshot mode,
     * the sensor goes into shutdown mode and only samples each time @ref
     * getTemperature() or @ref getRawTemperature() is called.
     *
     * @param mode the conversion mode
     */
    void setConversionMode(ConversionMode mode);

    /**
     * @brief Gets the conversion mode of the sensor
     * @return the conversion mode
     */
    ConversionMode getConversionMode();

    /**
     * @brief Returns whether the alert has been triggered.
     *
     * Returns whether there is an alert, regardless of the alert polarity.
     *
     * @return true if the alert is active
     */
    bool hasAlert();

    /**
     * @brief Gets the state of the alert pin.
     * @return true if the alert pin is high, false if the alert pin is low
     */
    bool getAlertPin();

private:
    TwoWire& i2c;
    uint16_t config;
    uint8_t pointer;
    uint8_t address;
    ConversionMode conversionMode;

    void writeRegister(uint8_t reg, uint16_t value);
    uint16_t readRegister(uint8_t reg);

    void readConfig();
    void writeConfig();

    static uint16_t floatToTempReg(float temperature);
};

#endif
