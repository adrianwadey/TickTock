#include <NTPClient.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

// change next line to use with another board/shield
//#include <ESP8266WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>


WiFiUDP ntpUDP;
elapsedMillis refreshTimer;
elapsedSeconds updateTimer = 53;    //give it 5s to connect, then fetch time

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void NTP_setup(){
  timeClient.begin();
  refreshTimer = 0;
}

void NTP_loop() {
    if (refreshTimer < 1000) return;
    refreshTimer = 0;
    if (updateTimer > 59){
        timeClient.update();
        if (timeClient.getEpochTime() > 60*60*24*365) {   //if time < 1 yr (approx) then not valid yet
            setTime(timeClient.getEpochTime());
            UpdateTimeZone();
        }
        updateTimer = 0;
        Serial.println(timeClient.getFormattedTime());
    }
}
