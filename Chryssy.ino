#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <VariPass.h>
#include <TwiFi.h>

#include "Graphics.h"
#include "ConfigurationWiFi.h"
#include "ConfigurationBattery.h"
#include "ConfigurationVariPass.h"

#define PIN_OLED_RST     16
#define PIN_INTR_GEIGER  13
#define PIN_INTR_TRIGGER 14
#define PIN_INTR_BUTTON  12
#define PIN_BATTERY      A0

#define EEPROM_CONNECTED 2
#define EEPROM_HIDDEN    3

#define OLED_WIDTH  128
#define OLED_HEIGHT  64

#define SIEVERT_MULTI 0.0057

#define DELAY_INTERVAL 100
#define DELAY_WIFI     3000
#define DELAY_RESET    3000

int delaySecond;
int delayMinute;
int delayButton;
int delayTrigger;

bool   hitGeiger;
bool   hitTrigger;
bool   hitButton;

bool   uiHidden;
int    uiWiFiConnected;
String uiStringWiFi;
String uiStringCountdown;

bool   canRun;
bool   wifiConnecting;
bool   wasWiFiToggled;
bool   wasHoldToggled;
bool   buttonHeld;
bool   triggerHeld;
bool   isHoldActive;

int   geigerArray[60];
int   geigerCounts;
int   geigerHold;
int   geigerPrevCounts;
float geigerPrevSiev;

int valueBattery[10];
float batt;

Adafruit_SSD1306 display = Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT, &Wire, PIN_OLED_RST);

void setupGlobals();
void setupPins();
void setupBattery();
void setupDisplay();
void setupWiFi();

void startWiFi();
void stopWiFi();

void processDelays();
void processMinute();
void processBattery();
void processGeiger();
void processTrigger();
void processButton();

int textCenterX(String text, int multi);
int textLeftX(String text, int x, int multi);

void ICACHE_RAM_ATTR intrGeiger();
void ICACHE_RAM_ATTR intrTrigger();
void ICACHE_RAM_ATTR intrButton();

void connectAttempt(int idEntry, int attempt);
void connectSuccess(int idEntry);
void connectFail(int idEntry);

void setupGlobals() {
    delaySecond = 0;
    delayMinute = 60;
    delayButton = 0;

    hitGeiger  = false;
    hitTrigger = false;
    hitButton  = false;

    wifiConnecting = false;
    wasWiFiToggled = false;
    buttonHeld     = false;
    triggerHeld     = false;

    uiStringWiFi      = "x";
    uiStringCountdown = "";

    geigerCounts = 0;
    geigerPrevCounts = 0;
    geigerPrevSiev = 0;
}

void setupPins() {
    pinMode(PIN_INTR_GEIGER, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_INTR_GEIGER), intrGeiger, RISING);    
    pinMode(PIN_INTR_TRIGGER, INPUT_PULLUP); 
    attachInterrupt(digitalPinToInterrupt(PIN_INTR_TRIGGER), intrTrigger, CHANGE);   
    pinMode(PIN_INTR_BUTTON, INPUT_PULLUP); 
    attachInterrupt(digitalPinToInterrupt(PIN_INTR_BUTTON), intrButton, CHANGE);
}

void setupBattery() {
    int battery = analogRead(PIN_BATTERY);
    for (int i = 0; i < 10; i++)
        valueBattery[i] = battery;
}

void setupDisplay() {    
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed."));
        for(;;);
    }
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE);
    EEPROM.begin(512);
    uiHidden = (bool) EEPROM.read(EEPROM_HIDDEN);
    EEPROM.end();
}

void setupWiFi() {
    twifiInit(
        wifis,
        WIFI_COUNT,
        WIFI_HOST,
        WIFI_TIMEOUT,
        &connectAttempt,
        &connectSuccess,
        &connectFail,
        WIFI_DEBUG
        );
    WiFi.disconnect();
    EEPROM.begin(512);
    bool wasConnected = (bool) EEPROM.read(EEPROM_CONNECTED);
    EEPROM.end();
    if (wasConnected)
        startWiFi();
}

