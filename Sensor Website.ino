#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

String apiKey = "115DSSRJOQNQND4U";
const char *ssid =  "vivo 1901";
const char *password =  "12345678910";
const char* server_ts = "api.thingspeak.com";

#define LED 2       
#define DHTTYPE DHT11

uint8_t DHTPin = D2; 
DHT dht(DHTPin, DHTTYPE); 
int humidity, temperature;

ESP8266WebServer server(80);
WiFiClient client;

const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>
<head>
  <title>TS,HS</title>
</head>

<style>
    body{
        background:black;
    }

    .temp_show{
        position: absolute;
        height: 200px;
        width: 200px;
        background: rgb(189, 61, 189);
        border-radius: 50%;
    }

    .cel{
        left: 375px;
        top: 170px;
    }

    .far{
        right: 400px;
        top: 170px;
    }

    .ctext{
        background: black;
        height: 150px;
        width: 150px;
        left: 26px;
        top: 26px;
        color: rgb(241, 176, 241);
    }

    .ftext{
        background: black;
        height: 150px;
        width: 150px;
        left: 26px;
        top: 26px;
        color: rgb(241, 176, 241);
    }

    .cel_temp{
        position: absolute;
        font-size: 50px;
        font-family: 'Times New Roman', Times, serif;
        left: 45px;
        top: 45px;
    }

    .far_temp{
        position: absolute;
        font-size: 50px;
        font-family: 'Times New Roman', Times, serif;
        left: 45px;
        top: 49px;
    }

    .signc{
        color: rgb(241, 176, 241);;
        position: absolute;
        left: 100px;
        top: 50px;
    }

    .signf{
        color: rgb(241, 176, 241);;
        position: absolute;
        left: 100px;
        top: 50px;
    }

    .text{
        font-size: larger;
        position: absolute;
        top: 200px;
        left: 25px;
    }
</style>
<body>
<div id="time"></div><br>
    <div class="temp_show cel"><div class="temp_show cel ctext">
        <div id="temp" class="cel_temp"></div><div class="signc">C</div> <br>
        <div class="text">Temperature</div>  
    </div></div> 
    <div class="temp_show far"><div class="temp_show far ftext">
        <div id="hum" class="far_temp"></div><div class="signf">%</div> <br>
        <div class="text" style="left: 35px;">Humidity</div>
    </div></div>   

<br>
<br>  
<script>
var Tvalues = [];
var Hvalues = [];
var timeStamp = [];
setInterval(function() {
    getData();
  }, 1000);
 function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //Push the data in array
  var time = new Date().toLocaleTimeString();
  var txt = this.responseText;
  var obj = JSON.parse(txt); 
      Tvalues.push(obj.Temperature);
      Hvalues.push(obj.Humidity);
      timeStamp.push(time);
      
      let t = document.getElementById('temp');
      let h = document.getElementById('hum');
      let tstamp = document.getElementById('time');
      
      h.innerHTML = String(Hvalues[Hvalues.length-1]);
      t.innerHTML = String(Tvalues[Tvalues.length-1]);
      tstamp.innerHTML = String(timeStamp[timeStamp.length-1]);
    }
  };
  xhttp.open("GET", "readData", true);
  xhttp.send();
}



</script>
</body>
</html>

)=====";
 

void handleRoot() {
 String s = MAIN_page;
 server.send(200, "text/html", s);
}

void readData() {

 String data = "{\"Temperature\":\""+ String(temperature) +"\", \"Humidity\":\""+ String(humidity) +"\"}";
 digitalWrite(LED,!digitalRead(LED)); 
 server.send(200, "text/plane", data);
 
 delay(2000);
 
 temperature = dht.readTemperature(); 
 humidity = dht.readHumidity();
}

void setup (){
  
  Serial.begin(9600);
  Serial.println();
  pinMode(DHTPin, INPUT);
  dht.begin();
 
  WiFi.begin(ssid, password);
  Serial.println("");
  //Onboard LED port Direction output
  pinMode(LED,OUTPUT); 
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.on("/readData", readData);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  if (client.connect(server_ts, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";
  
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

  }
  client.stop();
}
