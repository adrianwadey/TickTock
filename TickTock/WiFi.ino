#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "CDebounce.h"
//extern "C" {
#include "user_interface.h"
//}

//From wl_definitions.h for reference
//typedef enum {
//    WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
//    WL_IDLE_STATUS      = 0,
//    WL_NO_SSID_AVAIL    = 1,
//    WL_SCAN_COMPLETED   = 2,
//    WL_CONNECTED        = 3,
//    WL_CONNECT_FAILED   = 4,
//    WL_CONNECTION_LOST  = 5,
//    WL_DISCONNECTED     = 6
//} wl_status_t;

typedef enum {
    WIFI_NONE,
    WIFI_NOT_CONNECTED,
    WIFI_CONNECTING,
    WIFI_CONNECTED
} wifi_display_t;

struct clientData {
  char temp[8];
  char humidity[8];
};
extern elapsedSeconds updateTimer;
const int JSONPort = 5050;
WiFiServer server(JSONPort);
String ipStr = "0.0.0.0";

void WiFi_setup(void){
    pinMode(LED_PIN, OUTPUT);
    //analogWriteFreq(100);
    analogWriteRange(255);
    wifi_station_set_hostname(DEVICE_NAME);
    WiFi.mode(WIFI_STA);
    WiFi.begin("","");  //use stored credentials


}

void DisplayWiFiStatus(wifi_display_t state);

elapsedMillis t = 0, r = 0;
wl_status_t prevWiFiStatus = (wl_status_t)-1;
void WiFi_loop(){
    StaticJsonDocument<200> doc;
    wl_status_t WiFiStatus = WiFi.status();
    if (prevWiFiStatus != WiFiStatus){
        prevWiFiStatus = WiFiStatus;
        switch (WiFiStatus){
            case WL_IDLE_STATUS:
                DisplayWiFiStatus(WIFI_NOT_CONNECTED);
                Serial.println("WiFi Idle");
                break;
            case WL_NO_SSID_AVAIL:
                DisplayWiFiStatus(WIFI_NOT_CONNECTED);
                Serial.println("WiFi SSID Not Available");
                break;
            case WL_SCAN_COMPLETED:
                DisplayWiFiStatus(WIFI_CONNECTING);
                Serial.println("WiFi Scan Complete");
                break;
            case WL_CONNECTED:
                DisplayWiFiStatus(WIFI_CONNECTED);
                Serial.println("WiFi Connected");
                Serial.println(WiFi.localIP());
                Serial.println(WiFi.SSID());
                Serial.println(WiFi.macAddress());
                if (!MDNS.begin(DEVICE_NAME)) {
                    Serial.println("Error setting up mDNS responder");
                } else {
                    Serial.print("mDNS responder started ");
                    Serial.println(DEVICE_NAME);
                    //MDNS.addService("http", "tcp", 80);
                }
                updateTimer = 59;
                server.begin();
                break;
            case WL_CONNECT_FAILED:
                DisplayWiFiStatus(WIFI_NOT_CONNECTED);
                Serial.println("WiFi Failed to Connect");
                break;
            case WL_CONNECTION_LOST:
                DisplayWiFiStatus(WIFI_NOT_CONNECTED);
                Serial.println("WiFi Connection Lost");
                break;
            case WL_DISCONNECTED:
                DisplayWiFiStatus(WIFI_CONNECTING);
                Serial.println("WiFi Disconnected");
                break;
        }
    }
    if (WPSSwitch.Changed(LOW)){
        DisplayWiFiStatus(WIFI_CONNECTING);
        Serial.println("Starting WPS");
        analogWrite(LED_PIN, 240);
        WiFi.beginWPSConfig();      //this is blocking
        Serial.println("WPS returned");
        //analogWrite(LED_PIN, 255);  //turn the LED off, if we've connected it will get turned back on
        digitalWrite(LED_PIN, 1);
    }
    //if (ttest > 3000){
    //    ttest = 0;
    //    analogWrite(LED_PIN
    //}
    IPAddress ip = WiFi.localIP();
    ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    MDNS.update();

    WiFiClient client = server.available();
    if (!client) {
        return;
    }
    Serial.println("");
    Serial.println("New Client");

    while (client.connected() && !client.available()){
        delay(1);
    }

    String req = client.readStringUntil('\r');
    Serial.println(req);
    // First line of HTTP request looks like "GET /path HTTP/1.1"
    // Retrieve the "/path" part by finding the spaces
    int addr_start = req.indexOf(' ');
    int addr_end = req.indexOf(' ', addr_start + 1);
    if (addr_start == -1 || addr_end == -1) {
        Serial.print("Invalid request: ");
        Serial.println(req);
        return;
    }
    req = req.substring(addr_start + 1, addr_end);
    Serial.print("Request: ");
    Serial.println(req);
    client.flush();
    
    String s;
    if (req == "/") {
        s = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
//        s += "<!DOCTYPE HTML>\r\n<html>";
//        s += "<head>";
//        s += "<meta http-equiv=\"refresh\" content=\"" + String(DHT_READ_INTERVAL_MS / 1000,0) + "\">";
//        s += "</head>";
//        s += "<body>";
//        s += "Hello from ESP8266 at ";
//        s += ipStr;
//        s += "<br/>";
//        s += "Temperature: " + String(lastValues.temperature,1) + "&deg; C<br/>";
//        s += "Relative Humidity: " + String(lastValues.humidity,0) + "%<br/>";
        // Build your own object tree in memory to store the data you want to send in the request
        doc["sensor"] = "dht11";
        
        JsonObject data = doc.createNestedObject("data");
        data["temperature"] = lastValues.temperature;
        data["humidity"] = lastValues.humidity;
        
        // Generate the JSON string
        serializeJson(doc, s);
        
        //s += "</body>";
        //s += "</html>\r\n\r\n";
        s += "\r\n\r\n";

        Serial.println("Sending 200");
    } else {
        s = "HTTP/1.1 404 Not Found\r\n\r\n";
        Serial.println("Sending 404");
    }
    client.print(s);
    Serial.println("Done with client");
}

void DisplayWiFiStatus(wifi_display_t state){
    switch (state){
        case WIFI_NONE:
            Serial.println("No WiFi");
            analogWrite(LED_PIN, 255);
            //digitalWrite(LED_RED_PIN, 0);
            //digitalWrite(LED_GREEN_PIN, 0);
            //digitalWrite(LED_BLUE_PIN, 0);
            break;
        case WIFI_NOT_CONNECTED:
            Serial.println("WiFi Not Connected");
            analogWrite(LED_PIN, 255);
            //digitalWrite(LED_RED_PIN, 1);
            //digitalWrite(LED_GREEN_PIN, 0);
            //digitalWrite(LED_BLUE_PIN, 0);
            break;
        case WIFI_CONNECTING:
            Serial.println("WiFi Connecting");
            analogWrite(LED_PIN, 240);
            //digitalWrite(LED_RED_PIN, 0);
            //digitalWrite(LED_GREEN_PIN, 0);
            //digitalWrite(LED_BLUE_PIN, 1);
            break;
        case WIFI_CONNECTED:
            Serial.println("WiFi Connected");
            analogWrite(LED_PIN, 0);
            //digitalWrite(LED_RED_PIN, 0);
            //digitalWrite(LED_GREEN_PIN, 1);
            //digitalWrite(LED_BLUE_PIN, 0);
            break;
    }
}
