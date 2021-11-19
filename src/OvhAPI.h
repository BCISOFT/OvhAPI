/*
  ovhapi.h - Library for using OVH API.
  Created by David A. Mellis, November 19, 2021.
  Released into the public domain.
*/
#ifndef OvhApi_h
#define OvhApi_h

#include <stdarg.h>
#include "Arduino.h"
#include <NTPClient.h>          // https://github.com/taranais/NTPClient
#include <HTTPClient.h>         // https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient/src
#include <Arduino_JSON.h>       // https://github.com/arduino-libraries/Arduino_JSON

#include "mbedtls/md.h"

#define ENDPOINT_OVH_EU            0
#define ENDPOINT_OVH_US            1
#define ENDPOINT_OVH_CA            2
#define ENDPOINT_KIMSUFI_EU        3
#define ENDPOINT_KIMSUFI_CA        4
#define ENDPOINT_SOYOUSTART_EU     5
#define ENDPOINT_SOYOUSTART_CA     6

#define noOVH_DEBUG

#ifdef OVH_DEBUG
  #define OVH_DEBUG_PRINTLN(x) Serial.println(x)
  #define OVH_DEBUG_PRINTLN2(x,y) Serial.println(x,y)
  #define OVH_DEBUG_PRINT(x) Serial.print(x)
  #define OVH_DEBUG_PRINT2(x,y) Serial.print(x,y)
#else
  #define OVH_DEBUG_PRINTLN(x)
  #define OVH_DEBUG_PRINTLN2(x,y)
  #define OVH_DEBUG_PRINT(x)
  #define OVH_DEBUG_PRINT2(x,y)
#endif

struct response {
  int code;
  String data;
};

class OvhAPI
{
  public:
    OvhAPI(NTPClient* timeClient, int endpoint, const char* application_key, const char* application_secret, const char* consumer_key);
    
    JSONVar GET(const char* _target, bool _need_auth=true);
    JSONVar GET(String& _target, bool _need_auth=true);
    
    JSONVar PUT(const char* _target, const char* payload, bool _need_auth=true);
    JSONVar PUT(String& _target, String& payload, bool _need_auth=true);
    
    JSONVar POST(const char* _target, const char* payload, bool _need_auth=true);
    JSONVar POST(String& _target, String& payload, bool _need_auth=true);
    
    JSONVar DELETE(const char* _target, bool _need_auth=true);
    JSONVar DELETE(String& _target, bool _need_auth=true);

  private:
    const char* _endpoint;
    const char* _application_key;
    const char* _application_secret;
    const char* _consumer_key;
    NTPClient* _timeClient;
    bool _time_delta_read = false;
    int _time_delta;

    String sha1(String payloadStr);
    int time_delta();

    JSONVar call(const char* method, const char* path, const char* data="", bool need_auth=true);
    struct response raw_call(const char* method, const char* path, const char* data="", bool need_auth=true);
};

#endif
