#include "Arduino.h"
#include <OvhAPI.h>


OvhAPI::OvhAPI(NTPClient* timeClient, int endpoint, const char* application_key, const char* application_secret, const char* consumer_key)
{
  switch(endpoint) {

    case ENDPOINT_OVH_US:
      _endpoint = "https://api.us.ovhcloud.com/1.0";
      break;

    case ENDPOINT_OVH_CA:
      _endpoint = "https://api.us.ovhcloud.com/1.0";
      break;

    case ENDPOINT_KIMSUFI_EU:
      _endpoint = "https://eu.api.kimsufi.com/1.0";
      break;

    case ENDPOINT_KIMSUFI_CA:
      _endpoint = "https://ca.api.kimsufi.com/1.0";
      break;

    case ENDPOINT_SOYOUSTART_EU:
      _endpoint = "https://eu.api.soyoustart.com/1.0";
      break;
      
    case ENDPOINT_SOYOUSTART_CA:
      _endpoint = "https://ca.api.soyoustart.com/1.0";
      break;

    case ENDPOINT_OVH_EU:
    default:
      _endpoint = "https://eu.api.ovh.com/1.0";
      break;
  }
  _application_key = application_key;
  _application_secret = application_secret;
  _consumer_key = consumer_key;
  _timeClient = timeClient;
  _http = new HTTPClient();

  // OVH_DEBUG_PRINT("OvhAPI::OvhAPI _endpoint= ");OVH_DEBUG_PRINTLN(_endpoint);
  // OVH_DEBUG_PRINT("OvhAPI::OvhAPI _application_key= ");OVH_DEBUG_PRINTLN(_application_key);
  // OVH_DEBUG_PRINT("OvhAPI::OvhAPI _application_secret= ");OVH_DEBUG_PRINTLN(_application_secret);
  // OVH_DEBUG_PRINT("OvhAPI::OvhAPI _consumer_key= ");OVH_DEBUG_PRINTLN(_consumer_key);
}

String OvhAPI::sha1(String payloadStr){
    const char *payload = payloadStr.c_str();
 
    int size = 20;
 
    byte shaResult[size];
 
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;
 
    const size_t payloadLength = strlen(payload);
 
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char *) payload, payloadLength);
    mbedtls_md_finish(&ctx, shaResult);
    mbedtls_md_free(&ctx);
 
    String hashStr = "";
 
    for(uint16_t i = 0; i < size; i++) {
        String hex = String(shaResult[i], HEX);
        if(hex.length() < 2) {
            hex = "0" + hex;
        }
        hashStr += hex;
    }

    return hashStr;
}

int OvhAPI::time_delta()
{
  if (!_time_delta_read) {
    _time_delta_read = true;
    JSONVar server_response = GET("/auth/time", false);
    const char* server_time_str = server_response["message"];
    int server_time = String(server_time_str).toInt();
    _time_delta = server_time - _timeClient->getEpochTime();
    OVH_DEBUG_PRINT("OvhAPI::time_delta(): server_response: ");OVH_DEBUG_PRINTLN(server_response);
    OVH_DEBUG_PRINT("OvhAPI::time_delta(): server_time: ");OVH_DEBUG_PRINTLN(server_time);
    OVH_DEBUG_PRINT("OvhAPI::time_delta(): _timeClient->getEpochTime(): ");OVH_DEBUG_PRINTLN(_timeClient->getEpochTime());
    OVH_DEBUG_PRINT("OvhAPI::time_delta(): _time_delta");OVH_DEBUG_PRINTLN(_time_delta);
  }
  return _time_delta;
}

JSONVar OvhAPI::GET(const char* _target, bool _need_auth)
{
  OVH_DEBUG_PRINT("OvhAPI::GET(");OVH_DEBUG_PRINT(_target);OVH_DEBUG_PRINT(", ");OVH_DEBUG_PRINT(_need_auth);OVH_DEBUG_PRINTLN(")");
  
  // FIXME
  //if kwargs:
  //          kwargs = self._canonicalize_kwargs(kwargs)
  //          query_string = self._prepare_query_string(kwargs)
  //          if '?' in _target:
  //              _target = '%s&%s' % (_target, query_string)
  //          else:
  //              _target = '%s?%s' % (_target, query_string)
  return call("GET", _target, "", _need_auth);
}

JSONVar OvhAPI::GET(String& _target, bool _need_auth)
{
  return GET(_target.c_str(), _need_auth);
}

JSONVar OvhAPI::PUT(const char* _target, const char* payload, bool _need_auth)
{
  OVH_DEBUG_PRINT("OvhAPI::PUT(");OVH_DEBUG_PRINT(_target);
  OVH_DEBUG_PRINT(", ");OVH_DEBUG_PRINT(payload);
  OVH_DEBUG_PRINT(", ");OVH_DEBUG_PRINT(_need_auth);
  OVH_DEBUG_PRINTLN(")");
  
  return call("PUT", _target, payload, _need_auth);
}

JSONVar OvhAPI::PUT(String& _target, String& payload, bool _need_auth)
{
  return PUT(_target.c_str(), payload.c_str(), _need_auth);
}

