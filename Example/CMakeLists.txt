set(TMP102_Example_SRCS main.cpp)
set(TMP102_Example_HDRS )
generate_arduino_firmware(TMP102_Example)
target_link_libraries(TMP102_Example TMP102_Library)
