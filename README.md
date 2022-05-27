# TickTock
NodeMCU 1 Button Internet Clock built using Arduino IDE

## Hardware
 * NodeMCU
 * 2 x 16 LCD
 * DHT11 Temperature & Humidity Sensor

## Features
Date, time, temperature and humidity are displayed on a 2 line LCD.

Single button operation, used to start WPS.

Once connected to network it fetches the time using NTP.

It uses http://worldtimeapi.org to identify location, timezone and DST data from current IP address.

It has a web server to allow remote browsing of the sensor data.