JSONVar OvhAPI::POST(const char* _target, const char* payload, bool _need_auth)
{
  OVH_DEBUG_PRINT("OvhAPI::POST(");OVH_DEBUG_PRINT(_target);
  OVH_DEBUG_PRINT(", ");OVH_DEBUG_PRINT(payload);
  OVH_DEBUG_PRINT(", ");OVH_DEBUG_PRINT(_need_auth);
  OVH_DEBUG_PRINTLN(")");
  
  return call("POST", _target, payload, _need_auth);
}

JSONVar OvhAPI::POST(String& _target, String& payload, bool _need_auth)
{
  return POST(_target.c_str(), payload.c_str(), _need_auth);
}

JSONVar OvhAPI::DELETE(const char* _target, bool _need_auth)
{
  OVH_DEBUG_PRINT("OvhAPI::DELETE(");OVH_DEBUG_PRINT(_target);OVH_DEBUG_PRINT(", ");OVH_DEBUG_PRINT(_need_auth);OVH_DEBUG_PRINTLN(")");
  
  return call("DELETE", _target, "", _need_auth);
}

JSONVar OvhAPI::DELETE(String& _target, bool _need_auth)
{
  return DELETE(_target.c_str(), _need_auth);
}

JSONVar OvhAPI::call(const char* method, const char* path, const char* data, bool need_auth)
{
  OVH_DEBUG_PRINT("BEGIN OvhAPI::call(");OVH_DEBUG_PRINT(method);OVH_DEBUG_PRINT(", ");
  OVH_DEBUG_PRINT(path);OVH_DEBUG_PRINT(", ");
  OVH_DEBUG_PRINT(need_auth);OVH_DEBUG_PRINTLN(")");
  
  struct response rtn = raw_call(method, path, data, need_auth);
  int status = rtn.code;
  String message = rtn.data;
  JSONVar json_result;
  JSONVar json_parse = JSON.parse(rtn.data);
  json_result["json_result"] = json_parse;
  
  if (status < 0 || status != 200) {
    switch(status) {
      case HTTPC_ERROR_CONNECTION_REFUSED:
        message = "HTTPC_ERROR_CONNECTION_REFUSED";
        break;
      case HTTPC_ERROR_SEND_HEADER_FAILED:
        message = "HTTPC_ERROR_SEND_HEADER_FAILED";
        break;
      case HTTPC_ERROR_SEND_PAYLOAD_FAILED:
        message = "HTTPC_ERROR_SEND_PAYLOAD_FAILED";
        break;
      case HTTPC_ERROR_NOT_CONNECTED:
        message = "HTTPC_ERROR_NOT_CONNECTED";
        break;
      case HTTPC_ERROR_CONNECTION_LOST:
        message = "HTTPC_ERROR_CONNECTION_LOST";
        break;
      case HTTPC_ERROR_NO_STREAM:
        message = "HTTPC_ERROR_NO_STREAM";
        break;
      case HTTPC_ERROR_NO_HTTP_SERVER:
        message = "HTTPC_ERROR_NO_HTTP_SERVER";
        break;
      case HTTPC_ERROR_TOO_LESS_RAM:
        message = "HTTPC_ERROR_TOO_LESS_RAM";
        break;
      case HTTPC_ERROR_ENCODING:
        message = "HTTPC_ERROR_ENCODING";
        break;
      case HTTPC_ERROR_STREAM_WRITE:
        message = "HTTPC_ERROR_STREAM_WRITE";
        break;
      case HTTPC_ERROR_READ_TIMEOUT:
        message = "HTTPC_ERROR_READ_TIMEOUT";
        break;
      default:
        message = rtn.data;
        break;
    }

  }

  json_result["status"] = status;
  json_result["message"] = message;

  OVH_DEBUG_PRINT("    INFO OvhAPI::call rtn.data=");OVH_DEBUG_PRINTLN(rtn.data);
  OVH_DEBUG_PRINT("    END OvhAPI::call json_result=");OVH_DEBUG_PRINTLN(json_result);
  return json_result;
}

struct response OvhAPI::raw_call(const char* method, const char* path, const char* data, bool need_auth)
{
  // OVH_DEBUG_PRINT("BEGIN OvhAPI::raw_call(");OVH_DEBUG_PRINT(method);OVH_DEBUG_PRINT(", ");
  // OVH_DEBUG_PRINT(path);OVH_DEBUG_PRINT(", ");
  // OVH_DEBUG_PRINT(data);OVH_DEBUG_PRINT(", ");
  // OVH_DEBUG_PRINT(need_auth);OVH_DEBUG_PRINTLN(")");
  
  struct response rtn = {HTTPC_ERROR_NOT_CONNECTED, "not connected"}; 
  String body = "";
  String target = String(_endpoint) + String(path);
  // OVH_DEBUG_PRINT("    INFO OvhAPI::raw_call: target=");OVH_DEBUG_PRINTLN(target);

  #ifdef OVH_PROFILE
    unsigned long pstart, pend, pdur;
    pstart=micros();
  #endif  

  String rest_method = String(method);
  String(method).toUpperCase();

