// Adapted from:
// (c) Copyright 2010-2012 MCQN Ltd.
// Released under Apache License, version 2.0
//
// Simple example to show how to use the HttpClient library
// Get's the web page given at http://<kHostname><kPath> and
// outputs the content to the serial port

//
// This really needs to be made asynchronous otherwise everything else stops while the
// HTTP GET is performed
//

#include <ArduinoJson.h>
#include <WiFiClient.h>
#include <HttpClient.h>

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

String json;
StaticJsonDocument<1000> doc;
bool timezonefetched = false;
int dst_offset;
int tz_offset;
String dst_from;
String dst_until;

// ntp is always in UCT
// Calculate time zone adjustment
// Calculate DST adjustment
void UpdateTimeZone()
{
    bool dst = false;
    time_t tnow = now();
    //             2022-03-27T01:00:00+00:00
    char dstr[] = "2022-03-27T01:00:00+00:00    ";
    sprintf(dstr, "%d-%2.2d-%2.2dT%2.2d:%2.2d:%2.2d+00:00", year(tnow), month(tnow), day(tnow), hour(tnow), minute(tnow), second(tnow));
    if (timezonefetched){
        if (strcmp(dstr, dst_from.c_str()) > 0 && strcmp(dstr, dst_until.c_str()) > 0) timezonefetched = false;   //after both dst dates, so get latest dst
    }
    if (!timezonefetched){
        int err = HttpGet("worldtimeapi.org", "/api/ip");   //from our ISP IP address

        //use the following to test from specific IP addresses to check different time zones
        //int err = HttpGet("worldtimeapi.org", "/api/ip/168.0.134.38");    //America/Sao_Paulo      //Melbourne 2404:8280:a222:bbbb:bba2:3:ffff:ffff
        Serial.print("HttpGet returned ");
        Serial.println(err);
        if (err) return;
        DeserializationError error = deserializeJson(doc, json);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        dst_offset = doc["dst_offset"];
        tz_offset = doc["raw_offset"];
        String t1 = doc["dst_from"];
        dst_from = t1;
        String t2 = doc["dst_until"];
        dst_until = t2;
        //Serial.println(dst);
        //Serial.println(dst_from);
        //Serial.println(dst_until);
        //Serial.println(dst_offset);
        //Serial.println(now());
        timezonefetched = true;
    }
    //update DST
    if (strcmp(dst_until.c_str(), dst_from.c_str()) > 0){     //end is after after start
        dst = false;
        if (strcmp(dstr, dst_from.c_str()) > 0) dst = true;     //after start of dst
        if (strcmp(dstr, dst_until.c_str()) > 0) dst = false;   //after end of dst
    } else {
        dst = true;
        if (strcmp(dstr, dst_until.c_str()) > 0) dst = false;   //after end of dst
        if (strcmp(dstr, dst_from.c_str()) > 0) dst = true;     //after start of dst
    }
    Serial.println(dst);
    if (dst){
        adjustTime(tz_offset + dst_offset);
    } else {
        adjustTime(tz_offset + 0);
    }
}

int HttpGet(const char *host, const char *path){
  int err =0;
  
  WiFiClient c;
  HttpClient http(c);

  err = http.get(host, path);
  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                Serial.print(c);
                json += c;
                
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
        Serial.println();
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
    return err;
}
