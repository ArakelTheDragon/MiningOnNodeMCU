#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "sha3.h"

const char* ssid     = "TP-Link_CCC7";
const char* password = "69538001";

const char* poolHost = "xmr-eu1.nanopool.org"; // Replace with your mining pool address
const int poolPort = 10300;                  // Replace with your mining pool port
const char* walletAddress = "1112"; // Replace with your wallet address

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Connect to mining pool
  if (!client.connect(poolHost, poolPort)) {
    Serial.println("Connection to mining pool failed");
    return;
  }
  Serial.println("Connected to mining pool");

  // Send login request
  sendLoginRequest();
}

void loop() {
  if (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println("Received: " + line);

    // Parse JSON response
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, line);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Handle mining job
    if (doc["method"] == "job") {
      String jobId = doc["params"]["job_id"];
      String blob = doc["params"]["blob"];
      String target = doc["params"]["target"];
      
      Serial.println("New job received:");
      Serial.println("Job ID: " + jobId);
      Serial.println("Blob: " + blob);
      Serial.println("Target: " + target);
      
      // Perform mining work (hashing)
      String result = performMining(blob, target);
      print (result);

      // Submit result
      if (result != "") {
        sendResult(jobId, result);
      }
    }
  }
}

void sendLoginRequest() {
  DynamicJsonDocument doc(1024);
  doc["method"] = "login";
  doc["params"]["login"] = walletAddress;
  doc["params"]["pass"] = "x";
  doc["id"] = 1;

  String request;
  serializeJson(doc, request);

  client.println(request);
  Serial.println("Sent login request: " + request);
}

String performMining(String blob, String target) {
  // This is a placeholder for the actual mining work
  // In a real implementation, you would perform the hashing here
  // using the blob and target values to find a valid nonce.

  // For demonstration, we'll just return an empty result
  return "";
}

void sendResult(String jobId, String result) {
  DynamicJsonDocument doc(1024);
  doc["method"] = "submit";
  doc["params"]["id"] = walletAddress;
  doc["params"]["job_id"] = jobId;
  doc["params"]["result"] = result;
  doc["id"] = 1;

  String request;
  serializeJson(doc, request);

  client.println(request);
  Serial.println("Sent result: " + request);
}
