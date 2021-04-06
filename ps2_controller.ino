#include <BleGamepad.h>
#include "driver/rtc_io.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//defines for buttons, remap to suit your project
#define RX 36 //thumbstick input
#define RY 39
#define LX 34
#define LY 15

#define R1 32 //shoulder buttons and thumbstick press
#define R2 25
#define R3 23
#define L1 33
#define L2 26
#define L3 22

#define DU 27 //DPAD
#define DD 14
#define DL 12
#define DR 13

#define BX 21 //main buttons
#define BO 19
#define BS 17
#define BT 16

#define START 18
#define SELECT 5
#define ANALOG 4
#define LED 2
#define LEDGPIO GPIO_NUM_2

//defines for other stuff
#define SHUTDOWNPRESSLENGTH 150 //the length the analog button has to be pressed for to shutdown the controller
#define OTADOWNPRESSLENGTH 1000 //the length the analog button has to be pressed for to get into OTA mode
#define ADCSAMPLES 10 //the number of adc samples taken to get the analog value for the thumbsticks
#define NBUTTONS 17 //number of defined buttons in button list

//credentials for OTA wifi mode
#define WIFI_SSID "YOUR AP NAME" //not needed if you do OTA over softAP
#define WIFI_PWD "YOUR AP PW"

//list of buttons to check
int buttons[] = {R1, R2, R3, L1, L2, L3, BX, BO, BS, BT, START, SELECT, ANALOG, DU, DR, DD, DL};
bool rawButtonState[NBUTTONS];
bool rawLastButtonState[NBUTTONS];
bool buttonState[NBUTTONS]; //this array reflects the state of the button after filtering

int analogPressedFor;
int waitCycle;

BleGamepad gamepad = BleGamepad("Supa Dupa Gamepad", "arduino101", 100); //modify this to suit your needs (first parameter is then name of the device, second the manufacturer, third the battery charge state in percent)


