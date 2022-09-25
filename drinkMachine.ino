#include <ArduinoQueue.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

#include <AccelStepper.h>
#include <MultiStepper.h>
#include "Carrier.h"
#include "Station.h"
#include <ArduinoJson.h>
#include <Stepper.h>
#include <SPI.h>

int stepPin = 0; //D3
int dirPin = 2; //D4
int stepsPerRev = 200;

int homeSwitch = 14; //D5
int moveFinished = 1;
int initialHoming = -1;

int fsrPin = A0;
int fsrReading;
int zeroed;

int numLiquor;
int numMixer;
int MAXDRINKS = 5;
ArduinoQueue<String> drinkRequest(MAXDRINKS);

String lastRequest;

Carrier carrier(0);
AccelStepper stepperX(1, stepPin, dirPin);

#ifndef APSSID
#define APSSID "BarBot"
#endif

const char *ssid = APSSID;
const char* DomainName = "barbot";

const char* PARAM_INPUT_1 = "numLiquor";
const char* PARAM_INPUT_2 = "numMixer";
const char* ALCOHOL_REQUEST = "alcoholic";
const char* NONALCOHOL_REQUEST = "nonalcoholic";

//ESP8266WebServer server(80);
AsyncWebServer server(80);


String processor(const String& var) {
  //Serial.println(var);
  if (var == "DRINK") {
    return drinkRequest.getHead();
  }

  if (var == "QUEUE") {
    return String(drinkRequest.itemCount());
  }

  if (var == "SYSTEM") {
    //    return String barbotSystem;
  }

  if (var == "STATUS") {
    if(!drinkRequest.isEmpty()){
      return "Making drink";
    }
    return "Idle";
  }
}

String setCurrentDrink() {
  String drink;
  if(drinkRequest.isEmpty()){
    drink = "Nothing";
  }
  else{
    drink = drinkRequest.getHead();
  }
  return drink;
}

String getDrinkQueue() {
  String queue = String(drinkRequest.itemCount());
  return queue;
}

void setup() {
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  delay(10000);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //  server.on("/", handleRoot);
  Serial.println("HTTP server started");

  //  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //    request->send(200, "text/html", index_html);
  //  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/drinks.html", String(), false, processor);
  });

  server.on("/drinks", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/drinks.html", String(), false, processor);
  });

  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/setup.html", "text/html");
  });

  server.on("/about", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/about.html", "text/html");
  });

  server.on("/drinks.json", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/drinks.json", "text/plain");
  });

  server.on("/app.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/app.js", "text/plain");
  });

  server.on("/jquery-3.6.0.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/jquery-3.6.0.min.js", "text/plain");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/currentdrink", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", setCurrentDrink().c_str());
  });

  server.on("/drinkqueue", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", getDrinkQueue().c_str());
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "text/plain", String(), false, processor);
  });

  //  server.on("/drinkrequest", HTTP_GET, [](AsyncWebServerRequest *request){
  //    String lastRequest = request->getParam(DRINKREQUEST)->value();
  //    Serial.println(lastRequest);
  //    request->send("/", "text/plain", "message received");
  //  });


  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(ALCOHOL_REQUEST)) {
      lastRequest = request->getParam(ALCOHOL_REQUEST)->value();
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(NONALCOHOL_REQUEST)) {
      lastRequest = request->getParam(NONALCOHOL_REQUEST)->value();
    }
    else {
      lastRequest = "No message sent";
    }
    //    Serial.println(lastRequest);
    drinkRequest.enqueue(lastRequest);
    Serial.println(drinkRequest.getHead());
    Serial.println(drinkRequest.itemCount());
    //    getDrinkRequest(lastRequest);
    request->send(SPIFFS, "/drinks.html", String(), false, processor);
  });


  if (!MDNS.begin("barbot")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  server.onNotFound(notFound);
  server.begin();
  delay(1000);

  // Prompt user for inputs, maybe
  initialize();
}

void loop() {
  MDNS.update();
  carrier.moveCarrier(stepPin, dirPin);

  delay(10000);
  drinkRequest.dequeue();
  
}

//void loadDrinksJSON(){
//  const char *filename = "/drinks.json";
//  File file = SD.open(filename);
//  StaticJsonDocument<512> doc;
//  DeserializationError error = deserializeJson(doc, file);
//  if (error)
//    Serial.println(F("Failed to read file"));
//
//  file.close();
//}

void getAvailableDrinks() {
  numLiquor = 0;
  numMixer = 0;
  Serial.println("How many liquors will be used?");
  while (numLiquor == 0) {
    numLiquor = Serial.parseInt();
  }
  Serial.println("How many mixers will be used?");
  while (numMixer == 0) {
    numMixer = Serial.parseInt();
  }
  // prompt user for liquors/mixers and the slot they are in
  // update availableDrinksList
}

void initialize() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(homeSwitch, INPUT);
//  carrier.homeCarrier(homeSwitch, initialHoming, stepperX);
  //zeroed = carrier.zeroCarrier(fsrPin);

  //  loadDrinksJSON();
  Serial.println("Initialization is complete");
}

void startMDNS() {
  if (!MDNS.begin(DomainName, WiFi.softAPIP())) {
    Serial.println("[ERROR] MDNS responder did not setup");
    while (1) {
      delay(1000);
    }
  } else {
    Serial.println("[INFO] MDNS setup is successful!");
  }
}



void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/html", "Not found");
}
