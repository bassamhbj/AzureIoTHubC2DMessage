cmake_minimum_required(VERSION 2.8.11)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_C_FLAGS "--std=c99 -g ${CMAKE_C_FLAGS}")
endif()

include_directories("/usr/local/include/azureiot"
                    "/usr/local/include/azureiot/inc/")

set(SOURCE iot-device.c parson.c parson.h electric_shock.h electric_shock.c)
add_executable(iot-device-quiz ${SOURCE})
target_link_libraries(iot-device-quiz 
                          serializer
                          iothub_client
                          iothub_client_mqtt_transport
                          umqtt
                          aziotsharedutil
                          ssl
                          crypto
                          curl
                          pthread
                          m
                          ssl
                          crypto)