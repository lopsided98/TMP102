set(TMP102_Library_SRCS TMP102.cpp)
set(TMP102_Library_HDRS TMP102.h)

generate_arduino_library(TMP102_Library)
target_include_directories(TMP102_Library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
