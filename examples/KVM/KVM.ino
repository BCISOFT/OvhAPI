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
  
  // Get KVM acces to the first bare metal server
  // Not really usefull on an ESP32 but demonstrate using POST
  Serial.println("Test KVM");

  JSONVar servers = ovh.GET("/dedicated/server");
  Serial.print("Serveurs: ");
  Serial.println(servers);

  int status = servers["status"];
  if (status != 200) {
    Serial.println("Quit");
    return;
  }
  
  const char* buffer = servers["json_result"][0];
  Serial.print("Server: ");
  Serial.println(buffer);

  HTTPClient http;
  http.begin("https://ifconfig.me/ip");
  int httpResponseCode = http.GET();
  String localip = http.getString();
  http.end();
  
  Serial.print("httpResponseCode: ");
  Serial.println(httpResponseCode);
  
  if (httpResponseCode != 200) {
    Serial.print("Err message: ");
    Serial.println(localip);
    return;
  }
  
  Serial.print("Public IP: ");
  Serial.println(localip);

  String post_url = String("/dedicated/server/" + String(buffer) + "/features/ipmi/access");
  Serial.print("    post_url: ");
  Serial.println(post_url);

  JSONVar payload;
  payload["ipToAllow"] = localip;
  payload["ttl"] = 15;
  payload["type"] = "kvmipJnlp";
  Serial.print("    payload: ");
  Serial.println(JSONVar::stringify(payload).c_str());
  
  JSONVar kvmaccess = ovh.POST(post_url.c_str(), JSONVar::stringify(payload).c_str());
  Serial.print("Serial access: ");
  Serial.println(kvmaccess);
  int rtncode = kvmaccess["status"];
  Serial.print("rtncode: ");
  Serial.println(String(rtncode).c_str());
  
  if (rtncode == 200 || rtncode == 0) {
    int loop_count = 0;
    do {
      delay(3000);
      JSONVar kvm = ovh.GET(String(post_url + "?type=kvmipJnlp").c_str());
      Serial.print("KVM: ");
      Serial.println(kvm);
      status = kvm["status"];
      if (status != 404) break;
    } while(loop_count++ < 5);
  }

}

void loop() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
}
