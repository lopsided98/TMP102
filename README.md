# TMP102
### Arduino library for the TI TMP102 digital temperature sensor

Simple but full featured library for interacting with the TMP102.

#### Features

* Read temperature
* Sampling rate configuration
* Alert configuration
* Shutdown mode
* Oneshot mode

#### Simple Example
	
```
Wire.begin();
TMP102 sensor;
float temp = sensor.getTemperature();
```

This library uses a modified version of https://github.com/francoiscampbell/arduino-cmake