void startWiFi() {
    wifiConnecting = true;
    bool wifiResult = twifiConnect(false);
    if (wifiResult) {
        EEPROM.begin(512);
        EEPROM.write(EEPROM_CONNECTED, 1);  
        EEPROM.end();
    }
    drawUI();
    wifiConnecting = false;
}

void stopWiFi() {
    WiFi.disconnect();
    EEPROM.begin(512);
    EEPROM.write(EEPROM_CONNECTED, 0);  
    EEPROM.end();
}

void drawUI() {
    display.clearDisplay();

    if (buttonHeld || wifiConnecting || !uiHidden) {
        display.setTextSize(1);
        if (twifiIsConnected()) {      
            int32_t rssi = WiFi.RSSI();
            if (rssi >= -70) {
                display.drawBitmap(2, 3, iconWiFi3, ICON_WIFI_3_WIDTH, ICON_WIFI_3_HEIGHT, 1);
            } 
            else if (rssi < -70 && rssi >= -80) {
                display.drawBitmap(2, 3, iconWiFi2, ICON_WIFI_2_WIDTH, ICON_WIFI_2_HEIGHT, 1);
            }
            else if (rssi < -80) {
                display.drawBitmap(2, 3, iconWiFi1, ICON_WIFI_1_WIDTH, ICON_WIFI_1_HEIGHT, 1);
            }
        }
        else {
            display.drawBitmap(2, 3, iconWiFi0, ICON_WIFI_0_WIDTH, ICON_WIFI_0_HEIGHT, 1);
        }
    
        display.setCursor(17, 4);
        if (wifiConnecting || twifiIsConnected())
            display.print(uiStringWiFi);
        else
            display.print("x");
    }

    if (!uiHidden) {
        display.drawLine(0, 15, 127, 15, WHITE); 
    
        display.setCursor(textCenterX(uiStringCountdown, 1), 4);
        display.print(uiStringCountdown);   

        if (BATTERY_VOLTAGE) {
            String battString = String(batt, 2) + "v";
            display.setCursor(textLeftX(battString, 0, 1), 4);
            display.print(battString);
        }
        else {                
            if (batt >= 0 && batt < 10)
                display.drawBitmap(112, 3, iconBatt00, ICON_BATT_00_WIDTH, ICON_BATT_00_HEIGHT, 1);
            else if (batt >= 10 && batt < 20)
                display.drawBitmap(112, 3, iconBatt10, ICON_BATT_10_WIDTH, ICON_BATT_10_HEIGHT, 1);
            else if (batt >= 20 && batt < 30)
                display.drawBitmap(112, 3, iconBatt20, ICON_BATT_20_WIDTH, ICON_BATT_20_HEIGHT, 1);
            else if (batt >= 30 && batt < 40)
                display.drawBitmap(112, 3, iconBatt30, ICON_BATT_30_WIDTH, ICON_BATT_30_HEIGHT, 1);
            else if (batt >= 40 && batt < 50)
                display.drawBitmap(112, 3, iconBatt40, ICON_BATT_40_WIDTH, ICON_BATT_40_HEIGHT, 1);
            else if (batt >= 50 && batt < 60)
                display.drawBitmap(112, 3, iconBatt50, ICON_BATT_50_WIDTH, ICON_BATT_50_HEIGHT, 1);
            else if (batt >= 60 && batt < 70)
                display.drawBitmap(112, 3, iconBatt60, ICON_BATT_60_WIDTH, ICON_BATT_60_HEIGHT, 1);
            else if (batt >= 70 && batt < 80)
                display.drawBitmap(112, 3, iconBatt70, ICON_BATT_70_WIDTH, ICON_BATT_70_HEIGHT, 1);
            else if (batt >= 80 && batt < 90)
                display.drawBitmap(112, 3, iconBatt80, ICON_BATT_80_WIDTH, ICON_BATT_80_HEIGHT, 1);
            else if (batt >= 90 && batt < 100)
                display.drawBitmap(112, 3, iconBatt90, ICON_BATT_90_WIDTH, ICON_BATT_90_HEIGHT, 1);
            else if (batt >= 100)
                display.drawBitmap(112, 3, iconBatt100, ICON_BATT_100_WIDTH, ICON_BATT_100_HEIGHT, 1);  
                   
            String battString = String(batt, 0) + "%";
            display.setCursor(textLeftX(battString, 16, 1), 4);
            display.print(battString);
        } 
    
        String text = "Prev:";
        display.setCursor(2, 46);
        display.print(text);
    
        String prevA = String(geigerPrevCounts);
        String prevB = String(geigerPrevSiev, 2);
        if (prevA.length() >= prevB.length())
            display.setCursor(38, 46);
        else
            display.setCursor(38 + (prevB.length()-prevA.length())*6, 46);
        display.print(String(geigerPrevCounts) + " CPM");
        
        if (prevB.length() >= prevA.length())
            display.setCursor(38, 55);
        else
            display.setCursor(38 + (prevA.length()-prevB.length())*6, 55);
        display.print(String(geigerPrevSiev, 2) + " uSv/h");
        
        if (wifiConnecting) {
            if (uiStringWiFi != "") {
                String text = "Connecting";
                display.setTextSize(2);
                display.setCursor(textCenterX(text, 2), 23);
                display.print(text);
            }
        }
        else {        
            text = "CPM";
            display.setTextSize(2);
            display.setCursor(textLeftX(text, 2, 2), 27);
            display.print(text);
            
            int totalCounts = 0;
            for (int i = 0; i < 60; i++)
                totalCounts += geigerArray[i];
            totalCounts += geigerCounts;
            if (isHoldActive) {
                text = "HOLD";
                display.setTextSize(1);
                display.setCursor(2, 55);
                display.print(text);    
                        
                if (geigerHold < totalCounts)
                    geigerHold = totalCounts;
                text = String(geigerHold);                                    
            }
            else {
                text = String(totalCounts);
            }
            display.setTextSize(3);
            display.setCursor(textLeftX(text, 40, 3), 20);
            display.print(text);
        }
    }
    
    display.display();
}

