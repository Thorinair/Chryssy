#include "TwiFi.h"

#include <EEPROM.h>
#include "ESP8266WiFi.h"

#include "ConfigurationWiFi.h"

/****** [ADD CODE] Include additional header files as needed. ******/

#include "Shared.h"

/****** Stop including. ******/

/* Default Settings */
#define DEFAULT_WIFI 0

/* WiFi Attempt Duration */
#define WIFI_ATTEMPT_TIME 500



/* Functions */
String splitData(String data, char separator, int index);
IPAddress getIPFromString(char* addressString, int id);
void setupWiFiSettings();
int attemptWiFiConnection(char* ssid, char* pass, char* conf, bool isOpen);



/* Variables */
int prevWiFi;
int currWiFi;

/****** [ADD CODE] Define custom external variables you might need. ******/

extern String uiStringWiFi;

/****** Stop adding. ******/

/* Functions */
String splitData(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for(int i = 0; i <= maxIndex && found <= index; i++) {
        if(data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
} 

IPAddress getIPFromString(char* addressString, int id) {
    String address = splitData(String(addressString), '|', id);
    return IPAddress(splitData(address, '.', 0).toInt(), splitData(address, '.', 1).toInt(), splitData(address, '.', 2).toInt(), splitData(address, '.', 3).toInt());
}

void setupWiFiSettings() {    
    EEPROM.begin(512);
    int saved = EEPROM.read(EEPROM_SAVED);
    EEPROM.end();

    if (saved == 1) {
        if (WIFI_DEBUG)
            Serial.println("\nWiFi settings already exist. Loading...");
        EEPROM.begin(512);
        prevWiFi = currWiFi = (int) EEPROM.read(EEPROM_WIFI);
        EEPROM.end();
        if (WIFI_DEBUG)
            Serial.println("Loaded WiFi settings from EEPROM.");
    }
    else {   
        prevWiFi = currWiFi = DEFAULT_WIFI;
        if (WIFI_DEBUG)
            Serial.println("\nCreated new WiFi settings. Saving...");
        EEPROM.begin(512);
        EEPROM.write(EEPROM_SAVED, 1);
        EEPROM.write(EEPROM_WIFI,  prevWiFi);  
        EEPROM.end();
        if (WIFI_DEBUG)
            Serial.println("Saved WiFi settings to EEPROM.");       
    }
}

int attemptWiFiConnection(char* ssid, char* pass, char* conf, bool isOpen) {
    if (conf != "DHCP") {
        String address;
        
        IPAddress ipLocal =   getIPFromString(conf, 0);
        IPAddress ipGateway = getIPFromString(conf, 1);
        IPAddress ipSubnet =  getIPFromString(conf, 2);
        IPAddress ipDNS1 =    getIPFromString(conf, 3);
        IPAddress ipDNS2 =    getIPFromString(conf, 4);

        WiFi.config(ipLocal, ipGateway, ipSubnet, ipDNS1, ipDNS2);
    }
    if (isOpen)
        WiFi.begin(ssid, NULL);
    else    
        WiFi.begin(ssid, pass);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < (WIFI_TIMEOUT * 1000) / WIFI_ATTEMPT_TIME) {        
        delay(WIFI_ATTEMPT_TIME);
        if (WIFI_DEBUG)
            Serial.print(".");
        attempt++;
            
        /****** [ADD CODE] Insert the code to be executed while a connection is being attempted here. ******/
        
        if (attempt % 2)
            uiStringWiFi = String(ssid[0]) + String(ssid[1]) + String(ssid[2]) + "?";
        else 
            uiStringWiFi = "";
        drawUI();
        
        /****** Stop inserting. ******/
        
    }
    if (WiFi.status() == WL_CONNECTED) {
        if (WIFI_DEBUG)
            Serial.println("\nConnected to WiFi: '" + (String) ssid + "' With IP: " + WiFi.localIP());
        
        /****** [ADD CODE] Insert the code to be executed when a connection is successful. You can use this for very specific cases to access internal variables. ******/
        
        uiStringWiFi = String(ssid[0]) + String(ssid[1]) + String(ssid[2]);

        /****** Stop inserting. ******/
        
        return WIFI_RESULT_DONE;
    }
    else {
        if (WIFI_DEBUG)
            Serial.println("\nWiFi connection timed out."); 

        /****** [ADD CODE] Insert the code to be executed when a connection fails. You can use this for very specific cases to access internal variables. ******/



        /****** Stop inserting. ******/
        
        return WIFI_RESULT_FAIL;
    }
}



/* Public Functions */
int connectWiFi(bool repeatAttempts) {
    setupWiFiSettings();
    
    WiFi.mode(WIFI_STA);
    WiFi.hostname(WIFI_HOST);

    int result;
    int counter = 0;

    do {
        result = 0;
        switch (currWiFi) {
            case 0:  
                #ifdef WIFI_0_SSID         
                    result = attemptWiFiConnection(WIFI_0_SSID, WIFI_0_PASS, WIFI_0_CONF, WIFI_0_OPEN);
                #endif  
                break;  
            case 1:  
                #ifdef WIFI_1_SSID         
                    result = attemptWiFiConnection(WIFI_1_SSID, WIFI_1_PASS, WIFI_1_CONF, WIFI_1_OPEN);
                #endif  
                break;   
            case 2:  
                #ifdef WIFI_2_SSID         
                    result = attemptWiFiConnection(WIFI_2_SSID, WIFI_2_PASS, WIFI_2_CONF, WIFI_2_OPEN);
                #endif   
                break;
            case 3:  
                #ifdef WIFI_3_SSID         
                    result = attemptWiFiConnection(WIFI_3_SSID, WIFI_3_PASS, WIFI_3_CONF, WIFI_3_OPEN);
                #endif   
                break;
            case 4:  
                #ifdef WIFI_4_SSID         
                    result = attemptWiFiConnection(WIFI_4_SSID, WIFI_4_PASS, WIFI_4_CONF, WIFI_4_OPEN);
                #endif   
                break;
            case 5:  
                #ifdef WIFI_5_SSID         
                    result = attemptWiFiConnection(WIFI_5_SSID, WIFI_5_PASS, WIFI_5_CONF, WIFI_5_OPEN);
                #endif   
                break;
            case 6:  
                #ifdef WIFI_6_SSID         
                    result = attemptWiFiConnection(WIFI_6_SSID, WIFI_6_PASS, WIFI_6_CONF, WIFI_6_OPEN);
                #endif   
                break;
            case 7:  
                #ifdef WIFI_7_SSID         
                    result = attemptWiFiConnection(WIFI_7_SSID, WIFI_7_PASS, WIFI_7_CONF, WIFI_7_OPEN);
                #endif   
                break;
        }
        
        if (result == WIFI_RESULT_FAIL && counter >= 7 && !repeatAttempts)
            return WIFI_RESULT_FAIL;
            
        if (result == WIFI_RESULT_FAIL) {
            if (WIFI_DEBUG)
                Serial.println("\nWiFi " + String(currWiFi) + " failed..." );
            currWiFi++;
            if (currWiFi >= 8)
                currWiFi = 0;
            counter++;
            
            /****** [ADD CODE] Insert the code for handling failed attempts here. ******/


            
            /****** Stop inserting. ******/
            
        }
    } while (result == WIFI_RESULT_FAIL);   
    if (WIFI_DEBUG)     
        Serial.println("\nWiFi " + String(currWiFi) + " connected!" );
    
    if (currWiFi != prevWiFi) {
        if (WIFI_DEBUG)
            Serial.println("Saved settings to EEPROM.");     
        prevWiFi = currWiFi;
        EEPROM.begin(512);
        EEPROM.write(EEPROM_SAVED, 1);
        EEPROM.write(EEPROM_WIFI,  prevWiFi);  
        EEPROM.end();
    }

    return WIFI_RESULT_DONE;
}
