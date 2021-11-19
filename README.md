OvhAPI
======

Arduino/ESP32 library to simplify working OVH API.

* Author: Christophe Bouvier (https://www.bcisoft.fr)
* Copyright (C) 2021 BCiSoft.
* Released under the MIT license.

Description
-----------
This library allows you to use callback functions to track single, double, triple and long clicks. It takes care of debouncing. Using this lib will reduce and simplify your source code significantly. 

It has been tested with Arduino ESP32 devices.

How To Use
----------

This library helps you to call OVH API [https://api.ovh.com/](https://api.ovh.com/).

__Prerequisites__

You need to install these libraries:
- mbedtls
- NTPClient
- HTTPClient
- Arduino_JSON


__API Keys__

Any application that wants to communicate with the OVHcloud API must be declared in advance.

To do this, click the following link: https://eu.api.ovh.com/createToken/.

Fill in your OVHcloud customer ID, password, and application name. The name will be useful later if you want to allow others to use it.

You can also add a description of the application and a validity period.

The Rights field allows you to restrict the use of the application to certain APIs.
In order to allow all OVHcloud APIs for an HTTP method, put an asterisk (*) into the field, as in the following example where the GET method is allowed for all APIs:

After you click Create keys, you will be issued three keys:
- the application key, called AK
- your secret application key, not to be disclosed, called AS
- a secret "consumer key", not to be disclosed, called CK

See [https://docs.ovh.com/gb/en/api/first-steps-with-ovh-api/](https://docs.ovh.com/gb/en/api/first-steps-with-ovh-api/) for more details.



__Time__

For the library to work you need to have a working UTC NTPClient:

```
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

void setup() {
  // Initialize a NTPClient to get UTC time (offset=0)
  timeClient.begin(); 
  timeClient.setTimeOffset(0);
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
}
```

Then give reference to the timeClient object during OvhAPI instantiation:
```
OvhAPI ovh(&timeClient, ENDPOINT_OVH_EU, OVH_AK, OVH_AS, OVH_CK);
```



Examples
-----
- Welcome – the most basic example, shows your name
- Billing – shows how to get billing detail
- KVM – how to grant KVM access on a bare metal server



Installation
------------
Open the Arduino IDE choose "Sketch > Include Library" and search for "OvhAPI". 
Or download the ZIP archive (https://github.com/BCISOFT/OvhAPI/archive/refs/heads/main.zip), and choose "Sketch > Include Library > Add .ZIP Library..." and select the downloaded file.



License
-------

MIT License

Copyright (c) 2021 BCISOFT

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
