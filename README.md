# AZ-RH7722AM-EthernetESP32
Az Instruments RH7722 CO2&amp;Humidity&amp;Temperature meter with AM interface and Ethernet connection ESP32 based
Simple device that takes RS323 Out from AZ RH7722 and sends data to AM Server over Ethernet connection.
The device create fife "analog" devices - CO2, Humidity,Temperature, DewPoint, WetBulb for AM Server and refresh data reading them from serial port.
It uses new ESP32 function onReceive - based on RX interrupt from serial port of ESP32.
