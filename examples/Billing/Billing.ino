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
  
  Serial.println("Test billing");
  // Warning: calling /me/bill without filtering can result in a very big list and crash JSON parsing. In this case you'll get empty json_result
  JSONVar bills = ovh.GET("/me/bill?date.from=20211115");
  Serial.print("bills: ");
  Serial.println(bills["json_result"]);
  
  // Get detail for first elemnt
  const char* buffer = bills["json_result"][0];
  Serial.print("Billing # ");
  Serial.println(buffer);
  JSONVar bill_detail = ovh.GET("/me/bill/" + String(buffer));
  Serial.println("Bill detail: ");
  Serial.println(bill_detail);

  // Print date 
  buffer = bill_detail["json_result"]["date"];
  Serial.println("Date: " + String(buffer));
}

void loop() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
}