void processDelays() {
    if (delaySecond <= 0) {            
        delaySecond = 1000;
        if (delayMinute <= 0) {
            processMinute();
            delayMinute = 60;
        }
        else {
            delayMinute--;
        }    
        uiStringCountdown = String(delayMinute);
        for (int i = 1; i < 60; i++)
            geigerArray[i - 1] = geigerArray[i];
        geigerArray[59] = geigerCounts;
        geigerCounts = 0;
        drawUI();
    }
    else {
        delaySecond -= DELAY_INTERVAL;
    }
}

void processMinute() {
    canRun = false;
    
    int totalCounts = 0;
    for (int i = 0; i < 60; i++)
        totalCounts += geigerArray[i];
    totalCounts += geigerCounts; 
    
    if (isHoldActive) {
        if (geigerHold < totalCounts)
            geigerHold = totalCounts;
        geigerPrevCounts = geigerHold;
    }
    else {
        geigerPrevCounts = totalCounts;
    }
    
    geigerPrevSiev = ((float) geigerPrevCounts * SIEVERT_MULTI);

    if (!twifiIsConnected()) {
        EEPROM.begin(512);
        bool wasConnected = (bool) EEPROM.read(EEPROM_CONNECTED);
        EEPROM.end();
        if (wasConnected) {
            WiFi.disconnect();
            startWiFi();
        }
    }    
    if (twifiIsConnected()) {
        int result;
        varipassWriteInt(VARIPASS_KEY, VARIPASS_ID_COUNTS, geigerPrevCounts, &result);
        varipassWriteFloat(VARIPASS_KEY, VARIPASS_ID_SIEVERTS, geigerPrevSiev, &result, 4);
        varipassWriteFloat(VARIPASS_KEY, VARIPASS_ID_BATTERY, batt, &result, 2);
    }
    canRun = true;
}

void processBattery() {
    for (int i = 1; i < 10; i++)
        valueBattery[i-1] = valueBattery[i];
    valueBattery[9] = analogRead(PIN_BATTERY);

    float avgBattery = 0;
    for (int i = 0; i < 10; i++)
        avgBattery += valueBattery[i];
    avgBattery /= 10;
    
    if (BATTERY_VOLTAGE) {
        batt = (avgBattery / 1024) * 5;
    }
    else {
        batt = (((avgBattery / 1024) * 5 - BATTERY_V_MIN) / (BATTERY_V_MAX - BATTERY_V_MIN)) * 100; 
        if (batt < 0)
            batt = 0;
        else if (batt > 100)
            batt = 100;
    } 
}

