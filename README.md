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

## License

This code is licensed under GPL3, so its opensource. You can do anything with it, you can modify and redistribute it but i also dont give any warranty, so dont blame me if your ESP32 catches fire.

