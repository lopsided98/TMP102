#=============================================================================#
# Author: Ben Wolsieffer                                                      #
# Date:   07.13.2016                                                          #
#                                                                             #
# Description: Arduino library for the TI TMP102 digital temperature sensor   #
#                                                                             #
#=============================================================================#
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(TMP102 C CXX)

set(ARDUINO_DEFAULT_BOARD uno) # Default Board ID, when not specified
set(ARDUINO_DEFAULT_PORT /dev/ttyACM0) # Default Port, when not specified

add_subdirectory(Example) # Library example sketch
add_subdirectory(Library) # Library implementation