  #ifdef OVH_PROFILE
    pend=micros(); pdur=pend-pstart;
    Serial.print("Timing 245: "); Serial.println(pdur);
    pstart=micros();
  #endif

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    _http->begin(target.c_str());
    _http->addHeader("X-Ovh-Application", _application_key);
    // OVH_DEBUG_PRINT("    INFO OvhAPI::raw_call: X-Ovh-Application = ");OVH_DEBUG_PRINTLN(_application_key);

    #ifdef OVH_PROFILE
      pend=micros(); pdur=pend-pstart;
      Serial.print("Timing 257: "); Serial.println(pdur);
      pstart=micros();
    #endif

    if (strlen(data) > 0) {
      _http->addHeader("Content-type", "application/json");
      // OVH_DEBUG_PRINTLN("    INFO OvhAPI::raw_call: Content-Type = application/json");
      body = String(JSONVar::stringify(JSON.parse(data)).c_str());
    }

    #ifdef OVH_PROFILE
      pend=micros(); pdur=pend-pstart;
      Serial.print("Timing 268: "); Serial.println(pdur);
      pstart=micros();
    #endif

    if (need_auth) {
      String now = String(time_delta() + _timeClient->getEpochTime());
      // OVH_DEBUG_PRINT("    INFO OvhAPI::raw_call: now=");OVH_DEBUG_PRINTLN(now);

      #ifdef OVH_PROFILE
        pend=micros(); pdur=pend-pstart;
        Serial.print("Timing 279: "); Serial.println(pdur);
        pstart=micros();
      #endif

      String signature = String(_application_secret) + "+" + String(_consumer_key) + "+" + rest_method + "+" + target + "+" + body + "+" + now;
      // OVH_DEBUG_PRINT("    INFO OvhAPI::raw_call: signature = ");OVH_DEBUG_PRINTLN(signature);

      #ifdef OVH_PROFILE
        pend=micros(); pdur=pend-pstart;
        Serial.print("Timing 288: "); Serial.println(pdur);
        pstart=micros();
      #endif

      signature = "$1$" + sha1(signature);
      // OVH_DEBUG_PRINT("    INFO OvhAPI::raw_call: $1$ + sha1(signature) = ");OVH_DEBUG_PRINTLN(signature);

      _http->addHeader("X-Ovh-Consumer", _consumer_key);
      _http->addHeader("X-Ovh-Timestamp", now);
      _http->addHeader("X-Ovh-Signature", signature);

      #ifdef OVH_PROFILE
        pend=micros(); pdur=pend-pstart;
        Serial.print("Timing 301: "); Serial.println(pdur);
        pstart=micros();
      #endif

      // OVH_DEBUG_PRINT("    INFO OvhAPI::raw_call: X-Ovh-Consumer = ");OVH_DEBUG_PRINTLN(_consumer_key);
      // OVH_DEBUG_PRINT("    INFO OvhAPI::raw_call: X-Ovh-Timestamp = ");OVH_DEBUG_PRINTLN(now);
      // OVH_DEBUG_PRINT("    INFO OvhAPI::raw_call: X-Ovh-Signature = ");OVH_DEBUG_PRINTLN(signature);
    }

    if(rest_method == "GET") {
      rtn.code = _http->GET();
    } else if(rest_method == "PUT") {
      rtn.code = _http->PUT(body);
    } else if(rest_method == "POST") {
      rtn.code = _http->POST(body);
    } else if(rest_method == "DELETE") {
      rtn.code = _http->sendRequest("DELETE");
    } else {
      OVH_DEBUG_PRINTLN("    END OvhAPI::raw_call: method not allowed");
      rtn.code = HTTP_CODE_METHOD_NOT_ALLOWED;
      return {HTTP_CODE_METHOD_NOT_ALLOWED, "method not allowed"};
    }

    #ifdef OVH_PROFILE
      pend=micros(); pdur=pend-pstart;
      Serial.print("Timing 326: "); Serial.println(pdur);
      pstart=micros();
    #endif

    // OVH_DEBUG_PRINT("    INFO OvhAPI::raw_call: rtn.code=");OVH_DEBUG_PRINTLN(rtn.code);
      
    rtn.data = _http->getString();

  #ifdef OVH_PROFILE
    pend=micros(); pdur=pend-pstart;
    Serial.print("Timing 336: "); Serial.println(pdur);
    pstart=micros();
  #endif

  } else {
    OVH_DEBUG_PRINTLN("    END OvhAPI::raw_call: WiFi not connected");
    return {HTTPC_ERROR_NOT_CONNECTED, "not connected"};
  }

  #ifdef OVH_PROFILE
    pend=micros(); pdur=pend-pstart;
    Serial.print("Timing 347: "); Serial.println(pdur);
    pstart=micros();
  #endif

  OVH_DEBUG_PRINT("    END OvhAPI::raw_call rtn.code=");OVH_DEBUG_PRINTLN(rtn.code);
  OVH_DEBUG_PRINT("    END OvhAPI::raw_call rtn.data=");OVH_DEBUG_PRINTLN(rtn.data);
  return rtn;
}