void processGeiger() {
    if (hitGeiger) {
        hitGeiger = false;
        drawUI();   
    }
}

void processTrigger() {
    int trigger = digitalRead(PIN_INTR_TRIGGER);
    if (trigger == HIGH)
        triggerHeld = true;
    else
        triggerHeld = false;  
        
    if (hitTrigger) {
        if (trigger == LOW) {
            if (wasHoldToggled) {
                wasHoldToggled = false;
            }
            else {
                uiHidden = !uiHidden;
                EEPROM.begin(512);
                EEPROM.write(EEPROM_HIDDEN, (int)uiHidden);  
                EEPROM.end();
            }
        }
        hitTrigger = false;
        drawUI();
    }
    
    if (trigger == HIGH) {
        if (delayTrigger >= DELAY_RESET/DELAY_INTERVAL && !wasHoldToggled) {
            wasHoldToggled = true; 
            delayTrigger = 0;
            
            geigerHold = 0;
            isHoldActive = !isHoldActive;
             
            trigger = digitalRead(PIN_INTR_TRIGGER);
            if (trigger == LOW)  
                wasHoldToggled = false;     
        }
        else {
            delayTrigger++;
        }            
    }
    else {
        delayTrigger = 0;
    }
}

void processButton() {    
    int button = digitalRead(PIN_INTR_BUTTON);
    if (button == HIGH)
        buttonHeld = true;
    else
        buttonHeld = false;  
    
    if (hitButton) {
        if (button == LOW) {
            if (wasWiFiToggled) {
                wasWiFiToggled = false;
            }
        }
        hitButton = false;
        drawUI();
    }
    
    if (button == HIGH) {
        if (delayButton >= DELAY_WIFI/DELAY_INTERVAL && !wasWiFiToggled) {
            wasWiFiToggled = true;      
            delayButton = 0;
            
            if (twifiIsConnected()) {    
                stopWiFi();
                drawUI();
            }
            else {
                canRun = false;
                stopWiFi();
                startWiFi();
                canRun = true;
            }  
             
            button = digitalRead(PIN_INTR_BUTTON);
            if (button == LOW)  
                wasWiFiToggled = false;     
        }
        else {
            delayButton++;
        }            
    }
    else {
        delayButton = 0;
    }   
}

void hideUI() {
    display.clearDisplay();
    display.display();
}

int textCenterX(String text, int multi) {
    return (128 - text.length() * (6*multi)) / 2 - 1;
}

int textLeftX(String text, int x, int multi) {
    return 128 - text.length() * (6*multi) - x - 1;
}

void ICACHE_RAM_ATTR intrGeiger() {
    if (canRun) {
        geigerCounts++;
        hitGeiger = true;
    }
}

void ICACHE_RAM_ATTR intrTrigger() {
    if (canRun) {
        hitTrigger = true;
    }
}

void ICACHE_RAM_ATTR intrButton() {
    if (canRun) {
        hitButton = true;
    }
}

void connectAttempt(int idEntry, int attempt) {
    if (attempt % 2)
        uiStringWiFi = String(wifis[idEntry].ssid[0]) + String(wifis[idEntry].ssid[1]) + String(wifis[idEntry].ssid[2]) + "?";
    else 
        uiStringWiFi = "";
    drawUI();
}

void connectSuccess(int idEntry) {
    uiStringWiFi = String(wifis[idEntry].ssid[0]) + String(wifis[idEntry].ssid[1]) + String(wifis[idEntry].ssid[2]);
}

void connectFail(int idEntry) {
}

void setup() {
    Serial.begin(115200);
    
    canRun = false;
    setupGlobals();
    setupPins();
    setupDisplay();
    setupBattery();
    setupWiFi();
    canRun = true;
}

void loop() {
    processBattery();
    processDelays();
    processGeiger();
    processTrigger();
    processButton();
    
    delay(DELAY_INTERVAL);
}