//function for OTA update
//pressing X restarts the controller
//pressing O connects to the WFI defined by the credentials
//pressing |_| creates a softAP to wich you can connect to update the firmware
//pressing X while in either of the OTA modes will restart the controller to exit the OTA mode
void doOta() {
  Serial.println("doing OTA");
  gamepad.end();
  Serial.println("waiting for selection");
  while (42) {
    ledcWrite(0, 8191);
    delay(800);
    ledcWrite(0, 0);
    delay(200);
    if (digitalRead(BO) == HIGH) {
      Serial.println("doing OTA over existing wifi");
      Serial.println("connecting to wifi");
      WiFi.mode(WIFI_STA);
      WiFi.begin(WIFI_SSID, WIFI_PWD);
      int loopItt = 0;
      while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("not connected...");
        ledcWrite(0, 4000);
        delay(250);
        ledcWrite(0, 0);
        delay(250);
        loopItt++;
        if (loopItt > 15) {
          Serial.println("Timeout!");
          while (digitalRead(ANALOG) == LOW) {
            ledcWrite(0, 8191);
            delay(500);
            ledcWrite(0, 4000);
            delay(500);
          }
          Serial.println("restarting");
          ESP.restart();
        }
      }
      Serial.println("connected!");
      ledcWrite(0, 4000);
    
      ArduinoOTA.setHostname("SupaDupaController[OTA]"); //modify this to suit your application
      ArduinoOTA.onStart([]() {
        ledcWrite(0, 2000);
        delay(250);
        ledcWrite(0, 8191);
        delay(250);
        Serial.println("start OTA update");
      });
      ArduinoOTA.onEnd([]() {
        Serial.println("OTA update finished");
        for (int i = 0; i < 5; i++) {
          ledcWrite(0, 8191);
          delay(100);
          ledcWrite(0, 0);
          delay(100);
        }
        Serial.println("restarting");
        ESP.restart();
      });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        ledcWrite(0, (int) (((float) progress / total) * 8191));
        Serial.print("Progress:");
        Serial.print((int) (((float) progress / total) * 100), DEC);
        Serial.println("%");
      });
      ArduinoOTA.onError([](ota_error_t error) {
        Serial.print("Error:");
        Serial.println(error);
        switch (error) {
          case OTA_AUTH_ERROR:
            Serial.println("Auth Failed!");
            break;
          case OTA_BEGIN_ERROR:
            Serial.println("Begin Failed!");
            break;
          case OTA_CONNECT_ERROR:
            Serial.println("Connect Failed!");
            break;
          case OTA_RECEIVE_ERROR:
            Serial.println("Receive Failed!");
            break;
          case OTA_END_ERROR:
            Serial.println("End Failed!");
            break;
          default:
            Serial.println("Unknown Error!");
            break;
        }
        while (digitalRead(ANALOG) == LOW) {
          ledcWrite(0, 4000);
          delay(250);
          ledcWrite(0, 2000);
          delay(250);
        }
        Serial.println("restarting");
        ESP.restart();
      });
      ArduinoOTA.begin();
      Serial.println("OTA Ready");
      Serial.print("IP address:");
      Serial.println(WiFi.localIP());
      
      while (42) {
        if (digitalRead(BX) == HIGH) ESP.restart();
        ArduinoOTA.handle();
      }
    }
    if (digitalRead(BS) == HIGH) {
      Serial.println("doing OTA over own wifi");

      WiFi.softAP("SupaDupaController[WIFI]", ""); //modify this to suit your application (first parameter name of the AP, second the password of the AP (leave blank for no password))
      Serial.println("softAP startet");
      Serial.print("IP:");
      Serial.println(WiFi.softAPIP());
      
      ArduinoOTA.setHostname("SupaDupaController[OTA]"); //modify this to suit your application
      ArduinoOTA.onStart([]() {
        ledcWrite(0, 2000);
        delay(250);
        ledcWrite(0, 8191);
        delay(250);
        Serial.println("start OTA update");
      });
      ArduinoOTA.onEnd([]() {
        Serial.println("OTA update finished");
        for (int i = 0; i < 5; i++) {
          ledcWrite(0, 8191);
          delay(100);
          ledcWrite(0, 0);
          delay(100);
        }
        Serial.println("restarting");
        ESP.restart();
      });
      ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        ledcWrite(0, (int) (((float) progress / total) * 8191));
        Serial.print("Progress:");
        Serial.print((int) (((float) progress / total) * 100), DEC);
        Serial.println("%");
      });
      ArduinoOTA.onError([](ota_error_t error) {
        Serial.print("Error:");
        Serial.println(error);
        switch (error) {
          case OTA_AUTH_ERROR:
            Serial.println("Auth Failed!");
            break;
          case OTA_BEGIN_ERROR:
            Serial.println("Begin Failed!");
            break;
          case OTA_CONNECT_ERROR:
            Serial.println("Connect Failed!");
            break;
          case OTA_RECEIVE_ERROR:
            Serial.println("Receive Failed!");
            break;
          case OTA_END_ERROR:
            Serial.println("End Failed!");
            break;
          default:
            Serial.println("Unknown Error!");
            break;
        }
        while (digitalRead(ANALOG) == LOW) {
          ledcWrite(0, 4000);
          delay(250);
          ledcWrite(0, 2000);
          delay(250);
        }
        Serial.println("restarting");
        ESP.restart();
      });
      ArduinoOTA.begin();
      Serial.println("OTA Ready");
      Serial.print("IP address:");
      Serial.println(WiFi.localIP());
      
      while (42) {
        if (digitalRead(BX) == HIGH) ESP.restart();
        ArduinoOTA.handle();
      }
    }
    if (digitalRead(BX) == HIGH) {
      Serial.println("Exiting OTA");
      ESP.restart();
    }
  }
}

void setup() {
  //debrick
  //this is the first code that should run in case you upload a firmware that crashes the device before you can enter the OTA mode
  //pressing down X and /\ should start the OTA mode on bootup. This includes crash cycles, deep sleep and initial power up
  pinMode(BX, INPUT_PULLDOWN);
  pinMode(BT, INPUT_PULLDOWN);
  if (digitalRead(BX) == HIGH && digitalRead(BT) == HIGH) {
    doOta();
  }

  //set pinMode of buttons
  //seting the pulldown manually might be redundant
  for (int i = 0; i < NBUTTONS; i++) {
    pinMode(buttons[i], INPUT_PULLDOWN);
    rtc_gpio_pullup_dis((gpio_num_t) buttons[i]);
    rtc_gpio_pulldown_en((gpio_num_t) buttons[i]); //set pin to pulldown, works even in deep sleep (important)
  }
  pinMode(RX, INPUT);
  pinMode(RY, INPUT);
  pinMode(LX, INPUT);
  pinMode(LY, INPUT);

  //seting up the LED pin for PWM
  ledcSetup(0, 5000, 13); //channel 0, basefreq 5000, 13 bit depth
  ledcAttachPin(LED, 0);
  ledcWrite(0, 0);

  Serial.begin(115200);

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1) Serial.println("Back from ze dead!");
  
  Serial.println("Beginning BLE worker");
  gamepad.begin(false); //no auto report

  //propably redundant but just to be shure
  for (int i = 0; i < NBUTTONS; i++) {
    rawButtonState[i] = false;
  }
  analogPressedFor = 0;
}

