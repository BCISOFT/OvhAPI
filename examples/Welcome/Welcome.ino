#include <OvhAPI.h>
#include <WiFi.h>

// Wifi credentials
const char* ssid     = "your-ssid";
const char* password = "your-password";

//See https://docs.ovh.com/gb/en/api/first-steps-with-ovh-api/ to generate your keys
const char* OVH_AK = "your Application Key";
const char* OVH_AS = "your Application Secret";
const char* OVH_CK = "your Consumer Key";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  Serial.begin(115200);
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get UTC time (offset=0)
  timeClient.begin(); 
  timeClient.setTimeOffset(0);
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  OvhAPI ovh(&timeClient, ENDPOINT_OVH_EU, OVH_AK, OVH_AS, OVH_CK);
  
  JSONVar result = ovh.GET("/me");
  Serial.print("API return status code: ");
  Serial.println(result["status"]);
  const char* buffer = result["json_result"]["firstname"];
  Serial.println("Welcome " + String(buffer));
}

void loop() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
}
