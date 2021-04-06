# ESP32 BLE PS2 Controller

## Overview

The title says it all. This code will make your ESP32 act as a bluetooth controller. The code is written for a PS2 controller but can be adapted easely. It also features OTA over an existing AP or by creating its own AP.

If you have a simmilar project you can use this code to start

## What you need

You need an ESP32, an old controller big enough to host the ESP together with a battery and charge controller, the [arduinoIDE](https://arduino.cc/en/software) together with the needed libraries and tools for the ESP32 [(getting startet with the ESP32)](https://dronebotworkshop.com/esp32-intro/#Getting_started_with_the_Arduino_IDE) and last but not least the [ESP32 BLE Gamepad](https://github.com/lemmingDev/ESP32-BLE-Gamepad) library from [lemmingDev](https://github.com/lemmingDev).

## Compiling

If you have all of this you can just open the arduinoIDE, open the project folder and you are good. Connect your ESP32 via USB, click upload and if everything goes right its done.

### It wont go right

You will most definitely have some problems compiling it. Turns out OTA on the ESP32 is "quite" heavy on the memory so you have to set the partition scheme to "minimal SPIFFS". This can be done under the tools section in the arduinoIDE but it turns out not all definied boards have this option exposed so you might have to go with the "ESP32 Dev module". While you are here you can set the clock of the ESP32 to 80Mhz as this saves some power. 80Mhz is the lowest it can go because we need bluetooth and WiFi.

Now it should compile

## Adjusting the code

If you also want to modify a PS2 controller then you dont have to change anything in the code besides the credentials of the WIFI and some minor stuff like the host- and devicename. If you want to change the button configuration you have to define the pin on wich the button is, add it to the buttons array and update the NBUTTONS define to the new length of the array.

## Normal behavior

The standart code has some complex behavior wich is documented here.

### LED indicator

When the controller isn't connected to a device the LED will blink (midlong on, midlong off).
When the controller is connected the LED will be continuesly.
When in OTA update mode the LED will reflect the progress of the upload (off 0%, fully on 100%), after the upload is finished the LED should blink 5 times fast. When an error accourse the LED will blink fast from nearly off to medium on.
When connecting to a WiFi the LED will blink fast from medium on to off long as long as it isnt connected. When the connection times out (due to a wrong password for example) the LED will blink slow from medium on to fully on.


### Power on/off

The code has software power controll. Press the button on the ANALOG pin for SHUTDOWNPRESSLENGTH tens of milliseconds (150 -> 1500 ms) and the ESP32 will go to deep sleep. To wake it up press ANALOG again.

### OTA

If you have not exposed the USB data lines of the ESP32 to the outside of the controller you have to open up the controller every time just to update the firmware. This sucks so i added OTA (update over the air) so you never have to open up the controller again.
To enter OTA press the ANALOG button for OTADOWNPRESSLENGTH tens of milliseconds (1000 -> 10000 ms -> 10 s). The LED should blink (long on, short off). Now you can press X to exit OTA mode, press O to do OTA over an existing WiFi wich can be defined in the code (`WIFI_SSID` and `WIFI_PWD`). Press |_| (Square) to do OTA over softAP, the ESP32 will then create its own WiFi network to wich you can connect to update the controller.
While being in either of the two OTA modes you can press X to exit the OTA mode.
If an error accourse either while connecting to the WiFi or while updating pressing the ANALOG button will restart the device.

### Brick prevention

If you have uploaded some code that bricks the device you can press /\ (triangle) on bootup (this includes after crashing and deepsleep) to enter the OTA mode directly. From there you can do OTA normaly as statet in the OTA section above. For this to work you should not include any code above the brick prevention section in the setup function.

## License

This code is licensed under GPL3, so its opensource. You can do anything with it, you can modify and redistribute it but i also dont give any warranty, so dont blame me if your ESP32 catches fire.