void loop() {
  if (gamepad.isConnected()) { //when the controller is connected to a device this will get executed
    ledcWrite(0, 8191);

    //testing the buttons
    for (int i = 0; i < NBUTTONS; i++) {
      rawLastButtonState[i] = rawButtonState[i];
      rawButtonState[i] = digitalRead(buttons[i]);
      buttonState[i] = rawLastButtonState[i] & rawButtonState[i];
      if (i < NBUTTONS) {
        if (buttonState[i] == true) {
          gamepad.press(1LL << i);
        } else {
          gamepad.release(1LL << i);
        }
      }
      if (i == NBUTTONS - 5) { //analog button
        if (buttonState[i] == true) {
          analogPressedFor++;
          if (analogPressedFor >= OTADOWNPRESSLENGTH) {
            doOta();
          }
        } else {
          if (analogPressedFor > SHUTDOWNPRESSLENGTH) {
            //do deep sleep
            esp_sleep_enable_ext1_wakeup(1LL << ANALOG, ESP_EXT1_WAKEUP_ANY_HIGH);
            Serial.println("going to sleep");
            delay(500);
            ledcWrite(0, 0);
            esp_deep_sleep_start();
          }
          analogPressedFor = 0;
        }
      }
    }

    //testing the analog sticks
    int sample = 0;
    for (int i = 0; i < ADCSAMPLES; i++) {
      sample += analogRead(RX);
      delayMicroseconds(1);
    }
    sample = sample / ADCSAMPLES;
    sample = map(sample, 0, 4096, -127, 128);
    gamepad.setZ(sample*255);

    sample = 0;
    for (int i = 0; i < ADCSAMPLES; i++) {
      sample += analogRead(RY);
      delayMicroseconds(1);
    }
    sample = sample / ADCSAMPLES;
    sample = map(sample, 0, 4096, -127, 128);
    gamepad.setRZ(sample*255);

    sample = 0;
    for (int i = 0; i < ADCSAMPLES; i++) {
      sample += analogRead(LX);
      delayMicroseconds(1);
    }
    sample = sample / ADCSAMPLES;
    sample = map(sample, 0, 4096, -127, 128);
    gamepad.setX(sample*255);
    sample = 0;
    for (int i = 0; i < ADCSAMPLES; i++) {
      sample += analogRead(LY);
      delayMicroseconds(1);
    }
    sample = sample / ADCSAMPLES;
    sample = map(sample, 0, 4096, -127, 128);
    gamepad.setY(sample*255);

    //sending report of the button and analog stick state
    gamepad.sendReport();

    delay(10);
  } else { //this gets executet when the controller is not connected
    if (digitalRead(ANALOG) == HIGH) {
      analogPressedFor++;
      if (analogPressedFor >= OTADOWNPRESSLENGTH) {
        doOta();
      }
    } else {
      if (analogPressedFor > SHUTDOWNPRESSLENGTH) {
        //do deep sleep
        esp_sleep_enable_ext1_wakeup(1LL << ANALOG, ESP_EXT1_WAKEUP_ANY_HIGH);
        Serial.println("going to sleep");
        delay(500);
        ledcWrite(0, 0);
        esp_deep_sleep_start();
      }
      analogPressedFor = 0;
    }
    if (waitCycle >= 50) {
      ledcWrite(0, 8191);
    } else {
      ledcWrite(0, 0);
    }
    delay(10);
    waitCycle++;
    if (waitCycle > 100) waitCycle = 0;
  }
}
