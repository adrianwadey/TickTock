#include <ESP8266WebServer.h>

ESP8266WebServer Web(80);
extern String ipStr;
extern const int JSONPort;
void WebServer_setup(){
    Web.on("/", handle_index); //Handle Index page
    Web.on("/live", handle_live); //Handle Index page
  
    Web.begin(); //Start the server
    Serial.println("Server listening");
}

void WebServer_loop(){
      Web.handleClient(); //Handling of incoming client requests
}

void handle_index(){
    String WebPage = "";
    WebPage += "<!DOCTYPE HTML>\r\n";
    WebPage += "<html>\r\n";
    WebPage += "<head>\r\n";
    //WebPage += "<meta http-equiv=\"refresh\" content=\"" + String(DHT_READ_INTERVAL_MS / 1000,0) + "\">\r\n";
    WebPage += "<title>Blowey</title>";
    WebPage += "</head>\r\n";
    WebPage += "<body>\r\n";
    WebPage += "<h1>Hello from " DEVICE_NAME " at ";
    WebPage += ipStr;
    WebPage += "</h1><br/>";
    WebPage += "Temperature: " + String(lastValues.temperature,1) + "&deg; C<br/>";
    WebPage += "Relative Humidity: " + String(lastValues.humidity,0) + "%<br/>";
    WebPage += "<br/>";
    WebPage += "<p>JSON data available on Port " + String(JSONPort);
    WebPage += "</p>";
    WebPage += "</body>\r\n";
    WebPage += "</html>\r\n";
    
    Web.send(200, "text/html", WebPage);
}

void handle_live(){
    handle_index();
}
