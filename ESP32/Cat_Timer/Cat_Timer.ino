#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// =======================================================
// ⚠️ 1. CONFIGURATION: UPDATE THESE THREE VALUES ⚠️
// =======================================================
const char* WIFI_SSID = "YOUR_WIFI_SSID";           // Your home Wi-Fi network name
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";   // Your home Wi-Fi password

// **CRITICAL:** Use the IPv4 address you found with ipconfig/ifconfig
const char* SERVER_IP = "YOUR_PC_IPV4_ADDRESS";     
// Example: "192.168.1.50" 

const int SERVER_PORT = 3000;                     // Must match your Node.js port
const char* SERVER_ENDPOINT = "/log-timer";       // Must match your Node.js route

// =======================================================
// 2. DEVICE AND LOGGING CONFIGS
// =======================================================
const char* ESP_ID = "Nodemcu_CatTracker_V1";
const unsigned long LOG_INTERVAL_MS = 5000;       // Log data every 5 seconds (5000 ms)

// Array of cat activities to simulate/log
const char* catEvents[] = {
  "Cat Eating",
  "Cat Sleeping",
  "Cat Running Zoomies",
  "Cat Chasing Toy",
  "Cat Begging for Food",
  "Cat Hiding in Box",
  "Cat Knocking Object Off Table",
  "Cat Meowing Loudly"
};
const int NUM_EVENTS = sizeof(catEvents) / sizeof(catEvents[0]);

unsigned long lastLogTime = 0;
unsigned long startTime = 0;

// =======================================================
// 3. SETUP
// =======================================================
void setup() {
  Serial.begin(115200);
  delay(100);

  // --- WiFi Connection ---
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("ESP IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Server URL: http://");
  Serial.print(SERVER_IP);
  Serial.print(":");
  Serial.print(SERVER_PORT);
  Serial.println(SERVER_ENDPOINT);
  
  startTime = millis();
  lastLogTime = startTime;
  Serial.println("\nCat Activity Tracker Started!");
}

// =======================================================
// 4. MAIN LOOP
// =======================================================
void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastLogTime >= LOG_INTERVAL_MS) {
    postCatActivity(currentTime - startTime); // Pass total duration
    lastLogTime = currentTime;
  }

  // Add a small delay to keep the loop from running too fast
  delay(10);
}

// =======================================================
// 5. POST DATA FUNCTION
// =======================================================
void postCatActivity(unsigned long timerDuration_ms) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.reconnect();
    return;
  }

  // --- 1. Build the JSON Payload ---
  // Select a random event from the array
  int randomIndex = random(NUM_EVENTS);
  const char* eventName = catEvents[randomIndex];

  StaticJsonDocument<200> doc;
  doc["timerDuration_ms"] = timerDuration_ms;
  doc["eventName"] = eventName;
  doc["espId"] = ESP_ID;
  
  String jsonPayload;
  serializeJson(doc, jsonPayload);

  // --- 2. Assemble the URL ---
  HTTPClient http;
  String serverUrl = "http://" + String(SERVER_IP) + ":" + String(SERVER_PORT) + String(SERVER_ENDPOINT);

  Serial.print("POSTing [");
  Serial.print(timerDuration_ms / 1000);
  Serial.print("s]: ");
  Serial.println(jsonPayload);

  // --- 3. Execute the POST Request ---
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  // This is the critical line!
  int httpResponseCode = http.POST(jsonPayload); 

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("HTTP Code: ");
    Serial.print(httpResponseCode);
    Serial.print(", Response: ");
    Serial.println(response);
  } else {
    Serial.print("HTTP Failed: ");
    Serial.println(http.errorToString(httpResponseCode).c_str());
  }

  http.end();
}